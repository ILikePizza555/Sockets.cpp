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
 * Creates a stream by copying `output` `n` times and appending it with `ending`. Once bytes have been read, a pointer
 * is incremented and they cannot be accessed again. If no more data can be read, the connection is "closed".
 *
 * @tparam n
 * @tparam output_size
 * @tparam ending_size
 */
template<size_t n, size_t output_size, size_t ending_size>
class OutputSocketStub
{
private:
    static size_t constexpr out_stream_size = (output_size * n) + ending_size;
    ByteString<out_stream_size> out_stream{};
    size_t pos = 0;
public:
    OutputSocketStub(const ByteString<output_size>& output, const ByteString<ending_size>& ending)
    {
        // Copy output n times to out_stream
        for(size_t i = 0; i < n; ++i)
        {
            std::copy(output.begin(), output.end(), out_stream.begin() + (output_size * i));
        }

        // Copy the ending
        std::copy(ending.begin(), ending.end(), out_stream.begin() + (n * output_size));
    }

    OutputSocketStub(OutputSocketStub<n, output_size, ending_size>&& other) noexcept :
    out_stream(std::move(other.out_stream)), pos(other.pos)
    {
        other.pos = 0;
    }

    OutputSocketStub<n, output_size, ending_size>&
    operator=(OutputSocketStub<n, output_size, ending_size>&& other) noexcept
    {
        if(this != &other)
        {
            this->pos = other.pos;
            other.pos = 0;

            this->out_stream = std::move(other.out_stream);
        }
        return *this;
    }

    /**
     * Reads up to `amount` bytes from the stream to the buffer.
     * @return Number of bytes read.
     */
    ssize_t
    recv(ByteBuffer &b, size_t amount, size_t offset = 0, int s = 0)
    {
        size_t read_amount = std::min(amount, out_stream.size() - pos);
        if(read_amount == 0)
        {
#ifdef _WIN32
            WSASetLastError(ECONNRESET);
#elif __unix
            errno = ECONNRESET;
#endif
            throw sockets::SocketReadError("recv");
        }

        auto begin_iter = out_stream.begin() + read_amount;
        auto end_iter = begin_iter + read_amount;
        std::copy(begin_iter, end_iter, b.begin() + offset);

        pos += read_amount;
        return read_amount;
    }

    ssize_t
    send(const ByteBuffer&, size_t, int) { return 0; }

    bool
    invalid()
    {
        return false;
    }

    const ByteString<out_stream_size>&
    stream() const
    {
        return out_stream;
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

TEST_CASE("Connection::read_until() reads until a delimiter is encountered", "[Connection]")
{
    using sockets::Connection;

    SECTION("single-byte delimiter arrives after several calls to recv")
    {
        ByteString<1> delim{'\n'};
        OutputSocketStub<3, 7, 1> stub({'a', 'b', 'c', 'd', 'e', 'f', 0}, delim);
        Connection<OutputSocketStub<3, 7, 1>> conn(std::move(stub));

        auto expected = conn.get_socket().stream();
        ByteBuffer& actual = conn.read_until(delim);
        REQUIRE(std::equal(actual.begin(), actual.end(), expected.begin(), expected.end()));
    }

    SECTION("single-byte delimiter arrives after single call to recv")
    {
        ByteString<1> delim{'\n'};
        OutputSocketStub<1, 7, 1> stub({'a', 'b', 'c', 'd', 'e', 'f', 0}, delim);
        Connection<OutputSocketStub<1, 7, 1>> conn(std::move(stub));

        auto expected = conn.get_socket().stream();
        ByteBuffer& actual = conn.read_until(delim);
        REQUIRE(std::equal(actual.begin(), actual.end(), expected.begin(), expected.end()));
    }

    SECTION("multi-byte delimiter arrives after several calls to recv")
    {
        static const ByteString<4> DELIMITER{1, 2, 3, '\n'};

        OutputSocketStub<3, 7, 4> stub({'a', 'b', 'c', 'd', 'e', 'f', 0}, DELIMITER);
        Connection<OutputSocketStub<3, 7, 4>> conn(std::move(stub));

        auto expected = conn.get_socket().stream();
        ByteBuffer& actual = conn.read_until(DELIMITER);
        REQUIRE(std::equal(actual.begin(), actual.end(), expected.begin(), expected.end()));
    }

    SECTION("multi-byte delimiter arrives after single call to recv")
    {
        static const ByteString<4> DELIMITER{'\r', '\n', '\r', '\n'};

        OutputSocketStub<1, 7, 4>  stub({'a', 'b', 'c', 'd', 'e', 'f', 0}, DELIMITER);
        Connection<OutputSocketStub<1, 7, 4>> conn(std::move(stub));

        auto expected = conn.get_socket().stream();
        ByteBuffer& actual = conn.read_until(DELIMITER);
        REQUIRE(std::equal(actual.begin(), actual.end(), expected.begin(), expected.end()));
    }

    SECTION("read_until() deletes excess bytes")
    {
        OutputSocketStub<1, 3, 8> stub({'a', 'b', 'c'}, {'d', 'e', '\r', '\n', 5, 6, 7});
        Connection<OutputSocketStub<1, 3, 8>> conn(std::move(stub));

        ByteBuffer expected{'a', 'b', 'c', 'd', 'e', '\r', '\n'};
        ByteBuffer& actual = conn.read_until<2>({'\r', '\n'});
        REQUIRE(expected == actual);
    }
}