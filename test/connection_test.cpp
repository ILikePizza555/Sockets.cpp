//
// Created by avris on 9/10/2018.
//
#include "catch.hpp"

#include <algorithm>
#include <Connection.h>

/**
 * Outputs the number 10 on calls to recv(). If n > than amount, amount is respected. Ignore all other functions.
 */
struct ReceiveSocketStub
{
    const size_t n;

    ssize_t
    recv(ByteBuffer& b, size_t amount, int flags)
    {
        auto write_amt = std::min(amount, n);
        for(size_t i = 0; i < write_amt; ++i)
            b[i] = 10;
        return write_amt;
    }

    ssize_t
    recv(byte* b, size_t amount, int flags)
    {
        auto write_amt = std::min(amount, n);
        for(size_t i = 0; i < write_amt; ++i)
            *(b + i) = 10;
        return write_amt;
    }

    ssize_t
    send(const ByteBuffer& buffer, int flags)
    {
        return 0;
    }

    ssize_t
    send(const byte*, size_t length, int flags)
    {
        return 0;
    }

    static bool is_data_correct(const ByteBuffer& buf)
    {
        return std::all_of(buf.cbegin(), buf.cend(), [](byte b){return b == 10;});
    }
};

/**
 * Stub that fails on every method call
 */
struct FailureSocketStub
{
    ssize_t recv(ByteBuffer& b, size_t amount, int flags) { return -1; }
    ssize_t recv(byte* b, size_t amount, int flags) { return -1; }
    ssize_t send(const ByteBuffer& buffer, int flags) { return -1; }
    ssize_t send(const byte*, size_t length, int flags) { return -1; }
};

/**
 * Outputs the number 10 until n calls have been made. Then outputs delim.
 */
template<size_t delim_size>
struct DelimiterSocketStub
{
private:
    size_t c = 0;
public:
    const size_t n;
    const ByteString<delim_size> delim;

    ssize_t
    recv(ByteBuffer& b, size_t amount, int flags)
    {
        if (c >= n)
        {
            std::copy(delim.cbegin(), delim.cend(), b.begin());
            return delim_size;
        }

        auto write_amt = std::min(amount, n);
        for(size_t i = 0; i < write_amt; ++i)
            b[i] = 10;

        c++; //Ayy lmao

        return write_amt;
    }

    ssize_t
    recv(byte* b, size_t amount, int flags)
    {
        if (c >= n)
        {
            std::copy(delim.cbegin(), delim.cend(), b.begin());
            return delim_size;
        }

        auto write_amt = std::min(amount, n);
        for(size_t i = 0; i < write_amt; ++i)
            b[i] = 10;

        c++; //Ayy lmao

        return write_amt;
    }

    ssize_t
    send(const ByteBuffer& buffer, int flags)
    {
        return 0;
    }

    ssize_t
    send(const byte*, size_t length, int flags)
    {
        return 0;
    }
};

TEST_CASE("Connection::read()", "[Connection]")
{
    using sockets::Connection;

    // Testing READ_AMOUNT <= AVAILABLE_DATA_AMOUNT
    SECTION("when READ_AMOUNT <= AVAILABLE_DATA_AMOUNT, read() should return a buffer where size() <= READ_AMOUNT")
    {
        static const size_t AVAILABLE_DATA_AMOUNT = 64;
        static const size_t READ_AMOUNT = 64;

        Connection<ReceiveSocketStub> conn(ReceiveSocketStub{AVAILABLE_DATA_AMOUNT});

        ByteBuffer& buf = conn.read(READ_AMOUNT);

        // We can test that buf.size() == READ_AMOUNT, but the documentation simply says that
        // read must read *up to* n bytes.
        REQUIRE(buf.size() <= READ_AMOUNT);
        // Ensure the data isn't tampered with
        REQUIRE(ReceiveSocketStub::is_data_correct(buf));
    }

    // Testing READ_AMOUNT > AVAILABLE_DATA_AMOUNT
    SECTION("when READ_AMOUNT > AVAILABLE_DATA_AMOUNT, read() should return a buffer where size() <= READ_AMOUNT")
    {
        static const size_t AVAILABLE_DATA_AMOUNT = 63;
        static const size_t READ_AMOUNT = 64;

        Connection<ReceiveSocketStub> conn(ReceiveSocketStub{AVAILABLE_DATA_AMOUNT});

        ByteBuffer& buf = conn.read(READ_AMOUNT);

        REQUIRE(buf.size() <= READ_AMOUNT);
        REQUIRE(ReceiveSocketStub::is_data_correct(buf));
    }

    SECTION("read() throws an exception on socket failure")
    {
        Connection<FailureSocketStub> conn(FailureSocketStub{});

        REQUIRE_THROWS_AS(conn.read(1), sockets::SocketError);
    }
}

TEST_CASE("Connection::read_exactly()", "[Connection]")
{
    using sockets::Connection;

    SECTION("when READ_AMOUNT <= AVAILABLE_DATA_AMOUNT, read_exactly should return a buffer where size() == READ_AMOUNT")
    {
        static const size_t AVAILABLE_DATA_AMOUNT = 64;
        static const size_t READ_AMOUNT = 64;

        Connection<ReceiveSocketStub> conn(ReceiveSocketStub{AVAILABLE_DATA_AMOUNT});

        ByteBuffer& buf = conn.read_exactly(READ_AMOUNT);

        REQUIRE(buf.size() == READ_AMOUNT);
        REQUIRE(ReceiveSocketStub::is_data_correct(buf));
    }

    SECTION("when READ_AMOUNT > AVAILABLE_DATA_AMOUNT, read_exactly should return a buffer where size() == READ_AMOUNT")
    {
        static const size_t AVAILABLE_DATA_AMOUNT = 16;
        static const size_t READ_AMOUNT = 64;

        Connection<ReceiveSocketStub> conn(ReceiveSocketStub{AVAILABLE_DATA_AMOUNT});

        ByteBuffer& buf = conn.read_exactly(READ_AMOUNT);

        REQUIRE(buf.size() == READ_AMOUNT);
        REQUIRE(ReceiveSocketStub::is_data_correct(buf));
    }

    SECTION("read_exactly() throws an exception on socket failure")
    {
        Connection<FailureSocketStub> conn(FailureSocketStub{});

        REQUIRE_THROWS_AS(conn.read_exactly(1), sockets::SocketError);
    }
}

TEST_CASE("Connection::read_until()", "[Connection]")
{
    using sockets::Connection;


}