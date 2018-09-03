#pragma once

#include <algorithm>
#include <Connection.h>
#include <sys/socket.h>
#include <unistd.h>
#include <SocketException.h>

namespace sockets {
    /**
     * Helper function that writes the buffer to the iterator
     */
    template<typename Iter> void write_out(std::unique_ptr<byte[]>& buffer, size_t size, Iter& out)
    {
        for(size_t i = 0; i < size; ++i)
        {
            *out = buffer[i];
            ++out;
        }
    }

    void check_connection_state(const std::string& function_name, sock_t& socket, bool closed)
    {
        if(closed) throw ClosedError("Connection", function_name);
        if(socket == invalid_socket) throw SocketError("Connection", function_name, "invalid socket", EBADF);
    }

    Connection::Connection() : _socket(invalid_socket), _bufferCapacity(0), _buffer(nullptr), _closed(true) {}
    Connection::Connection(sock_t socket, size_t bufferCapacity) : _socket(socket), _bufferCapacity(bufferCapacity), _buffer(std::make_unique<byte[]>(bufferCapacity)) ,_closed(false) {}
    Connection::~Connection()
    {
       close(_socket);
       _buffer.release();
       _closed = true; 
    }

    template<typename Iter> size_t Connection::read_into(Iter out) { return  read_into(_bufferCapacity, out); }

    template<typename Iter> size_t Connection::read_into(size_t n, Iter out)
    {
        if(n > _bufferCapacity) throw std::out_of_range("n cannot be larger than bufferCapacity");
        check_connection_state("read_into", _socket, _closed);

        ssize_t bytes = recv(_socket, _buffer.get(), n, 0); // TODO: Add an option for flags
        if (bytes == -1) throw SocketError("Connection", "read_into", "error on recv()", get_error_code());

        //Write out the buffer
        write_out(_buffer, static_cast<size_t>(bytes), out);

        return bytes;
    }

    ByteString Connection::read_bytes() { return read_bytes(_bufferCapacity); }
    ByteString Connection::read_bytes(size_t n)
    {
        if(n > _bufferCapacity) throw std::out_of_range("n cannot be larger than bufferCapacity");
        check_connection_state("read_bytes", _socket, _closed);

        ssize_t bytes = recv(_socket, _buffer.get(), n, 0);
        if (bytes == -1) throw SocketError("Connection", "read_into", "error on recv()", get_error_code());

        return ByteString(_buffer, static_cast<size_t>(bytes));
    }

    template<typename Iter> void Connection::read_exactly_into(size_t n, Iter out)
    {
        check_connection_state("read_exactly_into", _socket, _closed);

        size_t bytes_read = 0;

        while(bytes_read < n)
        {
            size_t read_amt = std::max(_bufferCapacity, n - bytes_read);

            ssize_t bytes = recv(_socket, _buffer.get(), read_amt, 0);
            if (bytes == -1) throw SocketError("Connection", "read_exactly_into", "error on recv()", get_error_code());
            
            write_out(_buffer, bytes, out);
        }
    }
}
