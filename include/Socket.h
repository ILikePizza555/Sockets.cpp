//
// Created by avris on 9/5/2018.
//

#pragma once

#include "Byte.h"
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
        accept(i_addr_ptr address, i_addr_len_t* address_length);

        int
        bind(i_addr_cptr address, i_addr_len_t address_length);

        int
        connect(i_addr_cptr address, i_addr_len_t address_length);

        int
        getpeername(i_addr_ptr address_out, i_addr_len_t* address_length_out);

        int
        getsockname(i_addr_ptr address_out, i_addr_len_t* address_length_out);

        int
        listen(int backlog);

        ssize_t
        recvfrom(ByteBuffer& buffer, size_t amount, int flags, i_addr_ptr address, i_addr_len_t* address_len);

        /**
         * Receives bytes into the ByteBuffer. Resizes the buffer to the appropriate size.
         *
         * @param buffer The buffer to write too.
         * @param amount The maximum amount of bytes to read.
         * @param offset The index in the buffer to start writing at.
         * @param flags Flags to pass to the system.
         *
         * @return The number of bytes read, or -1 if there has been an error.
         */
        ssize_t
        recv(ByteBuffer& buffer, size_t amount, size_t offset = 0, int flags = 0);

        ssize_t
        sendto(const ByteBuffer& buffer, int flags, i_addr_cptr address, i_addr_len_t address_len);

        ssize_t
        sendto(const byte*, size_t length, int flags, i_addr_cptr address, i_addr_len_t address_len);

        ssize_t
        send(const ByteBuffer& buffer, int flags);

        ssize_t
        send(const byte*, size_t length, int flags);

        int
        close();
    };
}