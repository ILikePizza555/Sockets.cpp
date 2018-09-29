//
// Created by avris on 9/27/2018.
//

#include <abl/abl.h>
#include <abl/TCPSocket.h>
#include <Error.h>


namespace sockets
{
    TCPSocket::TCPSocket(sock_t socket) : socket(socket) {}
    TCPSocket::TCPSocket(sockets::ip_family fam)
    {
        socket = ::socket(fam, sock_type::STREAM, sock_proto::TCP);
    }

    TCPSocket TCPSocket::accept()
    {
        auto result = ::accept(this->socket, nullptr, nullptr);
        if (result == invalid_socket)
            throw MethodError("TCPSocket::accept", "accept", WSAGetLastError(), get_error_message);
        return TCPSocket(result);
    }

    std::tuple<TCPSocket, addr_t> TCPSocket::acceptfrom()
    {
        auto* addr_ptr = new sockaddr_storage;
        socklen_t addr_len = sizeof(sockaddr_storage);
        auto result = ::accept(this->socket, reinterpret_cast<sockaddr*>(addr_ptr), &addr_len);
        if (result == invalid_socket)
            throw MethodError("TCPSocket::accept", "accept", WSAGetLastError(), get_error_message);

        return std::make_tuple(TCPSocket(result), addr_t{std::unique_ptr<sockaddr_storage>(addr_ptr), addr_len});
    }

    void
    TCPSocket::bind(sockets::addr_t &addr)
    {
        auto result = ::bind(this->socket, addr.as_sockaddr(), addr.length);
        if (result == SOCKET_ERROR)
            throw MethodError("TCPSocket::bind", "bind", WSAGetLastError(), get_error_message);
    }

    void TCPSocket::connect(sockets::addr_t &addr)
    {
        auto result = ::connect(this->socket, addr.as_sockaddr(), addr.length);
        if (result == SOCKET_ERROR)
            throw MethodError("TCPSocket::connect", "connect", WSAGetLastError(), get_error_message);
    }

    void TCPSocket::listen(int backlog)
    {
        auto result = ::listen(this->socket, backlog);
        if (result == SOCKET_ERROR)
            throw MethodError("TCPSocket::listen", "listen", WSAGetLastError(), get_error_message);
    }

    addr_t TCPSocket::getpeername()
    {
        auto* addr_ptr = new sockaddr_storage;
        socklen_t addr_len = sizeof(sockaddr_storage);
        auto result = ::getpeername(this->socket, reinterpret_cast<sockaddr*>(addr_ptr), &addr_len);
        if (result == SOCKET_ERROR)
            throw MethodError("TCPSocket::getpeername", "getpeername", WSAGetLastError(), get_error_message);

        return addr_t{std::unique_ptr<sockaddr_storage>(addr_ptr), addr_len};
    }

    addr_t TCPSocket::getsockname()
    {
        auto* addr_ptr = new sockaddr_storage;
        socklen_t addr_len = sizeof(sockaddr_storage);
        auto result = ::getsockname(this->socket, reinterpret_cast<sockaddr*>(addr_ptr), &addr_len);
        if(result == SOCKET_ERROR)
            throw MethodError("TCPSocket::getsockname", "getsockname", WSAGetLastError(), get_error_message);

        return addr_t{std::unique_ptr<sockaddr_storage>(addr_ptr), addr_len};
    }

    size_t TCPSocket::recv(ByteBuffer& buffer, size_t amount, size_t offset, int flags)
    {
        // Resize the buffer to the maximum ammount
        buffer.resize(amount + offset);

        // Read
        ssize_t result = ::recv(socket,
                                reinterpret_cast<char*>(buffer.data() + offset),
                                static_cast<int>(amount),
                                flags);
        if(result == SOCKET_ERROR)
            throw MethodError("TCPSocket::recv", "recv", WSAGetLastError(), get_error_message);

        // Resize to the read amount. This correctly sets size() on the buffer.
        buffer.resize(static_cast<size_t>(result) + offset);
        return result;
    }

    size_t TCPSocket::send(const ByteBuffer& buffer, size_t offset, int flags)
    {
        ssize_t result =  ::send(socket,
                                 reinterpret_cast<const char*>(buffer.data()),
                                 static_cast<int>(buffer.size()),
                                 flags);
        if(result == SOCKET_ERROR)
            throw MethodError("TCPSocket::send", "send", WSAGetLastError(), get_error_message);
        
        return result;
    }

    int TCPSocket::close()
    {
        return closesocket(this->socket);
    }
};