#pragma once

#include "Byte.h"
#include "ByteString.h"
#include "Socket.h"

namespace sockets {
    /**
     * Represents a connection between a client and a server.
     * 
     * A connection cannot be copied.
     */ 
    class Connection {
    private:
        sock_t _socket;
        size_t _bufferCapacity;
        bool _closed;
    public:
        Connection();
        Connection(sock_t socket, size_t bufferCapacity);
        ~Connection();
        
        // Delete the copy constructor
        Connection(const Connection&) = delete;
        // Delete copy assignment operator
        Connection& operator==(const Connection&) = delete;

        /**
         * Reads all available bytes, up to n bytes if provided.
         */
        ByteString read();
        ByteString read(size_t n);

        /**
         * Blocks until exactly n bytes are read.
         */
        ByteString read_exactly(size_t n);

        /**
         * Blocks until the delimiter is read. Returns all read bytes
         */
        ByteString read_until(byte delim);
        ByteString read_until(ByteString& delim);

        /**
         * Writes the bytes to the connection.
         */
        void write(ByteString bytes);

        /**
         * Returns true if the connection has been closed. False if otherwise.
         */
        bool closed();
    };
}
