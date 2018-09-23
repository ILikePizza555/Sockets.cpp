#pragma once

#include <algorithm>
#include "Byte.h"
#include "Socket.h"
#include "SocketException.h"

#ifndef DEFAULT_BUFFER_CAPACITY
#define DEFAULT_BUFFER_CAPACITY 1400
#endif

namespace sockets {
    namespace
    {
        /**
         * Helper function that checks the state of a Connection and throws an error if it's invalid.
         * @param function_name
         * @param socket
         * @param closed
         */
        template<typename T = Socket>
        void
        check_connection_state(const std::string &function_name, T &socket, bool closed)
        {
            if (closed) throw sockets::ClosedError("Connection", function_name);
            if (socket.socket == sockets::invalid_socket)
                throw sockets::SocketError("Connection", function_name, "invalid socket", EBADF);
        }
    }

    /**
     * Represents a connection between a client and a server.
     * 
     * A connection cannot be copied.
     *
     * An alternative implementation of Socket can be specified using the template parameter.
     */
    template<typename T = Socket>
    class Connection
    {
    private:
        T _socket;
        ByteBuffer _buffer;
        bool _closed;
    public:
        Connection() : _socket(invalid_socket), _buffer(), _closed(true) {}

        explicit Connection(T socket) : _socket(socket), _buffer(), _closed(false)
        {}

        ~Connection()
        {
            _socket.close();
            _closed = true;
        }

        // Delete the copy constructor
        Connection(const Connection &) = delete;

        // Delete copy assignment operator
        Connection &
        operator=(const Connection &) = delete;

        /**
         * Reads up to n bytes from the network.
         *
         * @param n Number of bytes to read.
         * @return A reference to a ByteBuffer containing the data received
         */
        ByteBuffer&
        read(size_t n)
        {
            check_connection_state(__func__, _socket, _closed);

            // Check if the buffer needs to be cleared. This is to prevent accidentally returning old data.
            if(!_buffer.empty()) _buffer.clear();
            // Ensure that the buffer has the capacity for n bytes
            _buffer.reserve(n);

            ssize_t bytes_read = _socket.recv(_buffer, n);
            if (bytes_read == -1) throw SocketError("Connection", __func__, "error on recv()", get_error_code());

            return _buffer;
        }

        /**
         * Reads exactly n bytes from the network, and returns the result as a reference to a ByteBuffer.
         * This method blocks until n bytes are read.
         *
         * @param n The number of bytes to read.
         * @return A reference to a ByteBuffer containing the data received.
         */
        ByteBuffer&
        read_exactly(size_t n)
        {
            check_connection_state(__func__, _socket, _closed);

            // Check if the buffer needs to be cleared
            if(!_buffer.empty()) _buffer.clear();
            // Ensure that the buffer has the capacity for n bytes
            _buffer.reserve(n);

            while (_buffer.size() < n)
            {
                ssize_t bytes_received = _socket.recv(_buffer, n - _buffer.size(), _buffer.size());
                if (bytes_received == -1)
                    throw SocketError("Connection", __func__, "error on recv()", get_error_code());
            }

            return _buffer;
        }

        /**
         * Reads bytes from the network until a delimiter is reached, then returns all bytes read up to the end of the
         * delimiter. Bytes read after the delimiter are discarded.
         *
         * @tparam delim_size Size of the delimiter
         * @param delim The delimiter
         * @return A reference to a ByteBuffer containing the data recieved.
         */
        template<size_t delim_size>
        ByteBuffer&
        read_until(const ByteString<delim_size>& delim)
        {
            check_connection_state(__func__, _socket, _closed);

            // Check if the buffer needs to be cleared
            if(!_buffer.empty()) _buffer.clear();

            while(true)
            {
                ssize_t bytes_received = _socket.recv(_buffer, DEFAULT_BUFFER_CAPACITY, _buffer.size());
                if(bytes_received == -1)
                    throw SocketError("Connection", __func__, "error on recv()", get_error_code());

                // Search for a delimiter in the received bytes
                auto needle = std::search(_buffer.begin() + bytes_received, _buffer.end(), delim.begin(), delim.end());
                if(needle != _buffer.end()) // Delimiter was not found.
                {
                    // Clear the bytes after the delimiter
                    _buffer.erase(needle + delim_size, _buffer.cend());

                    return _buffer;
                }
            }
        }

        /**
         * Writes bytes to the connection.
         *
         * @return The number of bytes written.
         */
        template<typename Iter>
        size_t
        write(Iter begin, Iter end)
        {
            check_connection_state(__func__, _socket, _closed);

            ssize_t bytes = _socket.send(_buffer, _buffer.capacity(), 0);
            if(bytes == -1) throw SocketError("Connection", __func__, "error on send()", get_error_code());

            return static_cast<size_t>(bytes);
        }

        template<size_t data_size>
        size_t
        write(const ByteString<data_size> &data)
        {
            check_connection_state(__func__, _socket, _closed);

            ssize_t bytes = _socket.send(data.data(), data.size(), 0);
            if(bytes == -1) throw SocketError("Connection", __func__, "error on send()", get_error_code());

            return static_cast<size_t>(bytes);
        }

        //TODO: getpeername()

        /**
         * Returns true if closed. False if otherwise.
         */
        bool
        closed()
        {
            return _closed;
        }
    };


}
