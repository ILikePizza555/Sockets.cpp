#pragma once

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

        /**
         * Helper functions that checks if the last bytes in the buffer are equivalent to delim
         * @param buffer
         * @param last_index The index of the last element in the buffer.
         * @param delim
         * @return
         */
        template<size_t delim_size>
        bool
        check_delimiter(const ByteBuffer& buffer, size_t last_index, const ByteString<delim_size>& delim)
        {
            if (last_index < delim.size()) return false;

            size_t buffer_index = last_index - delim.size();
            for (size_t j = 0; j < delim.size(); ++j, ++buffer_index)
            {
                if (buffer[buffer_index] != delim[j]) return false;
            }
            return true;
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

            // Check if the buffer needs to be cleared
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
                    throw SocketError("Connection_c", __func__, "error on recv()", get_error_code());

                // Update the counter and offset the cursor
                bytes_read += bytes_received;
                cursor += bytes_received;
            }

            return _buffer;
        }

        /**
         * Reads bytes into the iterator until the delimiter is reached. The delimiter will also be read into the iterator.
         *
         * @returns The number of bytes read.
         */
        template<typename Iter>
        size_t
        read_until_into(const ByteString &delim, Iter out)
        {
            if (delim.size() > _buffer.capacity()) throw std::out_of_range("delim.size() is larger than buffer capacity");
            check_connection_state(__func__, _socket, _closed);

            while (true)
            {
                ssize_t bytes = _socket.recv(_buffer.get(), _buffer.capacity(), 0);
                if (bytes == -1) throw SocketError("Connection_c", __func__, "error on recv()", get_error_code());

                // Iterate over the buffer to read to the iterator, while checking for a delimiter
                for (size_t i = 0; i < _buffer.capacity(); ++i)
                {
                    *out = _buffer[i];
                    ++out;

                    if (check_delimiter(_buffer, i, delim)) return static_cast<size_t>(bytes);
                }
            }
        }

        /**
         * Reads until the delimiter is reached. Outputs all bytes read, including the delimiter.
         *
         * @returns A ByteString containing all the data read.
         */
        ByteString
        read_until_bytes(const ByteString &delim)
        {
            check_connection_state(__func__, _socket, _closed);

            // Store the old capacity to restore later
            auto old_capacity = _buffer.capacity();

            // Index to keep track of where the end of the field buffer is
            size_t bytes_read = 0;

            bool stop = false;
            while (!stop)
            {
                // On each iteration we create a buffer with a capacity of the remaining space in the field buffer
                Buffer<byte> loop_buff(_buffer.capacity() - bytes_read);

                ssize_t bytes = _socket.recv(loop_buff.get(), loop_buff.capacity(), 0);
                if (bytes == -1) throw SocketError("Connection_c", __func__, "error on recv()", get_error_code());

                // Copy from the loop buffer to the real buffer, while checking for a delimiter
                for(size_t i = 0; i < loop_buff.capacity(); ++i)
                {
                    _buffer[bytes_read] = loop_buff[i];
                    ++bytes_read;

                    if(check_delimiter(_buffer, bytes_read, delim))
                    {
                        stop = true;
                        break;
                    }
                }

                // Resize the real buffer if it's full
                if(bytes_read == _buffer.capacity())
                    _buffer.resize(_buffer.capacity() + bytes);
            }

            auto rv = _buffer.to_bytestring();
            _buffer.resize(old_capacity);
            return rv;
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
