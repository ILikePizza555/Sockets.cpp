//
// Created by avris on 9/10/2018.
//
#include "catch.hpp"

#include <abl/handle.h>
#include <abl/system.h>

#include <algorithm>
#include <Connection.h>

using sockets::abl::UniqueHandle;

/**
 * Outputs the number 2 on calls to recv(). If n > than amount, amount is respected. Ignore all other functions.
 */
struct ReceiveSocketStub
{
private:
    size_t n;
    UniqueHandle socket;

public:
    explicit ReceiveSocketStub(size_t n) : n(n), socket(UniqueHandle(nullptr, &sockets::abl::close_handle)) {}
    ReceiveSocketStub(ReceiveSocketStub&& other) noexcept : n(other.n), socket(std::move(other.socket)) { other.n = 0; }

    ReceiveSocketStub& operator=(ReceiveSocketStub&& other) noexcept
    {
        if(this != &other)
        {
            this->n = other.n;
            other.n = 0;

            this->socket = std::move(other.socket);
        }
        return *this;
    }

    ssize_t
    recv(ByteBuffer& b, size_t amount, size_t offset = 0, int s = 0)
    {
        ssize_t size = std::min(amount, n);
        b.resize(static_cast<size_t>(size) + offset);

        std::generate(b.begin() + offset, b.end(), [](){return 2;});

        return size;
    }

    ssize_t
    send(const ByteBuffer&, size_t, int) { return 0; }

    bool
    invalid()
    {
        return false;
    }

    static bool is_data_correct(const ByteBuffer& buf)
    {
        return std::all_of(buf.cbegin(), buf.cend(), [](byte b){return b == 2;});
    }
};

/**
 * Outputs the number 2 `amount` times until n calls have been made. Then outputs delim.
 */
template<size_t delim_size>
struct DelimiterSocketStub
{
private:
    /* Counter */
    size_t c = 0;
    UniqueHandle socket = UniqueHandle(nullptr, &sockets::abl::close_handle);
    size_t n;
    ByteString<delim_size> delim;
public:
    DelimiterSocketStub(size_t n, ByteString<delim_size> delim) : n(n), delim(delim)
    {}

    DelimiterSocketStub(DelimiterSocketStub&& other) noexcept:
    c(other.c), socket(std::move(other.socket)), n(other.n), delim(std::move(other.delim))
    {
        other.c = 0;
        other.n = 0;
    }

    DelimiterSocketStub& operator=(DelimiterSocketStub&& other) noexcept
    {
        if(this != &other)
        {
            this->c = other.c;
            this->n = other.n;
            other.c = 0;
            other.n = 0;

            this->socket = std::move(other.socket);
            this->delim = std::move(other.delim);
        }
        return *this;
    }

    ssize_t
    recv(ByteBuffer &b, size_t amount, size_t offset = 0, int s = 0)
    {
        if (c >= n)
        {
            b.resize(delim_size + offset);
            std::copy(delim.cbegin(), delim.cend(), b.begin() + offset);
            return delim_size;
        }

        b.resize(amount + offset);
        std::generate(b.begin() + offset, b.end(), [](){return 2;});

        c++; //Ayy lmao

        return amount;
    }

    ssize_t
    send(const ByteBuffer&, size_t, int)
    {
        return 0;
    }

    bool
    invalid()
    {
        return false;
    };
};

TEST_CASE("Connection::read()", "[Connection]")
{
    using sockets::Connection;

    // Testing READ_AMOUNT <= AVAILABLE_DATA_AMOUNT
    SECTION("when READ_AMOUNT <= AVAILABLE_DATA_AMOUNT, read() should return a buffer where size() <= READ_AMOUNT")
    {
        static const size_t AVAILABLE_DATA_AMOUNT = 64;
        static const size_t READ_AMOUNT = 64;

        Connection<ReceiveSocketStub> conn(std::move(ReceiveSocketStub{AVAILABLE_DATA_AMOUNT}));

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

        Connection<ReceiveSocketStub> conn(std::move(ReceiveSocketStub{AVAILABLE_DATA_AMOUNT}));

        ByteBuffer& buf = conn.read(READ_AMOUNT);

        REQUIRE(buf.size() <= READ_AMOUNT);
        REQUIRE(ReceiveSocketStub::is_data_correct(buf));
    }
}

TEST_CASE("Connection::read_exactly()", "[Connection]")
{
    using sockets::Connection;

    SECTION("when READ_AMOUNT <= AVAILABLE_DATA_AMOUNT, read_exactly should return a buffer where size() == READ_AMOUNT")
    {
        static const size_t AVAILABLE_DATA_AMOUNT = 64;
        static const size_t READ_AMOUNT = 64;

        Connection<ReceiveSocketStub> conn(std::move(ReceiveSocketStub{AVAILABLE_DATA_AMOUNT}));

        ByteBuffer& buf = conn.read_exactly(READ_AMOUNT);

        REQUIRE(buf.size() == READ_AMOUNT);
        REQUIRE(ReceiveSocketStub::is_data_correct(buf));
    }

    SECTION("when READ_AMOUNT > AVAILABLE_DATA_AMOUNT, read_exactly should return a buffer where size() == READ_AMOUNT")
    {
        static const size_t AVAILABLE_DATA_AMOUNT = 16;
        static const size_t READ_AMOUNT = 64;

        Connection<ReceiveSocketStub> conn(std::move(ReceiveSocketStub{AVAILABLE_DATA_AMOUNT}));

        ByteBuffer& buf = conn.read_exactly(READ_AMOUNT);

        REQUIRE(buf.size() == READ_AMOUNT);
        REQUIRE(ReceiveSocketStub::is_data_correct(buf));
    }
}

TEST_CASE("Connection::read_until()", "[Connection]")
{
    using sockets::Connection;

    SECTION("delimiter of size 1")
    {
        static const ByteString<1> DELIMITER{'\n'};

        DelimiterSocketStub<DELIMITER.size()> stub{2, DELIMITER};
        Connection<DelimiterSocketStub<DELIMITER.size()>> conn(std::move(stub));

        ByteBuffer& buf = conn.read_until(DELIMITER);
        REQUIRE(buf.back() == DELIMITER[0]);
    }

    SECTION("delimiter of size 4")
    {
        static const ByteString<4> DELIMITER{1, 2, 3, '\n'};

        DelimiterSocketStub<DELIMITER.size()> stub{2, DELIMITER};
        Connection<DelimiterSocketStub<DELIMITER.size()>> conn(std::move(stub));

        ByteBuffer& buf = conn.read_until(DELIMITER);
        REQUIRE(std::equal(buf.end() - DELIMITER.size(), buf.end(), DELIMITER.begin(), DELIMITER.end()));
    }

    SECTION("read_until() deletes excess bytes")
    {
        static const ByteString<8> DELIMITER{'1', '2', '\n', '3', '4', 5, 6, 7};

        DelimiterSocketStub<8> stub{1, DELIMITER};
        Connection<DelimiterSocketStub<8>> conn(std::move(stub));

        ByteBuffer& buf = conn.read_until<1>({'\n'});
        REQUIRE(buf.back() == '\n');
    }
}