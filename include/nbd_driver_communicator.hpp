
#ifndef __NBD_DRIVER_COMMUNICATOR_HPP__
#define __NBD_DRIVER_COMMUNICATOR_HPP__

#include <csignal> // sigaction, singal, sigaddset, sigemptyset, sigfullset

#include <string>    // std::string
#include <thread>    // std::thread
#include <stdexcept> // std::runtime_error

#include <sys/epoll.h> // epoll, epoll_event

#include "i_driver_communicator.hpp"
#include "driver_data.hpp"
#include "logger.hpp"
#include "file_descriptor.hpp"

namespace abc
{
    class NBDError : public DriverError
    {
    public:
        explicit NBDError(const std::string &str_) : DriverError(str_)
        {
            // Logger *logger = Singleton<Logger>::GetInstance();
            // logger->Log(Logger::log_level_t::ERROR, "nbd error: ", __FILE__, __LINE__);
        }
    };

    class NBDriverCommunicator : public IDriverCommunicator
    {
    public:
        explicit NBDriverCommunicator(const std::string &dev_path_,
                                      size_t storage_size_); // may throw bad_open, bad_read, bad_signal, bad_ioctl
        explicit NBDriverCommunicator(const std::string &dev_path_, size_t blocks_num_, size_t block_size_);
        ~NBDriverCommunicator() noexcept override;
        NBDriverCommunicator(const NBDriverCommunicator &other_) = delete;
        NBDriverCommunicator &operator=(const NBDriverCommunicator &other_) = delete;

        std::shared_ptr<DriverData> ReceiveRequest() override;      // may throw bad_read
        void SendReply(std::shared_ptr<DriverData> data_) override; // may throw bad_write
        void Disconnect() override;                                 // may throw bad_close, bad_ioctl, bad_join
        int GetRequestFD() const override;

    private:
        // used by the thread
        static void ListenerTask(NBDriverCommunicator *);

        // helper
        void NBDSetup(size_t storage_size_);
        void StartNBDThread();

        std::vector<FileDescriptor> m_sp;
        FileDescriptor m_nbd_fd;
        std::thread m_listener;
    };

    static volatile std::sig_atomic_t s_nbd_dev_to_disconnect;
} // namespace abc

#endif //__NBD_DRIVER_COMMUNICATOR_HPP__