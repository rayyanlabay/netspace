

#ifndef __I_NETWORK_PROTOCOL_HPP__
#define __I_NETWORK_PROTOCOL_HPP__

#include <memory>
#include <mutex>
#include <vector>

#include "driver_data.hpp"
#include "i_serializable.hpp"
#include "driver_data.hpp"

namespace abc
{

    class NetworkProtocolError : public NetspaceError
    {
    };

    class INetworkProtocol
    {

    public:
        explicit INetworkProtocol(std::shared_ptr<ISerializable> serializer_, int socket_);
        virtual ~INetworkProtocol() noexcept = default;

        INetworkProtocol(const INetworkProtocol &other_) = delete;
        INetworkProtocol &operator=(const INetworkProtocol &other_) = delete;

        virtual std::shared_ptr<DriverData> ReceiveRequest() = 0;
        virtual void SendReply(std::shared_ptr<DriverData> data_) = 0;
        virtual int GetRequestFD() = 0;

    protected:
        int m_socket;
        std::shared_ptr<ISerializable> m_serializer;
        std::mutex m_mutex;
    };

} // namespace abc

#endif //__I_NETWORK_PROTOCOL_HPP__