
#include <unistd.h> // close
#include <cstring>  // strcmp

#include <sstream>   // std::stringstream
#include <iostream>  // std::cout
#include <exception> // std::exception

#include "re_fw.hpp"
#include "nbd_driver_communicator.hpp"
#include "ram_storage.hpp"
#include "driver_data.hpp"
#include "utils.h"

/*************************************************************************
 * defines / enums
 *************************************************************************/
#define UNUSED(x) (void)x

using namespace abc;
using SHARED_PTR_DD = std::shared_ptr<DriverData>;

struct SrcArgs
{
    explicit SrcArgs(
        RequestEngine<std::string, SrcArgs> &re_,
        RAMStorage &ram_storage_,
        NBDriverCommunicator &nbd_comm_,
        SHARED_PTR_DD dd_)
        : re(re_),
          ram_storage(ram_storage_),
          nbd_comm(nbd_comm_),
          dd(dd_) {}

    RequestEngine<std::string, SrcArgs> &re;
    RAMStorage &ram_storage;
    NBDriverCommunicator &nbd_comm;
    SHARED_PTR_DD dd;
};

const bool g_is_server_running = true;

enum
{
    SETUP_TIME = 1,
    EVENT_NUMBER = 10,
    KB = 1024,
    MB = 1024 * KB
};

/******************************************************************/
/******************     STDIN   ***********************************/
/******************************************************************/
class StdinTask : public IRETask
{
public:
    static std::shared_ptr<StdinTask> CreateTask(
        SrcArgs args_)
    {
        return std::make_shared<StdinTask>(
            args_.re,
            args_.ram_storage,
            args_.nbd_comm,
            args_.dd);
    }

    void Execute() override
    {
        char buf = 0;
        read(STDIN_FILENO, &buf, 1);
        if (buf == 'q')
        {
            m_re.Stop();
            m_nbd_comm.Disconnect(); // system_error, NBDError, FDError
        }
    }

    explicit StdinTask(
        RequestEngine<std::string, SrcArgs> &re_,
        RAMStorage &ram_storage_,
        NBDriverCommunicator &nbd_comm_,
        SHARED_PTR_DD dd_)
        // ----------------------------------MIL
        : m_re(re_),
          m_ram_storage(ram_storage_),
          m_nbd_comm(nbd_comm_),
          m_dd(dd_)
    {
    }

private:
    RequestEngine<std::string, SrcArgs> &m_re;
    RAMStorage &m_ram_storage;
    NBDriverCommunicator &m_nbd_comm; // NBDError, system_error
    SHARED_PTR_DD m_dd;
};

template <typename KEY, typename ARGS>
class StdinSrc : public IInputSrc<KEY, ARGS>
{
public:
    explicit StdinSrc(RequestEngine<std::string, SrcArgs> &re_,
                      RAMStorage &ram_storage_,
                      NBDriverCommunicator &nbd_comm_) : m_re(re_), m_ram_storage(ram_storage_), m_nbd_comm(nbd_comm_) {}
    // key, args
    std::pair<KEY, ARGS> Read() override
    {
        return std::make_pair("STDIN", ARGS(m_re, m_ram_storage, m_nbd_comm, nullptr));
    }

    int GetFd() override
    {
        return STDIN_FILENO;
    }

private:
    RequestEngine<std::string, SrcArgs> &m_re;
    RAMStorage &m_ram_storage;
    NBDriverCommunicator &m_nbd_comm;
};

/******************************************************************/
/******************     NBD      **********************************/
/******************************************************************/
class NBDTask : public IRETask
{
public:
    static std::shared_ptr<NBDTask> CreateTask(
        SrcArgs args_)
    {
        return std::make_shared<NBDTask>(
            args_.ram_storage,
            args_.nbd_comm,
            args_.dd);
    }

    void Execute() override
    {
        switch (m_dd->m_type)
        {
        case READ:
            m_ram_storage.Read(m_dd);
            break;
        case WRITE:
            m_ram_storage.Write(m_dd);
            break;
        }

        std::lock_guard<std::mutex> Lock(m_mutex);
        // protect with mutex
        m_nbd_comm.SendReply(m_dd); // may throw bad write
    }

    explicit NBDTask(
        RAMStorage &ram_storage_,
        NBDriverCommunicator &nbd_comm_,
        SHARED_PTR_DD dd_)
        // ----------------------------------MIL
        : m_ram_storage(ram_storage_),
          m_nbd_comm(nbd_comm_),
          m_dd(dd_),
          m_mutex()
    {
    }

private:
    RAMStorage &m_ram_storage;
    NBDriverCommunicator &m_nbd_comm; // NBDError, system_error
    SHARED_PTR_DD m_dd;
    std::mutex m_mutex;
};

template <typename KEY, typename ARGS>
class NBDSrc : public IInputSrc<KEY, ARGS>
{
public:
    explicit NBDSrc(RequestEngine<std::string, SrcArgs> &re_,
                    RAMStorage &ram_storage_,
                    NBDriverCommunicator &nbd_comm_)
        : m_re(re_), m_ram_storage(ram_storage_), m_nbd_comm(nbd_comm_), m_mutex()
    {
    }
    // key, args
    // key of the task and its args
    std::pair<KEY, ARGS> Read() override
    {
        SHARED_PTR_DD dd_request = m_nbd_comm.ReceiveRequest();
        
        return std::make_pair("NBD", ARGS(m_re,
                                          m_ram_storage,
                                          m_nbd_comm,
                                          dd_request));
    }

    int GetFd() override
    {
        return m_nbd_comm.GetRequestFD();
    }

private:
    RequestEngine<std::string, SrcArgs> &m_re;
    RAMStorage &m_ram_storage;
    NBDriverCommunicator &m_nbd_comm;
    std::mutex m_mutex;
};
/*************************************************************************
 * main()
 *************************************************************************/

int main(int argc, char **argv)
{
    UNUSED(argc);
    try
    {
        Logger *g_logger = Singleton<Logger>::GetInstance();
        g_logger->SetFilePath();
        g_logger->SetLogLevel(Logger::INFO);
        const std::string &device_name(argv[1]);

        size_t storage_size = 128 * MB;

        RAMStorage ram_storage(storage_size);

        NBDriverCommunicator nbd_comm(device_name, storage_size); // NBDError, system_error
        // RequestEngineOld(nbd_comm, ram_storage); // system_error, NBDError, FDError

        // request engine
        std::cout << "creating fw\n";
        RequestEngine<std::string, SrcArgs> re(4);

        using SRC_INPUT = StdinSrc<std::string, SrcArgs>;
        using NBD_INPUT = NBDSrc<std::string, SrcArgs>;

        // add stdin and nbd
        re.ConfigInputSrc(std::make_shared<NBD_INPUT>(re, ram_storage, nbd_comm));
        re.ConfigInputSrc(std::make_shared<SRC_INPUT>(re, ram_storage, nbd_comm));

        // add their respective tasks
        re.ConfigTask("NBD", NBDTask::CreateTask);
        re.ConfigTask("STDIN", StdinTask::CreateTask);

        re.Run();

        // re.Stop();

        // sleep(1);
    }
    catch (std::system_error &e)
    {
        std::cout << "system_error" << std::endl;
    }
    catch (std::runtime_error &e) // NBDERROR
    {
        std::cout << e.what() << std::endl;
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
