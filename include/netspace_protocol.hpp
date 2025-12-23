/*
Description: Netspace protocol
Author: HRD31
Reviewer:
Version:
    v0.1 - For mentor approval
*/

#ifndef __HRD31_NETSPACE_PROTOCOL_HPP__
#define __HRD31_NETSPACE_PROTOCOL_HPP__

#include <memory>
#include <csignal>
#include <vector>

#include "i_network_protocol.hpp"
#include "driver_data.hpp"

namespace hrd31
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

} // namespace hrd31

#endif //__HRD31_NETSPACE_PROTOCOL_HPP__