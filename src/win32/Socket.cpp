//
// Created by avris on 9/5/2018.
//

#include <winsock2.h>
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
sockets::Socket::accept(sockets::i_addr_ptr address, sockets::i_addr_len_t* address_length)
{
    return ::accept(socket, address, address_length);
}

int
sockets::Socket::bind(sockets::i_addr_cptr address, sockets::i_addr_len_t address_length)
{
    return ::bind(socket, address, address_length);
}

int
sockets::Socket::connect(sockets::i_addr_cptr address, sockets::i_addr_len_t address_length)
{
    return ::connect(socket, address, address_length);
}

int
sockets::Socket::getpeername(sockets::i_addr_ptr address_out, sockets::i_addr_len_t *address_length_out)
{
    return ::getpeername(socket, address_out, address_length_out);
}

int
sockets::Socket::getsockname(sockets::i_addr_ptr address_out, sockets::i_addr_len_t *address_length_out)
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
                          sockets::i_addr_ptr address,
                          sockets::i_addr_len_t *address_len)
{
    return ::recvfrom(socket,
            reinterpret_cast<i_buff_t>(buffer),
            static_cast<i_buff_len_t>(length),
            flags, address, address_len);
}

ssize_t
sockets::Socket::recvfrom(sockets::i_buff_t buffer,
                          sockets::i_buff_len_t length,
                          int flags,
                          sockets::i_addr_ptr address,
                          sockets::i_addr_len_t *address_len)
{
    return ::recvfrom(socket, buffer, length, flags, address, address_len);
}

ssize_t
sockets::Socket::recv(sockets::i_buff_t buffer, sockets::i_buff_len_t length, int flags)
{
    return ::recv(socket, buffer, length, flags);
}

ssize_t
sockets::Socket::recv(byte *buffer, size_t length, int flags)
{
    return ::recv(socket,
            reinterpret_cast<i_buff_t>(buffer),
            static_cast<i_buff_len_t>(length),
            flags);
}

ssize_t
sockets::Socket::recv(std::unique_ptr<byte[]> &buffer, size_t length, int flags)
{
    return ::recv(socket,
                  reinterpret_cast<i_buff_t>(buffer.get()),
                  static_cast<i_buff_len_t>(length),
                  flags);
}

ssize_t
sockets::Socket::sendto(sockets::i_cbuff_t buffer, sockets::i_buff_len_t length, int flags,
                        sockets::i_addr_cptr address, sockets::i_addr_len_t address_len)
{
    return ::sendto(socket, buffer, length, flags, address, address_len);
}

ssize_t
sockets::Socket::sendto(byte *buffer, size_t length, int flags, sockets::i_addr_cptr address,
                        sockets::i_addr_len_t address_len)
{
    return ::sendto(socket,
            reinterpret_cast<i_cbuff_t>(buffer),
            static_cast<i_buff_len_t>(length),
            flags, address, address_len);
}

ssize_t
sockets::Socket::sendto(std::unique_ptr<byte[]> &buffer, size_t length, int flags, sockets::i_addr_cptr address,
                        sockets::i_addr_len_t address_len)
{
    return ::sendto(socket,
                    reinterpret_cast<i_cbuff_t>(buffer.get()),
                    static_cast<i_buff_len_t>(length),
                    flags, address, address_len);
}
ssize_t
sockets::Socket::send(sockets::i_cbuff_t buffer, sockets::i_buff_len_t length, int flags)
{
    return ::send(socket, buffer, length, flags);
}

ssize_t
sockets::Socket::send(const byte *buffer, size_t length, int flags)
{
    return ::send(socket, reinterpret_cast<i_cbuff_t >(buffer), static_cast<i_buff_len_t>(length), flags);
}

ssize_t
sockets::Socket::send(const std::unique_ptr<byte[]> &buffer, size_t length, int flags)
{
    return ::send(socket, reinterpret_cast<i_cbuff_t>(buffer.get()), static_cast<i_buff_len_t>(length), flags);
}

int
sockets::Socket::close()
{
    return closesocket(socket);
}
