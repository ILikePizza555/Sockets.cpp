//
// Created by avris on 9/27/2018.
//

#include <abl/system.h>
#include <abl/enums.h>
#include <sockets/TCPSocket.h>
#include <sockets/Error.h>

#ifdef unix
#include <netinet/in.h>

#define SOCKET_ERROR -1
#endif

namespace sockets
{
    using namespace abl;

    TCPSocket::TCPSocket() : handle(nullptr, &abl::close_handle) {}

    TCPSocket::TCPSocket(abl::UniqueHandle&& handle) : handle(std::move(handle)) {}

    TCPSocket::TCPSocket(abl::ip_family fam) : handle(abl::new_unique_handle(fam, sock_type::STREAM, sock_proto::TCP)) {}

    bool TCPSocket::operator==(sockets::TCPSocket &other)
    {
        return this->handle == other.handle;
    }

    bool TCPSocket::invalid() const
    {
        return this->handle == nullptr;
    }

    TCPSocket TCPSocket::accept() const
    {
        ssize_t result = ::accept(system::get_system_handle(this->handle), nullptr, nullptr);
        if (result == SOCKET_ERROR)
            throw MethodError("TCPSocket::accept", "accept");
        return TCPSocket(system::unique_from_system_handle(static_cast<int>(result)));
    }

    std::tuple<TCPSocket, IpAddress> TCPSocket::acceptfrom() const
    {
        std::unique_ptr<sockaddr_storage> addr_ptr = std::make_unique<sockaddr_storage>();
        socklen_t addr_len = sizeof(sockaddr_storage);

        ssize_t result = ::accept(system::get_system_handle(this->handle),
                               reinterpret_cast<sockaddr*>(addr_ptr.get()),
                               &addr_len);
        if (result == SOCKET_ERROR)
            throw MethodError("TCPSocket::accept", "accept");


        return std::make_tuple(
                    TCPSocket(system::unique_from_system_handle(static_cast<int>(result))),
                    abl::system::to_ipaddress(reinterpret_cast<sockaddr *>(addr_ptr.get())));
    }

    void
    TCPSocket::bind(const IpAddress &addr)
    {
        ssize_t bind_result = -1;
        if(addr.is_ipv4())
        {
            auto sys_sockaddr = system::from_ipv4(addr.get_as_ipv4());
            bind_result = ::bind(system::get_system_handle(this->handle),
                                 reinterpret_cast<const sockaddr*>(&sys_sockaddr),
                                 sizeof(sys_sockaddr));
        }
        else if (addr.is_ipv6())
        {
            auto sys_sockaddr = system::from_ipv6(addr.get_as_ipv6());
            bind_result = ::bind(system::get_system_handle(this->handle),
                                 reinterpret_cast<const sockaddr*>(&sys_sockaddr),
                                 sizeof(sys_sockaddr));
        }

        if (bind_result == SOCKET_ERROR)
            throw MethodError("TCPSocket::bind", "bind");
    }

    void TCPSocket::connect(const IpAddress &addr)
    {
        ssize_t connect_result = -1;
        if(addr.is_ipv4())
        {
            auto sys_sockaddr = system::from_ipv4(addr.get_as_ipv4());
            connect_result = ::connect(system::get_system_handle(this->handle),
                                       reinterpret_cast<sockaddr*>(&sys_sockaddr),
                                       sizeof(sys_sockaddr));
        }
        else if(addr.is_ipv6())
        {
            auto sys_sockaddr = system::from_ipv6(addr.get_as_ipv6());
            connect_result = ::connect(system::get_system_handle(this->handle),
                                       reinterpret_cast<sockaddr*>(&sys_sockaddr),
                                       sizeof(sys_sockaddr));
        }

        if (connect_result == SOCKET_ERROR)
            throw MethodError("TCPSocket::connect", "connect");
    }

    void TCPSocket::listen(int backlog)
    {
        auto result = ::listen(system::get_system_handle(this->handle), backlog);
        if (result == SOCKET_ERROR)
            throw MethodError("TCPSocket::listen", "listen");
    }

    IpAddress TCPSocket::getpeername() const
    {
        std::unique_ptr<sockaddr_storage> addr_ptr = std::make_unique<sockaddr_storage>();
        socklen_t addr_len = sizeof(sockaddr_storage);

        auto result = ::getpeername(system::get_system_handle(this->handle),
                                    reinterpret_cast<sockaddr*>(addr_ptr.get()),
                                    &addr_len);
        if (result == SOCKET_ERROR)
            throw MethodError("TCPSocket::getpeername", "getpeername");

        return system::to_ipaddress(reinterpret_cast<sockaddr*>(addr_ptr.get()));
    }

    IpAddress TCPSocket::getsockname() const
    {
        std::unique_ptr<sockaddr_storage> addr_ptr = std::make_unique<sockaddr_storage>();
        socklen_t addr_len = sizeof(sockaddr_storage);

        auto result = ::getsockname(system::get_system_handle(this->handle),
                                    reinterpret_cast<sockaddr*>(addr_ptr.get()),
                                    &addr_len);
        if(result == SOCKET_ERROR)
            throw MethodError("TCPSocket::getsockname", "getsockname");

        return system::to_ipaddress(reinterpret_cast<sockaddr*>(addr_ptr.get()));
    }

    size_t TCPSocket::recv(ByteBuffer& buffer, size_t amount, size_t offset, int flags) const
    {
        // Resize the buffer to the maximum ammount
        buffer.resize(amount + offset);

        // Read
        ssize_t result = ::recv(system::get_system_handle(this->handle),
                                reinterpret_cast<char*>(buffer.data() + offset),
                                static_cast<int>(amount),
                                flags);
        if(result == SOCKET_ERROR)
            throw SocketReadError("TCPSocket::recv");

        // Resize to the read amount. This correctly sets size() on the buffer.
        buffer.resize(static_cast<size_t>(result) + offset);
        return static_cast<size_t>(result);
    }

    size_t TCPSocket::send(const ByteBuffer& buffer, size_t offset, int flags) const
    {
        ssize_t result =  ::send(system::get_system_handle(this->handle),
                                 reinterpret_cast<const char*>(buffer.data() + offset),
                                 static_cast<int>(buffer.size()),
                                 flags);
        if(result == SOCKET_ERROR)
            throw SocketWriteError("TCPSocket::send");

        return static_cast<size_t>(result);
    }
}