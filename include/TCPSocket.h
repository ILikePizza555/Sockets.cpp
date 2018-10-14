//
// Created by avris on 9/27/2018.
//

#pragma once

#include "abl/handle.h"
#include "abl/ip.h"
#include "Byte.h"
#include <tuple>
#include <memory>

namespace sockets
{
    struct TCPSocket
    {
        abl::UniqueHandle handle;

        TCPSocket();

        /**
         * Constructs a new TCPSocket object by moving the handle
         * @param socket
         */
        explicit TCPSocket(abl::UniqueHandle&& socket);

        /**
         * Constructs a new TCPSocket object by creating a new socket
         * @param fam
         */
        explicit TCPSocket(abl::ip_family fam);

        bool operator==(TCPSocket& other);

        /**
         * Returns true if the socket is invalid; false otherwise.
         */
        bool invalid() const;

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
         * Performs the same function as accept(), but also returns the address of the connected client.
         *
         * @return A tuple with a TCPSocket and an instance of IpAddress.
         */
        std::tuple<TCPSocket, abl::IpAddress>
        acceptfrom() const;

        /**
         * Binds the socket to an address.
         *
         * @param addr The address to bind too.
         */
        void bind(const abl::IpAddress& addr);

        /**
         * Connects the socket to an address.
         *
         * @param addr
         */
        void connect(const abl::IpAddress& addr);

        /**
         * Marks the socket as passive, indicating it will be used for incoming connections.
         *
         * @param backlog
         */
        void listen(int backlog);

        /**
         * Returns the address of the peer connected to the socket.
         *
         * @return
         */
        abl::IpAddress getpeername() const;

        /**
         * Returns the address that the socket is bound too.
         *
         * @return
         */
        abl::IpAddress getsockname() const;

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
    };
}