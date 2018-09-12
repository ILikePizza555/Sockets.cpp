#pragma once

#include "Byte.h"
#include "ByteBuffer.h"
#include "ByteString.h"
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
        bool
        check_delimiter(const Buffer<byte>& buffer, size_t last_index, const ByteString &delim);
    }

    /**
     * Represents a connection between a client and a server.
     * 
     * A connection cannot be copied.
     *
     * An alternative implementation of Socket can be specified using the template parameter.
     */
    template<typename T = Socket>
    class Connection_c
    {
    private:
        T _socket;
        ByteBuffer _buffer;
        bool _closed;
    public:
        Connection_c() : _socket(invalid_socket), _buffer(0), _closed(true) {}

        explicit Connection_c(T socket, size_t buffer_capacity = DEFAULT_BUFFER_CAPACITY) : _socket(socket),
                                                                                 _buffer(buffer_capacity),
                                                                                 _closed(false)
        {}

        ~Connection_c()
        {
            _socket.close();
            _closed = true;
        }

        // Delete the copy constructor
        Connection_c(const Connection_c &) = delete;

        // Delete copy assignment operator
        Connection_c &
        operator=(const Connection_c &) = delete;

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
        read_into(Iter out)
        { return read_into(_buffer.capacity(), out); }

        template<typename Iter>
        size_t
        read_into(size_t n, Iter out)
        {
            if (n > _buffer.capacity()) throw std::out_of_range("n cannot be larger than buffer.capacity()");
            check_connection_state(__func__, _socket, _closed);

            ssize_t bytes = _socket.recv(_buffer.get(), n, 0); // TODO: Add an option for flags
            if (bytes == -1) throw SocketError("Connection_c", __func__, "error on recv()", get_error_code());

            //Write out the buffer
            write_out(_buffer, static_cast<size_t>(bytes), out);

            return static_cast<size_t>(bytes);
        }

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
        read_bytes()
        { return read_bytes(_buffer.capacity()); }

        ByteString
        read_bytes(size_t n)
        {
            if (n > _buffer.capacity()) throw std::out_of_range("n cannot be larger than buffer.capacity()");
            check_connection_state(__func__, _socket, _closed);

            ssize_t bytes = _socket.recv(_buffer.get(), n, 0);
            if (bytes == -1) throw SocketError("Connection_c", __func__, "error on recv()", get_error_code());

            return _buffer.to_bytestring();
        }

        /**
         * Reads exactly n bytes into the given iterator. Blocks until n bytes are read.
         *
         * If the connection is closed, a ClosedError is thrown.
         * If the socket is invalid, a SocketError is thrown.
         */
        template<typename Iter>
        void
        read_exactly_into(size_t n, Iter out)
        {
            check_connection_state(__func__, _socket, _closed);

            size_t bytes_read = 0;

            while (bytes_read < n)
            {
                size_t read_amt = std::min(_buffer.capacity(), n - bytes_read);

                ssize_t bytes = _socket.recv(_buffer.get(), read_amt, 0);
                if (bytes == -1) throw SocketError("Connection_c", __func__, "error on recv()", get_error_code());

                //write_out should increment the iterator as it writes, therefore we don't do it here
                write_out(_buffer, static_cast<size_t>(bytes), out);
                bytes_read += bytes;
            }
        }

        /**
         * Reads exactly n bytes. Blocks until n bytes are read.
         */
        ByteString
        read_exactly_bytes(size_t n)
        {
            check_connection_state(__func__, _socket, _closed);

            // Make a single allocation here to pass to bytestring
            auto buf = ByteBuffer(n);
            // Cursor to hold the location of the next position to read to in the buffer
            byte *cursor = buf.begin();
            // Count of bytes read from the socket/written to the buffer
            size_t bytes_read = 0;

            while (bytes_read < n)
            {
                // Calculate the number of bytes left to read to pass to recv
                size_t read_amt = n - bytes_read;

                // Read into the buffer
                ssize_t bytes = _socket.recv(cursor, read_amt, 0);
                if (bytes == -1) throw SocketError("Connection_c", __func__, "error on recv()", get_error_code());

                // Update the values
                cursor += static_cast<size_t>(bytes);
                bytes_read += static_cast<size_t>(bytes);
            }

            return buf.to_bytestring();
        }

        /**
         * Reads bytes into the iterator until the delimiter is reached. The delimiter will also be read into the iterator.
         * Returns the number of bytes read.
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
         * Writes bytes to the connection. Returns the number of bytes written.
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

    using Connection = Connection_c<>;
}
