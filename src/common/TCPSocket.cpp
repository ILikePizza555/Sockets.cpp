//
// Created by avris on 9/27/2018.
//

#include <abl/abl.h>
#include <abl/TCPSocket.h>
#include <Error.h>

#ifdef unix
#include <unistd.h>
#endif

namespace sockets
{
    TCPSocket::TCPSocket(sock_t socket) : socket(socket) {}
    TCPSocket::TCPSocket(sockets::ip_family fam)
    {
        socket = ::socket(fam, sock_type::STREAM, sock_proto::TCP);
    }

    TCPSocket TCPSocket::accept() const
    {
        auto result = ::accept(this->socket, nullptr, nullptr);
        if (result == invalid_socket)
            throw MethodError("TCPSocket::accept", "accept");
        return TCPSocket(result);
    }

    std::tuple<TCPSocket, addr_t> TCPSocket::acceptfrom() const
    {
        auto* addr_ptr = new sockaddr_storage;
        socklen_t addr_len = sizeof(sockaddr_storage);
        auto result = ::accept(this->socket, reinterpret_cast<sockaddr*>(addr_ptr), &addr_len);
        if (result == invalid_socket)
            throw MethodError("TCPSocket::accept", "accept");

        return std::make_tuple(
                TCPSocket(result),
                addr_t{std::unique_ptr<sockaddr_storage>(addr_ptr), static_cast<size_t>(addr_len)}
                );
    }

    void
    TCPSocket::bind(const sockets::addr_t &addr)
    {
        auto result = ::bind(this->socket, addr.as_sockaddr(), addr.length);
        if (result == SOCKET_ERROR)
            throw MethodError("TCPSocket::bind", "bind");
    }

    void TCPSocket::connect(const sockets::addr_t &addr)
    {
        auto result = ::connect(this->socket, addr.as_sockaddr(), addr.length);
        if (result == SOCKET_ERROR)
            throw MethodError("TCPSocket::connect", "connect");
    }

    void TCPSocket::listen(int backlog)
    {
        auto result = ::listen(this->socket, backlog);
        if (result == SOCKET_ERROR)
            throw MethodError("TCPSocket::listen", "listen");
    }

    addr_t TCPSocket::getpeername() const
    {
        auto* addr_ptr = new sockaddr_storage;
        socklen_t addr_len = sizeof(sockaddr_storage);
        auto result = ::getpeername(this->socket, reinterpret_cast<sockaddr*>(addr_ptr), &addr_len);
        if (result == SOCKET_ERROR)
            throw MethodError("TCPSocket::getpeername", "getpeername");

        return addr_t{std::unique_ptr<sockaddr_storage>(addr_ptr), static_cast<size_t>(addr_len)};
    }

    addr_t TCPSocket::getsockname() const
    {
        auto* addr_ptr = new sockaddr_storage;
        socklen_t addr_len = sizeof(sockaddr_storage);
        auto result = ::getsockname(this->socket, reinterpret_cast<sockaddr*>(addr_ptr), &addr_len);
        if(result == SOCKET_ERROR)
            throw MethodError("TCPSocket::getsockname", "getsockname");

        return addr_t{std::unique_ptr<sockaddr_storage>(addr_ptr), static_cast<size_t>(addr_len)};
    }

    size_t TCPSocket::recv(ByteBuffer& buffer, size_t amount, size_t offset, int flags) const
    {
        // Resize the buffer to the maximum ammount
        buffer.resize(amount + offset);

        // Read
        ssize_t result = ::recv(socket,
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
        ssize_t result =  ::send(socket,
                                 reinterpret_cast<const char*>(buffer.data() + offset),
                                 static_cast<int>(buffer.size()),
                                 flags);
        if(result == SOCKET_ERROR)
            throw SocketWriteError("TCPSocket::send");

        return static_cast<size_t>(result);
    }

    int TCPSocket::close()
    {
#ifdef _WIN32
        return closesocket(this->socket);
#else
        return ::close(this->socket);
#endif
    }
}