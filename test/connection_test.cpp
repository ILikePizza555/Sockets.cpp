//
// Created by avris on 9/10/2018.
//
#include "catch.hpp"

#include <algorithm>
#include <Connection.h>

/**
 * Outputs the number 2 on calls to recv(). If n > than amount, amount is respected. Ignore all other functions.
 */
struct ReceiveSocketStub
{
    const size_t n;
    const sockets::sock_t socket = 1;

    ssize_t
    recv(ByteBuffer& b, size_t amount, size_t offset = 0, int flags = 0)
    {
        ssize_t size = std::min(amount, n);
        auto begin_pos = b.begin() + offset;
        auto end_pos = b.begin() + offset + size;

        std::generate(begin_pos, end_pos, [](){return 2;});

        return size;
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

    void
    close()
    {}

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
    const sockets::sock_t socket = 1;

    ssize_t recv(ByteBuffer& b, size_t amount, size_t offset = 0, int flags = 0) { return -1; }
    ssize_t send(const ByteBuffer& buffer, int flags) { return -1; }
    ssize_t send(const byte*, size_t length, int flags) { return -1; }
    void close() {}
};

/**
 * Outputs the number 2 amount times until n calls have been made. Then outputs delim.
 */
template<size_t delim_size>
struct DelimiterSocketStub
{
private:
    size_t c = 0;
public:
    const sockets::sock_t socket = 1;
    const size_t n;
    const ByteString<delim_size> delim;

    DelimiterSocketStub(size_t n, ByteString<delim_size> delim) : n(n), delim(delim)
    {}

    ssize_t
    recv(ByteBuffer &b, size_t amount, size_t offset = 0, int flags = 0)
    {
        if (c >= n)
        {
            std::copy(delim.cbegin(), delim.cend(), b.begin() + offset);
            return delim_size;
        }

        auto begin_pos = b.begin() + offset;
        auto end_pos = b.begin() + offset + amount;
        std::generate(begin_pos, end_pos, [](){return 2;});

        c++; //Ayy lmao

        return amount;
    }

    ssize_t
    send(const ByteBuffer &buffer, int flags)
    {
        return 0;
    }

    ssize_t
    send(const byte *, size_t length, int flags)
    {
        return 0;
    }

    void
    close()
    {};
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

    SECTION("delimiter of size 1")
    {
        static const ByteString<1> DELIMITER{'\n'};

        DelimiterSocketStub<DELIMITER.size()> stub{2, DELIMITER};
        Connection<DelimiterSocketStub<DELIMITER.size()>> conn(stub);

        ByteBuffer& buf = conn.read_until(DELIMITER);
        REQUIRE(buf.back() == DELIMITER[0]);
    }

    SECTION("delimiter of size 4")
    {
        static const ByteString<4> DELIMITER{1, 2, 3, '\n'};

        DelimiterSocketStub<DELIMITER.size()> stub{2, DELIMITER};
        Connection<DelimiterSocketStub<DELIMITER.size()>> conn(stub);

        ByteBuffer& buf = conn.read_until(DELIMITER);
        REQUIRE(std::equal(buf.end() - DELIMITER.size(), buf.end(), DELIMITER.begin(), DELIMITER.end()));
    }

    SECTION("read_until() deletes excess bytes")
    {
        static const ByteString<8> DELIMITER{'1', '2', '\n', '3', '4', 5, 6, 7};

        DelimiterSocketStub<8> stub{1, DELIMITER};
        Connection<DelimiterSocketStub<8>> conn(stub);

        ByteBuffer& buf = conn.read_until<1>({'\n'});
        REQUIRE(buf.back() == '\n');
    }
}