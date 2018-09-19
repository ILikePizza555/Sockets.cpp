//
// Created by avris on 9/5/2018.
//

#include <winsock2.h>
#include <Socket.h>
#include <SocketException.h>

namespace sockets {
    Socket::Socket(sockets::sock_t socket) : socket(socket)
    {}

    Socket::Socket(sockets::sock_addr_family family,
                            sockets::sock_type type,
                            sockets::sock_proto protocol)
    {
        socket = ::socket(family, type, protocol);
        if (socket == invalid_socket) throw SocketError("Socket", __func__, "Error creating socket.", get_error_code());
    }

    Socket
    Socket::accept(sockets::i_addr_ptr address, sockets::i_addr_len_t *address_length)
    {
        return ::accept(socket, address, address_length);
    }

    int
    Socket::bind(sockets::i_addr_cptr address, sockets::i_addr_len_t address_length)
    {
        return ::bind(socket, address, address_length);
    }

    int
    Socket::connect(sockets::i_addr_cptr address, sockets::i_addr_len_t address_length)
    {
        return ::connect(socket, address, address_length);
    }

    int
    Socket::getpeername(sockets::i_addr_ptr address_out, sockets::i_addr_len_t *address_length_out)
    {
        return ::getpeername(socket, address_out, address_length_out);
    }

    int
    Socket::getsockname(sockets::i_addr_ptr address_out, sockets::i_addr_len_t *address_length_out)
    {
        return ::getsockname(socket, address_out, address_length_out);
    }

    int
    Socket::listen(int backlog)
    {
        return ::listen(socket, backlog);
    }

    ssize_t
    Socket::recvfrom(ByteBuffer &buffer, size_t amount, int flags, i_addr_ptr address, i_addr_len_t *address_len)
    {
        return ::recvfrom(socket,
                          reinterpret_cast<i_buff_t>(buffer.data()),
                          static_cast<i_buff_len_t>(amount),
                          flags,
                          address,
                          address_len);
    }

    ssize_t
    Socket::recv(ByteBuffer& buffer, size_t amount, int flags)
    {
        return ::recv(socket,
                reinterpret_cast<i_buff_t>(buffer.data()),
                static_cast<i_buff_len_t>(amount),
                flags);
    }

    ssize_t
    Socket::recv(byte* buffer, size_t length, int flags)
    {
        return ::recv(socket,
                      reinterpret_cast<i_buff_t>(buffer),
                      static_cast<i_buff_len_t>(length),
                      flags);
    }

    ssize_t
    Socket::sendto(const ByteBuffer& buffer, int flags, i_addr_cptr address, i_addr_len_t address_len)
    {
        return ::sendto(socket,
                reinterpret_cast<i_cbuff_t>(buffer.data()),
                static_cast<i_buff_len_t>(buffer.size()),
                flags, address, address_len);
    }

    ssize_t
    Socket::sendto(const byte* buffer, size_t length, int flags, sockets::i_addr_cptr address,
                            sockets::i_addr_len_t address_len)
    {
        return ::sendto(socket,
                        reinterpret_cast<i_cbuff_t>(buffer),
                        static_cast<i_buff_len_t>(length),
                        flags, address, address_len);
    }

    ssize_t
    Socket::send(const ByteBuffer& buffer, int flags)
    {
        return ::send(socket,
                      reinterpret_cast<i_cbuff_t>(buffer.data()),
                      static_cast<i_buff_len_t>(buffer.size()),
                      flags);
    }

    ssize_t
    Socket::send(const byte* buffer, size_t length, int flags)
    {
        return ::send(socket, reinterpret_cast<i_cbuff_t>(buffer), static_cast<i_buff_len_t>(length), flags);
    }

    int
    Socket::close()
    {
        return closesocket(socket);
    }
}