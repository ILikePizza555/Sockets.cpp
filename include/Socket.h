//
// Created by avris on 9/5/2018.
//

#pragma once

#include <ws2tcpip.h>
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
        sock_t socket;

        Socket(sock_t socket);
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
        getsockname(sockaddr_t *vaddress_out, sockaddr_len_t *address_length_out);

        int
        listen(int backlog);

        ssize_t
        recvfrom(sock_buff_t buffer, s_sock_buff_t length, int flags, sockaddr_t *address, sockaddr_len_t *address_len);

        ssize_t
        recv(sock_buff_t buffer, s_sock_buff_t length, int flags);

        ssize_t
        sendto(const sock_buff_t buffer, s_sock_buff_t length, int flags, const sockaddr_t *address, socklen_t address_len);

        ssize_t
        send(const sock_buff_t buffer, s_sock_buff_t length, int flags);

        int
        close();
    };
}