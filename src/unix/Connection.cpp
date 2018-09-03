#pragma once

#include <Connection.h>
#include <sys/socket.h>
#include <unistd.h>
#include <SocketException.h>

namespace sockets {
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
        if(_closed) throw ClosedError("Connection", "read_into");
        if(_socket == invalid_socket) throw SocketError("Connection", "read_into", "invalid socket", EBADF);

        ssize_t bytes = recv(_socket, _buffer.get(), n, 0); // TODO: Add an option for flags
        if (bytes == -1) throw SocketError("Connection", "read_into", "error on recv()", get_error_code());

        //Write out the buffer
        for(size_t i = 0; i < static_cast<size_t>(bytes); ++i)
        {
            *out = _buffer[i];
            ++out;
        }

        return bytes;
    }

    ByteString Connection::read_bytes() { return read_bytes(_bufferCapacity); }
    ByteString Connection::read_bytes(size_t n)
    {
        if(n > _bufferCapacity) throw std::out_of_range("n cannot be larger than bufferCapacity");
        if(_closed) throw ClosedError("Connection", "read_into");
        if(_socket == invalid_socket) throw SocketError("Connection", "read_into", "invalid socket", EBADF);

        ssize_t bytes = recv(_socket, _buffer.get(), n, 0);
        if (bytes == -1) throw SocketError("Connection", "read_into", "error on recv()", get_error_code());

        return ByteString(_buffer, static_cast<size_t>(bytes));
    }
}
