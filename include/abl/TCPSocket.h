//
// Created by avris on 9/27/2018.
//

#pragma once

#include "handle.h"
#include "Byte.h"
#include <tuple>
#include <memory>
#include "ip.h"

namespace sockets
{
    struct TCPSocket
    {
        sock_t socket = invalid_socket;

        TCPSocket() = default;

        /**
         * Constructs a new TCPSocket object by copying the handle
         * @param socket
         */
        explicit TCPSocket(sock_t socket);

        /**
         * Constructs a new TCPSocket object by creating a new socket
         * @param fam
         */
        explicit TCPSocket(ip_family fam);

        /**
         * Accepts the first incoming connection and creates a new connected socket.
         *
         * This method will only work if the socket has been bound, and has been marked as passive
         * with listen().
         *
         * @return A new TCPSocket.
         */
        TCPSocket
        accept() const;

        /**
         * Performs the same function as accpet(), but also returns the address of the connected client.
         *
         * @return A tuple with a TCPSocket and addr_t structure.
         */
        std::tuple<TCPSocket, addr_t>
        acceptfrom() const;

        /**
         * Binds the socket to an address.
         *
         * @param addr The address to bind too.
         */
        void bind(const addr_t& addr);

        /**
         * Connects the socket to an address.
         *
         * @param addr
         */
        void connect(const addr_t& addr);

        /**
         * Marks the socket as passive, indicating it will be used for incoming connections.
         *
         * @param backlog
         */
        void listen(int backlog = SOMAXCONN);

        /**
         * Returns the address of the peer connected to the socket.
         *
         * @return
         */
        addr_t getpeername() const;

        /**
         * Returns the address that the socket is bound too.
         *
         * @return
         */
        addr_t getsockname() const;

        /**
         * Receives bytes into the ByteBuffer. Resizes the buffer to the appropriate size.
         *
         * @param buffer The buffer to write too.
         * @param amount The maximum amount of bytes to read.
         * @param offset The index in the buffer to start writing at.
         * @param flags Flags to pass to the system.
         *
         * @return The number of bytes read
         */
        size_t
        recv(ByteBuffer& buffer, size_t amount, size_t offset = 0, int flags = 0) const;

        size_t
        send(const ByteBuffer& buffer, size_t offset = 0, int flags = 0) const;

        int close();
    };

    inline bool operator==(TCPSocket a, TCPSocket b)
    {
        return a.socket == b.socket;
    }
}