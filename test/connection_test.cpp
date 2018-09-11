//
// Created by avris on 9/10/2018.
//
#include "catch.hpp"

#include <algorithm>
#include <Socket.h>
#include <Connection.h>
#include <numeric>

/**
 * Mocks the recv() function of the Socket class.
 */
template<size_t amount>
class ReceiveMockSocket : public sockets::Socket
{
public:
    explicit ReceiveMockSocket() { socket = 1; }

    ssize_t
    recv(sockets::i_buff_t buffer, sockets::i_buff_len_t length, int flags) final
    {
        size_t read_amt = std::min(amount, static_cast<size_t>(length));
        std::iota(buffer, buffer + read_amt, 0);
        return read_amt;
    }

    ssize_t
    recv(byte* buffer, size_t length, int flags) final
    {
        size_t read_amt = std::min(amount, length);
        std::iota(buffer, buffer + read_amt, 0);
        return read_amt;
    }

    ssize_t
    recv(std::unique_ptr<byte[]> &buffer, size_t length, int flags) final
    {
        return recv(buffer.get(), length, flags);
    }
};

TEST_CASE("Connection::read_into()", "[Connection]")
{
    using sockets::Connection;

    Connection test_connection(ReceiveMockSocket<128>(), 128);
}