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
        void
        check_connection_state(const std::string &function_name, sockets::Socket &socket, bool closed);
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
        Connection() : _socket(invalid_socket), _buffer(DEFAULT_BUFFER_CAPACITY), _closed(true) {}

        explicit Connection(T socket) : _socket(socket), _buffer(DEFAULT_BUFFER_CAPACITY), _closed(false)
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

            ssize_t bytes_read = _socket.recv(_buffer, n, 0);
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

            // Total bytes read
            size_t bytes_read = 0;
            // Cursor pointing to the position last written to in the buffer
            byte* cursor = _buffer.data();

            while (bytes_read < n)
            {
                ssize_t bytes_received = _socket.recv(cursor, n - bytes_read, 0);
                if (bytes_received == -1)
                    throw SocketError("Connection", __func__, "error on recv()", get_error_code());

                // Update the counter and offset the cursor
                bytes_read += bytes_received;
                cursor += bytes_received;
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
                // Index indicating the beginning of the range of data received this iteration
                size_t range_begin = _buffer.size();

                byte* cursor = _buffer.data() + _buffer.size();

                // Read into the unused part of the buffer
                ssize_t bytes_received = _socket.recv(cursor, _buffer.capacity() - _buffer.size(), 0);
                if(bytes_received == -1)
                    throw SocketError("Connection", __func__, "error on recv()", get_error_code());

                // Search for a delimiter in the received bytes
                auto needle = std::search(_buffer.begin() + range_begin, _buffer.end(), delim.begin(), delim.end());
                if(needle == _buffer.end()) // Delimiter was not found.
                {
                    // Expand the buffer if needed
                    if(_buffer.size() == _buffer.capacity()) _buffer.reserve(_buffer.capacity() * 2);

                    continue;
                }
                else // Delimiter was found
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

            _buffer.write(begin, end);

            ssize_t bytes = _socket.send(_buffer.get(), _buffer.capacity(), 0);
            if(bytes == -1) throw SocketError("Connection_c", __func__, "error on send()", get_error_code());

            return static_cast<size_t>(bytes);
        }

        size_t
        write(const ByteString &data)
        {
            check_connection_state(__func__, _socket, _closed);

            ssize_t bytes = _socket.send(data.cbegin(), data.size(), 0);
            if(bytes == -1) throw SocketError("Connection_c", __func__, "error on send()", get_error_code());

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
