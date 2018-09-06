//
// Created by avris on 9/5/2018.
//

#pragma once

#include <memory>
#include "sockets.h"

namespace sockets {
    /**
     * Class wrapper for bsd style sockets.
     *
     * This class does not perform any kind of resource management. This is because this class is intended as a very
     * thin layer of abstraction between the platform and the rest of this library.
     */
    struct Socket
    {
        sock_t socket = invalid_socket;

        Socket() = default;

        /**
         * Constructs a Socket by copying the handler
         */
        Socket(sock_t socket);

        /**
         * Constructs a Socket by passing the given parameters to the internal method for creating sockets.
         */
        Socket(sock_addr_family, sock_type, sock_proto);

        Socket
        accept(sockaddr_t* address, sockaddr_len_t* address_length);

        int
        bind(const sockaddr_t* address, sockaddr_len_t address_length);

        int
        connect(const sockaddr_t* address, sockaddr_len_t address_length);

        int
        getpeername(sockaddr_t *address_out, sockaddr_len_t *address_length_out);

        int
        getsockname(sockaddr_t *address_out, sockaddr_len_t *address_length_out);

        int
        listen(int backlog);

        ssize_t
        recvfrom(sock_buff_t buffer, s_sock_buff_t length, int flags, sockaddr_t *address, sockaddr_len_t *address_len);

        ssize_t
        recvfrom(byte* buffer, size_t length, int flags, sockaddr_t* address, sockaddr_len_t* address_len);

        ssize_t
        recv(sock_buff_t buffer, s_sock_buff_t length, int flags);

        ssize_t
        recv(byte* buffer, size_t length, int flags);

        ssize_t
        recv(std::unique_ptr<byte>& buffer, size_t length, int flags);

        ssize_t
        sendto(sock_buff_t buffer, s_sock_buff_t length, int flags, const sockaddr_t *address, sockaddr_len_t address_len);

        ssize_t
        sendto(byte* buffer, size_t length, int flags, const sockaddr_t* address, sockaddr_len_t address_len);

        ssize_t
        sendto(std::unique_ptr<byte>&, size_t length, int flags, const sockaddr_t* address, sockaddr_len_t address_len);

        ssize_t
        send(sock_buff_t buffer, s_sock_buff_t length, int flags);

        ssize_t
        send(byte* buffer, size_t length, int flags);

        ssize_t
        send(std::unique_ptr<byte>&, size_t length, int flags);

        int
        close();
    };
}