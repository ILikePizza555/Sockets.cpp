//
// Created by avris on 9/5/2018.
//

#include <Socket.h>
#include <SocketException.h>

sockets::Socket::Socket(sockets::sock_t socket) : socket(socket) {}

sockets::Socket::Socket(sockets::sock_addr_family family,
                        sockets::sock_type type,
                        sockets::sock_proto protocol)
{
    socket = ::socket(family, type, protocol);
    if(socket == invalid_socket) throw SocketError("Socket", __func__, "Error creating socket.", get_error_code());
}

sockets::Socket
sockets::Socket::accept(sockets::sockaddr_t* address, sockets::sockaddr_len_t* address_length)
{
    return ::accept(socket, address, address_length);
}

int
sockets::Socket::bind(const sockets::sockaddr_t *address, sockets::sockaddr_len_t address_length)
{
    return ::bind(socket, address, address_length);
}

int
sockets::Socket::connect(const sockets::sockaddr_t *address, sockets::sockaddr_len_t address_length)
{
    return ::connect(socket, address, address_length);
}

int
sockets::Socket::getpeername(sockets::sockaddr_t *address_out, sockets::sockaddr_len_t *address_length_out)
{
    return ::getpeername(socket, address_out, address_length_out);
}

int
sockets::Socket::getsockname(sockets::sockaddr_t *address_out, sockets::sockaddr_len_t *address_length_out)
{
    return ::getsockname(socket, address_out, address_length_out);
}

int
sockets::Socket::listen(int backlog)
{
    return ::listen(socket, backlog);
}

ssize_t
sockets::Socket::recvfrom(byte *buffer,
                          size_t length,
                          int flags,
                          sockets::sockaddr_t *address,
                          sockets::sockaddr_len_t *address_len)
{
    return ::recvfrom(socket,
            reinterpret_cast<sock_buff_t>(buffer),
            static_cast<s_sock_buff_t>(length),
            flags, address, address_len);
}

ssize_t
sockets::Socket::recvfrom(sockets::sock_buff_t buffer,
                          sockets::s_sock_buff_t length,
                          int flags,
                          sockets::sockaddr_t *address,
                          sockets::sockaddr_len_t *address_len)
{
    return ::recvfrom(socket, buffer, length, flags, address, address_len);
}

ssize_t
sockets::Socket::recv(sockets::sock_buff_t buffer, sockets::s_sock_buff_t length, int flags)
{
    return ::recv(socket, buffer, length, flags);
}

ssize_t
sockets::Socket::recv(byte *buffer, size_t length, int flags)
{
    return ::recv(socket,
            reinterpret_cast<sock_buff_t>(buffer),
            static_cast<s_sock_buff_t>(length),
            flags);
}

ssize_t
sockets::Socket::sendto(const sockets::sock_buff_t buffer, sockets::s_sock_buff_t length, int flags,
                        const sockets::sockaddr_t *address, socklen_t address_len)
{
    return ::sendto(socket, buffer, length, flags, address, address_len);
}

ssize_t
sockets::Socket::sendto(byte *buffer, size_t length, int flags, const sockets::sockaddr_t *address,
                        socklen_t address_len)
{
    return ::sendto(socket,
            reinterpret_cast<sock_buff_t>(buffer),
            static_cast<s_sock_buff_t>(length),
            flags, address, address_len);
}

ssize_t
sockets::Socket::send(const sockets::sock_buff_t buffer, sockets::s_sock_buff_t length, int flags)
{
    return ::send(socket, buffer, length, flags);
}

ssize_t
sockets::Socket::send(byte *buffer, size_t length, int flags)
{
    return ::send(socket, reinterpret_cast<sock_buff_t>(buffer), static_cast<s_sock_buff_t>(length), flags);
}

int
sockets::Socket::close()
{
#ifdef _WIN32
    return closesocket(socket);
#else
    return ::close(socket);
#endif
}
