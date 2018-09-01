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
         * Reads all available bytes into the given iterator. If n is provided, the method reads up to n bytes instead.
         */
        template<typename Iter> void read_into(Iter out);
        template<typename Iter> void read_into(size_t n, Iter out);
        
        /**
         * Reads out all available bytes. If n is provided, the methods reads up to n bytes instead.
         */
        ByteString read_bytes();
        ByteString read_bytes(size_t n);

        /**
         * Reads exactly n bytes into the given iterator. Blocks until n bytes are read.
         */
        template<typename Iter>
        void read_exactly_into(size_t n, Iter out);

        /**
         * Reads exactly n bytes. Blocks until n bytes are read.
         */
        ByteString read_exactly_bytes(size_t n);

        /**
         * Reads bytes into the iterator until the delimiter is reached. The delimiter will also be read into the iterator.
         */
        template<typename Iter>
        void read_until_into(const ByteString& delim, Iter out);

        /**
         * Reads until the delimiter is reached. Outputs all bytes read, including the delimiter.
         */
        ByteString read_until_bytes(const ByteString& delim);

        /**
         * Writes bytes to the connection. Returns the number of bytes written.
         */
        template<typename Iter> size_t write(Iter begin, Iter end);
        size_t write(const ByteString& data);

        /**
         * Returns true if closed. False if otherwise.
         */
        bool closed();
    };

    /**
     * Establishes a connection to the host.
     */
    Connection connect(const std::string& host, const std::string& port);
}
