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

        virtual Socket
        accept(i_addr_ptr address, i_addr_len_t* address_length);

        virtual int
        bind(i_addr_cptr address, i_addr_len_t address_length);

        virtual int
        connect(i_addr_cptr address, i_addr_len_t address_length);

        virtual int
        getpeername(i_addr_ptr address_out, i_addr_len_t* address_length_out);

        virtual int
        getsockname(i_addr_ptr address_out, i_addr_len_t* address_length_out);

        virtual int
        listen(int backlog);

        virtual ssize_t
        recvfrom(i_buff_t buffer, i_buff_len_t length, int flags, i_addr_ptr address, i_addr_len_t *address_len);

        virtual ssize_t
        recvfrom(byte* buffer, size_t length, int flags, i_addr_ptr address, i_addr_len_t* address_len);

        virtual ssize_t
        recv(i_buff_t buffer, i_buff_len_t length, int flags);

        virtual ssize_t
        recv(byte* buffer, size_t length, int flags);

        virtual ssize_t
        recv(std::unique_ptr<byte[]> &buffer, size_t length, int flags);

        virtual ssize_t
        sendto(i_cbuff_t buffer, i_buff_len_t length, int flags, i_addr_cptr address, i_addr_len_t address_len);

        virtual ssize_t
        sendto(byte* buffer, size_t length, int flags, i_addr_cptr address, i_addr_len_t address_len);

        virtual ssize_t
        sendto(std::unique_ptr<byte[]> &, size_t length, int flags, i_addr_cptr address, i_addr_len_t address_len);

        virtual ssize_t
        send(i_cbuff_t buffer, i_buff_len_t length, int flags);

        virtual ssize_t
        send(const byte* buffer, size_t length, int flags);

        virtual ssize_t
        send(const std::unique_ptr<byte[]> &, size_t length, int flags);

        virtual int
        close();
    };
}