/*
Description: nbd driver communicator
Author: Rayan
Reviewer: -
Version:
    v0.1 - For mentor approval
    v0.4 - upgrading legacy to modern cpp
*/

/*
status:

     done:
     excpetions

     todo:
     modern
     include commentry
     general commentry
*/

#include <unistd.h> // close, read, write
#include <fcntl.h>  // open
#include <cassert>  // assert
#include <stdio.h>  //
#include <errno.h>

#include <string>    // std::string
#include <thread>    // std::thread
#include <stdexcept> // std::runtime_error
#include <iostream>  // std::cout
#include <vector>    // std::vector

#include <arpa/inet.h>

#include <sys/ioctl.h>
#include <sys/socket.h> // socketpair
#include <sys/types.h>
#include <sys/stat.h>

#include "nbd_driver_communicator.hpp"
#include "driver_data.hpp"

#include "utils.h"
/*************************************************************************
 * defines / enums
 *************************************************************************/
enum
{
    NBD_SOCKET = 1,
    APP_SOCKET = 0,
    EXPLICIT_CALL_TO_SIG_HANLDER = 77
};

namespace hrd31
{
    /*******************
     internal namespace
     holds aux functinos
    ******************/
    namespace internal
    {
        /***********
         read_all()
        ***********/
        static int read_all(int fd, char *buf, size_t count)
        {
            int bytes_read;

            while (count > 0)
            {
                bytes_read = read(fd, buf, count);
                assert(bytes_read > 0);

                CheckFailAndThrow<NBDError>(bytes_read > 0, "read from buffer failed", __FILE__, __LINE__);

                buf += bytes_read;
                count -= bytes_read;
            }

            assert(count == 0);

            return 0;
        }

        /***********
         write_all()
        ***********/
        static int write_all(int fd, char *buf, size_t count)
        {
            int bytes_written;
            while (count > 0)
            {
                bytes_written = write(fd, buf, count);
                assert(bytes_written > 0);

                CheckFailAndThrow<NBDError>(bytes_written > 0, "write to buffer failed", __FILE__, __LINE__);

                buf += bytes_written;
                count -= bytes_written;
            }

            assert(count == 0);

            return 0;
        }

        /*******************************
         BlockSignals()
        *******************************/
        static void BlockSignals()
        {
            sigset_t sigset;

            CheckFailAndThrow<NBDError>(
                -1 != sigfillset(&sigset), "parent thread blocking signals failed", __FILE__, __LINE__);
            CheckFailAndThrow<NBDError>(
                0 == pthread_sigmask(SIG_BLOCK, &sigset, NULL), "parent thread pthread_mask error", __FILE__, __LINE__);
        }

        /**************
         Set_sigaction()
        **************/
        static int set_sigaction(int sig, const struct sigaction *act)
        {
            struct sigaction oact;
            int r = sigaction(sig, act, &oact);

            CheckFailAndThrow<NBDError>(r != -1, "nbd thread: int r = sigaction(sig, act, &oact)", __FILE__, __LINE__);

            if (r == 0 && oact.sa_handler != SIG_DFL)
            {
                std::cout << "overriden non-default signal handler" << sig << strsignal(sig) << "\n";
            }

            return r;
        }

        /*******************************
         signal handler disconnect_nbd()
        *******************************/
        static void disconnect_nbd(int signal)
        {
            UNUSED(signal);

            assert(signal == SIGTERM || signal == SIGINT || signal == EXPLICIT_CALL_TO_SIG_HANLDER);

            int nbd_fd = s_nbd_dev_to_disconnect;

            CheckFailAndThrow<NBDError>(nbd_fd != -1, "nbd fd is undefined", __FILE__, __LINE__);

            CheckFailAndThrow<NBDError>(
                ioctl(nbd_fd, NBD_DISCONNECT) != -1,
                "sys failed to request disconect on nbd device", __FILE__, __LINE__);

            std::cout << "sucessfuly requested disconnect on nbd device\n";
        }

        /************************
         SetSignalHandlerForApp()
        ************************/
        static void SetSignalHandlerForApp()
        {
            sigset_t sigset;

            CheckFailAndThrow<NBDError>(
                -1 != sigemptyset(&sigset), "nbd thread sigemptyset error", __FILE__, __LINE__);
            CheckFailAndThrow<NBDError>(
                -1 != sigaddset(&sigset, SIGINT), "nbd thread sigaddset error", __FILE__, __LINE__);

            CheckFailAndThrow<NBDError>(
                -1 != sigaddset(&sigset, SIGTERM), "nbd thread sigaddset error", __FILE__, __LINE__);

            CheckFailAndThrow<NBDError>(
                0 == pthread_sigmask(SIG_UNBLOCK, &sigset, NULL), "nbd thread pthread_mask error", __FILE__, __LINE__);

            struct sigaction act;
            act.sa_handler = disconnect_nbd;
            act.sa_flags = SA_RESTART;

            CheckFailAndThrow<NBDError>(
                -1 != sigemptyset(&act.sa_mask), "nbd thread: -1 != sigemptyset(&act.sa_mask)", __FILE__, __LINE__);

            set_sigaction(SIGINT, &act);
            set_sigaction(SIGTERM, &act);
        }

        /***********
         SetSockets()
        ***********/
        static std::vector<FileDescriptor> SetSockets()
        {
            std::vector<FileDescriptor> ret(2);

            int sp[2];
            int err = socketpair(AF_UNIX, SOCK_STREAM, 0, sp);
            assert(!err);

            CheckFailAndThrow<NBDError>(-1 != err, "socketpair", __FILE__, __LINE__);

            ret[APP_SOCKET] = FileDescriptor(sp[APP_SOCKET]);
            ret[NBD_SOCKET] = FileDescriptor(sp[NBD_SOCKET]);

            return ret;
        }

    } // namespace internal

    /**************
     ListenerTask()
    **************/
    void NBDriverCommunicator::ListenerTask(NBDriverCommunicator *this_)
    {
        int nbd_fd = int(this_->m_nbd_fd);
        int nbd_socket = int(this_->m_sp[NBD_SOCKET]);

        CheckFailAndThrow<NBDError>(-1 != ioctl(nbd_fd, NBD_SET_SOCK, nbd_socket),
                                    "-1 != ioctl(nbd_fd, NBD_SET_SOCK, nbd_socket)", __FILE__, __LINE__);
        // TODO set flags
        ioctl(nbd_fd, NBD_DO_IT);

        CheckFailAndThrow<NBDError>(-1 != ioctl(nbd_fd, NBD_CLEAR_QUE), "EXIT_FAILURE", __FILE__, __LINE__);
        CheckFailAndThrow<NBDError>(-1 != ioctl(nbd_fd, NBD_CLEAR_SOCK), "EXIT_FAILURE", __FILE__, __LINE__);
    }

    /**********
     NBDSetup()
    **********/
    void NBDriverCommunicator::NBDSetup(size_t storage_size_)
    {
        CheckFailAndThrow<NBDError>(
            -1 != ioctl(int(m_nbd_fd), NBD_SET_SIZE, storage_size_), "nbd set size fail", __FILE__, __LINE__);

        CheckFailAndThrow<NBDError>(
            -1 != ioctl(int(m_nbd_fd), NBD_CLEAR_SOCK), "sys nbd clear sock", __FILE__, __LINE__);
    }

    /****************
     StartNBDThread()
    ****************/
    void NBDriverCommunicator::StartNBDThread()
    {
        m_listener = std::thread(ListenerTask, this);
        s_nbd_dev_to_disconnect = int(m_nbd_fd);
    }

    /*******************************
     NBDriverConstructor(total_size)
    *******************************/
    NBDriverCommunicator::
        NBDriverCommunicator(const std::string &dev_path_, size_t storage_size_)
        : m_sp(internal::SetSockets()), m_nbd_fd(dev_path_)
    {
        NBDSetup(storage_size_);
        internal::BlockSignals();
        StartNBDThread();
        internal::SetSignalHandlerForApp();
    }

    /***************************************************
     NBDriverCommunicator(block_size, number_of_blocks)
    ***************************************************/
    NBDriverCommunicator::NBDriverCommunicator(
        const std::string &dev_path_,
        size_t blocks_num_,
        size_t block_size_) : NBDriverCommunicator(dev_path_, blocks_num_ * block_size_) {}

    /********************
     NBDriverDestructor()
    ********************/
    NBDriverCommunicator::~NBDriverCommunicator() noexcept
    {
        try
        {
            Disconnect();
        }
        catch (...)
        {
            std::cerr << "exception was detected during dtor";
        }
    }

    /*****************
     RecieveRequest()
    *****************/
    std::shared_ptr<DriverData> NBDriverCommunicator::ReceiveRequest()
    {
        nbd_request request;
        size_t bytes_read = 0;

        // is called in read
        bytes_read = read(int(m_sp[APP_SOCKET]), &request, sizeof(request));

        CheckFailAndThrow<NBDError>(bytes_read > 0, "bad read", __FILE__, __LINE__);

        // internal::PrintRequest(request);

        DriverData *dd = new DriverData(request);

        if (dd->m_type == WRITE)
        {
            internal::read_all(int(m_sp[APP_SOCKET]), dd->m_data.data(), dd->m_len);
        }

        return std::shared_ptr<DriverData>(dd);
    }

    /*****************
     SendReply()
    *****************/
    void NBDriverCommunicator::SendReply(std::shared_ptr<DriverData> data_)
    {
        struct nbd_reply reply;
        reply.magic = htonl(NBD_REPLY_MAGIC);
        reply.error = htonl(0);
        *(size_t *)(reply.handle) = data_->m_handle;

        internal::write_all(int(m_sp[APP_SOCKET]), (char *)&reply, sizeof(struct nbd_reply));

        if (data_->m_type == READ)
        {
            internal::write_all(int(m_sp[APP_SOCKET]), (char *)data_->m_data.data(), data_->m_len);
        }
    }

    /*****************
     Disconnect()
    *****************/
    void NBDriverCommunicator::Disconnect()
    {
        try
        {
            internal::disconnect_nbd(EXPLICIT_CALL_TO_SIG_HANLDER); // can throw system error
            m_sp[APP_SOCKET].CloseFD();
            m_nbd_fd.CloseFD();
        }
        catch (FDError &e)
        {
            // if file descriptor closing failed
            // it means disconnect happened, join thread gracefuly
            if(m_listener.joinable())
                m_listener.join();
            system("nbd-client -d /dev/nbd0");
        }

        utils_print::Red();
        printf("\ndisconnecting (not gracefuly)%s\n\n", WHT);
    }

    /*****************
     GetRequestFD()
    *****************/
    int NBDriverCommunicator::GetRequestFD() const
    {
        return int(m_sp[APP_SOCKET]);
    }

} // namespace hrd31
