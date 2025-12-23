
#ifndef __MASTER_DRIVER_COMMUNICATOR_HPP__
#define __MASTER_DRIVER_COMMUNICATOR_HPP__

#include <mutex>
#include <memory>

#include "i_driver_communicator.hpp"
#include "logger.hpp"
#include "i_network_protocol.hpp"

namespace abc
{
    class MasterDriverCommunicator : public IDriverCommunicator
    {
    public:
        explicit MasterDriverCommunicator(const std::string &ip_,
                                          const std::string &port_, const std::string &protocol_ = "UDP");
        ~MasterDriverCommunicator() noexcept override;

        MasterDriverCommunicator(const MasterDriverCommunicator &other_) = delete;
        MasterDriverCommunicator &operator=(const MasterDriverCommunicator &other_) = delete;

        std::shared_ptr<DriverData> ReceiveRequest() override;
        void SendReply(std::shared_ptr<DriverData> data_) override;
        void Disconnect() override;
        int GetRequestFD() const override;

    private:
        std::shared_ptr<INetworkProtocol> m_protocol;
        Logger *m_logger;
    };
} // namespace abc

#endif //__MASTER_DRIVER_COMMUNICATOR_HPP__