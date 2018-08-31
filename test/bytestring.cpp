#include "catch.hpp"

#include <ByteString.h>

TEST_CASE("ByteStrings can be constructed", "[ByteString]") 
{
    using sockets::byte;
    using sockets::data_ptr;
    using sockets::ByteString;

    SECTION("Empty ByteString")
    {
        auto b = ByteString();
        REQUIRE(b.size() == 0);
    }

    SECTION("ByteString from list of bytes")
    {
        ByteString b{1, 2, 3, 4};

        REQUIRE(b.at(0) == 1);
        REQUIRE(b.at(1) == 2);
        REQUIRE(b.at(2) == 3);
        REQUIRE(b.at(3) == 4);
    }

    SECTION("Bytestring from copied data")
    {
        data_ptr data(new byte[4]);
        data[0] = 0;
        data[1] = 1;
        data[2] = 2;
        data[3] = 3;
        auto b = ByteString(data, 4);

        REQUIRE(b.size() == 4);
        REQUIRE(b.at(0) == data[0]);
        REQUIRE(b.at(1) == data[1]);
        REQUIRE(b.at(2) == data[2]);
        REQUIRE(b.at(3) == data[3]);
    }

    SECTION("Copy a Bytestring")
    {
        data_ptr data(new byte[4]);
        data[0] = 1;
        data[1] = 2;
        data[2] = 3;
        data[3] = 4;
        auto a = ByteString(data, 4);
        auto b = ByteString(a);

        REQUIRE(a.size() == 4);
        REQUIRE(a.at(0) == data[0]);
        REQUIRE(a.at(1) == data[1]);
        REQUIRE(a.at(2) == data[2]);
        REQUIRE(a.at(3) == data[3]);

        REQUIRE(b.size() == 4);
        REQUIRE(b.at(0) == data[0]);
        REQUIRE(b.at(1) == data[1]);
        REQUIRE(b.at(2) == data[2]);
        REQUIRE(b.at(3) == data[3]);
    }

    SECTION("Move a Bytestring")
    {
        data_ptr data(new byte[4]);
        data[0] = 1;
        data[1] = 2;
        data[2] = 3;
        data[3] = 4;
        auto a = ByteString(data, 4);
        auto b = ByteString(std::move(a));

        REQUIRE(a.size() == 0);

        REQUIRE(b.size() == 4);
        REQUIRE(b.at(0) == data[0]);
        REQUIRE(b.at(1) == data[1]);
        REQUIRE(b.at(2) == data[2]);
        REQUIRE(b.at(3) == data[3]);
    }
}
