
#ifndef __NETSPACE_PROTOCOL_HPP__
#define __NETSPACE_PROTOCOL_HPP__

#include <memory>
#include <csignal>
#include <vector>

#include "i_network_protocol.hpp"
#include "driver_data.hpp"

namespace abc
{

    class NetspaceProtocolError : public NetworkProtocolError
    {
    };

    class NetspaceProtocol : public INetworkProtocol
    {

    public:
        explicit NetspaceProtocol(std::shared_ptr<ISerializable> serializer_, int socket_);
        virtual ~NetspaceProtocol() noexcept = default;

        NetspaceProtocol(const NetspaceProtocol &other_) = delete;
        NetspaceProtocol &operator=(const NetspaceProtocol &other_) = delete;

        std::shared_ptr<DriverData> ReceiveRequest() override;
        void SendReply(std::shared_ptr<DriverData> data_) override;
        int GetRequestFD() override;

    private:
        Logger *m_logger;
        size_t m_msg_size;
    };

} // namespace abc 

#endif //__NETSPACE_PROTOCOL_HPP__