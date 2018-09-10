#pragma once

#include <algorithm>
#include <Connection.h>
#include <SocketException.h>

/**
 * Helper function that checks the state of a Connection and throws an error if it's invalid.
 * @param function_name
 * @param socket
 * @param closed
 */
void
check_connection_state(const std::string &function_name, sockets::Socket &socket, bool closed)
{
    if (closed) throw sockets::ClosedError("Connection", function_name);
    if (socket.socket == sockets::invalid_socket)
        throw sockets::SocketError("Connection", function_name, "invalid socket", EBADF);
}

/**
 * Helper functions that checks if the last bytes in the buffer are equivalent to delim
 * @param buffer
 * @param last_index The index of the last element in the buffer.
 * @param delim
 * @return
 */
bool
check_delimiter(const Buffer<byte>& buffer, size_t last_index, const ByteString &delim)
{
    if (last_index < delim.size()) return false;

    size_t buffer_index = last_index - delim.size();
    for (size_t j = 0; j < delim.size(); ++j, ++buffer_index)
    {
        if (buffer[buffer_index] != delim.at(j)) return false;
    }
    return true;
}

namespace sockets {

    Connection::Connection() : _socket(invalid_socket), _buffer(0), _closed(true)
    {}

    Connection::Connection(Socket socket, size_t buffer_capacity) : _socket(socket),
                                                                   _buffer(buffer_capacity),
                                                                   _closed(false)
    {}

    Connection::~Connection()
    {
        _socket.close();
        _closed = true;
    }

    template<typename Iter>
    size_t
    Connection::read_into(Iter out)
    { return read_into(_buffer.capacity(), out); }

    template<typename Iter>
    size_t
    Connection::read_into(size_t n, Iter out)
    {
        if (n > _buffer.capacity()) throw std::out_of_range("n cannot be larger than buffer.capacity()");
        check_connection_state(__func__, _socket, _closed);

        ssize_t bytes = _socket.recv(_buffer.get(), n, 0); // TODO: Add an option for flags
        if (bytes == -1) throw SocketError("Connection", __func__, "error on recv()", get_error_code());

        //Write out the buffer
        write_out(_buffer, static_cast<size_t>(bytes), out);

        return static_cast<size_t>(bytes);
    }

    ByteString
    Connection::read_bytes()
    { return read_bytes(_buffer.capacity()); }

    ByteString
    Connection::read_bytes(size_t n)
    {
        if (n > _buffer.capacity()) throw std::out_of_range("n cannot be larger than buffer.capacity()");
        check_connection_state(__func__, _socket, _closed);

        ssize_t bytes = _socket.recv(_buffer.get(), n, 0);
        if (bytes == -1) throw SocketError("Connection", __func__, "error on recv()", get_error_code());

        return _buffer.to_bytestring();
    }

    template<typename Iter>
    void
    Connection::read_exactly_into(size_t n, Iter out)
    {
        check_connection_state(__func__, _socket, _closed);

        size_t bytes_read = 0;

        while (bytes_read < n)
        {
            size_t read_amt = std::min(_buffer.capacity(), n - bytes_read);

            ssize_t bytes = _socket.recv(_buffer.get(), read_amt, 0);
            if (bytes == -1) throw SocketError("Connection", __func__, "error on recv()", get_error_code());

            //write_out should increment the iterator as it writes, therefore we don't do it here
            write_out(_buffer, static_cast<size_t>(bytes), out);
            bytes_read += bytes;
        }
    }

    ByteString
    Connection::read_exactly_bytes(size_t n)
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
            if (bytes == -1) throw SocketError("Connection", __func__, "error on recv()", get_error_code());

            // Update the values
            cursor += static_cast<size_t>(bytes);
            bytes_read += static_cast<size_t>(bytes);
        }

        return buf.to_bytestring();
    }

    template<typename Iter>
    size_t
    Connection::read_until_into(const ByteString &delim, Iter out)
    {
        if (delim.size() > _buffer.capacity()) throw std::out_of_range("delim.size() is larger than buffer capacity");
        check_connection_state(__func__, _socket, _closed);

        while (true)
        {
            ssize_t bytes = _socket.recv(_buffer.get(), _buffer.capacity(), 0);
            if (bytes == -1) throw SocketError("Connection", __func__, "error on recv()", get_error_code());

            // Iterate over the buffer to read to the iterator, while checking for a delimiter
            for (size_t i = 0; i < _buffer.capacity(); ++i)
            {
                *out = _buffer[i];
                ++out;

                if (check_delimiter(_buffer, i, delim)) return static_cast<size_t>(bytes);
            }
        }
    }

    ByteString
    Connection::read_until_bytes(const ByteString &delim)
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
            if (bytes == -1) throw SocketError("Connection", __func__, "error on recv()", get_error_code());

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

    template<typename Iter>
    size_t
    Connection::write(Iter begin, Iter end)
    {
        check_connection_state(__func__, _socket, _closed);

        _buffer.write(begin, end);

        ssize_t bytes = _socket.send(_buffer.get(), _buffer.capacity(), 0);
        if(bytes == -1) throw SocketError("Connection", __func__, "error on send()", get_error_code());

        return static_cast<size_t>(bytes);
    }

    size_t
    Connection::write(const ByteString &data)
    {
        check_connection_state(__func__, _socket, _closed);

        ssize_t bytes = _socket.send(data.cbegin(), data.size(), 0);
        if(bytes == -1) throw SocketError("Connection", __func__, "error on send()", get_error_code());

        return static_cast<size_t>(bytes);
    }

    bool
    Connection::closed()
    {
        return _closed;
    }
}
