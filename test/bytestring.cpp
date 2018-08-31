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

TEST_CASE("ByteStrings can perform immutable operations", "[ByteString]")
{
    using sockets::ByteString;

    ByteString a{1, 2, 3, 4};
    ByteString b{5, 6, 7, 8};

    SECTION("ByteStrings can be appended")
    {
        ByteString actual = a.append(b);

        REQUIRE(actual.size() == a.size() + b.size());
        REQUIRE(actual.at(0) == 1);
        REQUIRE(actual.at(1) == 2);
        REQUIRE(actual.at(2) == 3);
        REQUIRE(actual.at(3) == 4);
        REQUIRE(actual.at(4) == 5);
        REQUIRE(actual.at(5) == 6);
        REQUIRE(actual.at(6) == 7);
        REQUIRE(actual.at(7) == 8);
    }

    SECTION("ByteString::replace replaces bytes")
    {
        const size_t replace_index = 2;
        ByteString actual = a.replace(b, replace_index);

        REQUIRE(actual.size() == a.size() + b.size() - replace_index);
        REQUIRE(actual.at(0) == 1);
        REQUIRE(actual.at(1) == 2);
        REQUIRE(actual.at(2) == 5);
        REQUIRE(actual.at(3) == 6);
        REQUIRE(actual.at(4) == 7);
        REQUIRE(actual.at(5) == 8);
    }
    
    SECTION("ByteString::replace acts like append when offset is equavalent to the size")
    {
        ByteString actual = a.replace(b, a.size());
        ByteString expected = a.append(b);

        REQUIRE(actual == expected);
    }

    SECTION("ByteString::replace throws an exception if offset is too high")
    {
        REQUIRE_THROWS(a.replace(b, 11037));
    }


    SECTION("ByteString::insert inserts bytes")
    {
        const size_t insert_index = 2;
        ByteString actual = a.insert(b, insert_index);
        ByteString expected{1, 2, 5, 6, 7, 8, 3, 4};

        REQUIRE(actual == expected);
    }

    SECTION("ByteString::insert throws an exception if pos is larger than size");
    {
        REQUIRE_THROWS(a.insert(b, 11037));
    }

    SECTION("ByteString::sub creates a sub-string with one parameter")
    {
        const size_t sub_index = 2;
        ByteString actual = a.sub(sub_index);
        ByteString expected{3, 4};

        REQUIRE(actual == expected);
    }

    SECTION("ByteString::sub throws an exception if begin_pos is larger than size")
    {
        REQUIRE_THROWS(a.sub(11037));
    }

    SECTION("ByteString::sub creates a sub-string with two parameters")
    {
        const size_t b_index = 1;
        const size_t e_index = 3;

        ByteString actual = a.sub(b_index, e_index);
        ByteString expected{2, 3, 4};

        REQUIRE(actual == expected);
    }

    SECTION("ByteString::sub throws an exception if begin_pos is greater than end_pos")
    {
        REQUIRE_THROWS(a.sub(11037, 2));
    }

    SECTION("ByteString::sub throws an exception if end_pos is greater than size")
    {
        REQUIRE_THROWS(a.sub(1, 11037));
    }
}
