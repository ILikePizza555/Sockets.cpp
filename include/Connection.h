#pragma once

#include <algorithm>
#include "Byte.h"
#include "abl/TCPSocket.h"
#include "Error.h"

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
        template<typename T = TCPSocket>
        void
        check_connection_state(const std::string &function_name, T &socket, bool closed)
        {
            if (closed) throw sockets::ClosedError("Connection", function_name);
            if (socket.socket == sockets::invalid_socket)
                throw sockets::InvalidSocketError("Connection", function_name);
        }
    }

    /**
     * Represents a connection between a client and a server.
     * 
     * A connection cannot be copied.
     *
     * An alternative implementation of Socket can be specified using the template parameter. This implementation must
     * implement recv(), send(), and operator==.
     */
    template<typename T = TCPSocket>
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
        Connection(const Connection<T> &) = delete;

        // Delete copy assignment operator
        Connection<T> &
        operator=(const Connection<T> &) = delete;

        // Move construction
        Connection(Connection<T>&& other) noexcept : _socket(other._socket), _buffer(std::move(other._buffer)), _closed(other._closed)
        {
            other._socket = T(invalid_socket);
            other._closed = true;
        }

        // Move assignment
        Connection<T>&
        operator=(Connection<T>&& other) noexcept
        {
            if(!(_socket == other._socket))
            {
                _socket = other._socket;
                other._socket = T(invalid_socket);

                _buffer = std::move(other._buffer);

                _closed = other._closed;
                other._closed = true;
            }
            return *this;
        }

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

            _socket.recv(_buffer, n);

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
                _socket.recv(_buffer, n - _buffer.size(), _buffer.size());
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

            // Check the distance so that we don't get any weird errors with casting
            auto distance = std::distance(begin, end);
            if(distance < 0) throw std::invalid_argument("begin and end have a negative distance");
            if(distance == 0) return 0;

            // Resize only if needed
            if(_buffer.size() < distance)
                _buffer.resize(static_cast<size_t>(distance));


            std::copy(begin, end, _buffer.begin());
            ssize_t bytes = _socket.send(_buffer, 0, 0);

            return static_cast<size_t>(bytes);
        }

        template<size_t data_size>
        size_t
        write(const ByteString<data_size> &data)
        {
            check_connection_state(__func__, _socket, _closed);

            ssize_t bytes = _socket.send(data.data(), data.size(), 0);

            return static_cast<size_t>(bytes);
        }

        T& get_socket()
        {
            return _socket;
        }

        std::string get_name() const
        {
            return _socket.getpeername().name();
        }

        /**
         * Returns true if closed. False if otherwise.
         */
        bool
        closed() const
        {
            return _closed;
        }

    };

    using TCPConnection = Connection<TCPSocket>;

    /**
     * Attempts to establish a connection to the specified host on the specified port and returns a Connection
     * object if successful.
     *
     * @param host The host to connect too.
     * @param port The port to connect on.
     * @return A connection object representing the connection.
     */
    TCPConnection connect_to(std::string host, std::string port);
    //TODO: Implement Unicode overloads
}
