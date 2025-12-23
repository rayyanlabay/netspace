#ifndef __NETSPACE_UDP_HPP__
#define __NETSPACE_UDP_HPP__

class NetspaceUdp
{
    public:
    explicit NetspaceUdp(int socket_) : m_in_socket(socket_) {}

    private:
    
    int m_in_socket;
};


#endif /* __NETSPACE_UDP_HPP__ */