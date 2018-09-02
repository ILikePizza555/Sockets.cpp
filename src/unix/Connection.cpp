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
        if(_closed) throw ClosedError("connection has been closed");

        size_t bytes = recv(_socket)
    }
}
