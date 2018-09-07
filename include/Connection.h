#pragma once

#include "Buffer.h"
#include "Byte.h"
#include "ByteString.h"
#include "Socket.h"

#ifndef DEFAULT_BUFFER_CAPACITY
#define DEFAULT_BUFFER_CAPACITY 1400
#endif

namespace sockets {
    /**
     * Represents a connection between a client and a server.
     * 
     * A connection cannot be copied.
     */
    class Connection
    {
    private:
        Socket _socket;
        Buffer<byte> _buffer;
        bool _closed;
    public:
        Connection();

        Connection(Socket socket, size_t buffer_capacity = DEFAULT_BUFFER_CAPACITY);

        ~Connection();

        // Delete the copy constructor
        Connection(const Connection &) = delete;

        // Delete copy assignment operator
        Connection &
        operator==(const Connection &) = delete;

        /**
         * Reads all available bytes into the given iterator. 
         *
         * If n is provided, the method reads up to n bytes instead. If n is larger than the buffer capacity, std::out_of_range is thrown.
         * 
         * If the connection is closed, a ClosedError is thrown.
         * If the socket is invalid, a SocketError is thrown.
         *
         * Returns number of bytes read.
         */
        template<typename Iter>
        size_t
        read_into(Iter out);

        template<typename Iter>
        size_t
        read_into(size_t n, Iter out);

        /**
         * Reads up to n bytes. If no n is provided, the largest possible value will be used.
         *
         * If n is larger than _bufferCapacity, a std::out_of_range exception is throw.
         * If the connection is closed, a ClosedError is thrown
         * If the socket is invalid a SocketError is thrown.
         *
         * Returns a ByteString containing the bytes read.
         */
        ByteString
        read_bytes();

        ByteString
        read_bytes(size_t n);

        /**
         * Reads exactly n bytes into the given iterator. Blocks until n bytes are read.
         *
         * If the connection is closed, a ClosedError is thrown.
         * If the socket is invalid, a SocketError is thrown.
         */
        template<typename Iter>
        void
        read_exactly_into(size_t n, Iter out);

        /**
         * Reads exactly n bytes. Blocks until n bytes are read.
         */
        ByteString
        read_exactly_bytes(size_t n);

        /**
         * Reads bytes into the iterator until the delimiter is reached. The delimiter will also be read into the iterator.
         * Returns the number of bytes read.
         */
        template<typename Iter>
        size_t
        read_until_into(const ByteString &delim, Iter out);

        /**
         * Reads until the delimiter is reached. Outputs all bytes read, including the delimiter.
         */
        ByteString
        read_until_bytes(const ByteString &delim);

        /**
         * Writes bytes to the connection. Returns the number of bytes written.
         */
        template<typename Iter>
        size_t
        write(Iter begin, Iter end);

        size_t
        write(const ByteString &data);

        /**
         * Returns true if closed. False if otherwise.
         */
        bool
        closed();
    };
}
