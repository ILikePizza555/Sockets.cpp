//
// Created by avris on 9/8/2018.
//
#include "catch.hpp"

#include <Buffer.h>
#include <vector>
#include <numeric>
#include <iterator>

TEST_CASE("Buffers are constructed properly", "[Buffer]")
{
    Buffer<char> b(128);
    REQUIRE(b.capacity() == 128);
}

TEST_CASE("Buffers can be written to.", "[Buffer]")
{
    static const int BUFFER_CAPACITY = 128;
    Buffer<char> b(BUFFER_CAPACITY);

    SECTION("Write with the amount of data less than or equal the capacity updates the buffer.")
    {
        std::vector<char> data(BUFFER_CAPACITY);
        std::iota(data.begin(), data.end(), 0);

        REQUIRE(data.size() == b.capacity());

        size_t bytes = b.write(data.begin(), data.end());

        // Number of items written should be equal to the size of data
        REQUIRE(bytes == data.size());

        // Check the data for errors
        for (size_t i = 0; i < b.capacity(); ++i)
        {
            REQUIRE(b[i] == data[i]);
        }
    }

    SECTION("Write with more data than the buffer capacity returns prematurely")
    {
        std::vector<char> data(200);
        std::iota(data.begin(), data.end(), -100);

        size_t bytes_written = b.write(data.begin(), data.end());

        REQUIRE(bytes_written < data.size());
        REQUIRE(bytes_written == b.capacity());

        for (size_t i = 0; i < b.capacity(); ++i)
        {
            REQUIRE(b[i] == data[i]);
        }
    }

    SECTION("Data can be written to the buffer through the iterators.")
    {
        std::iota(b.begin(), b.end(), 0);

        for(size_t i = 0; i < b.capacity(); ++i)
        {
            REQUIRE(b[i] == i);
        }
    }
}

TEST_CASE("Buffers can be resized", "[Buffer]")
{
    const size_t BUFFER_SIZE = 128;

    Buffer<char> b(BUFFER_SIZE);

    std::vector<char> data(BUFFER_SIZE);
    std::iota(data.begin(), data.end(), 0);

    b.write(data.begin(), data.end());

    SECTION("Buffer can be resized to a bigger size with no changes in existing data")
    {
        const size_t NEW_CAPACITY = 200;

        b.resize(NEW_CAPACITY);

        REQUIRE(b.capacity() == NEW_CAPACITY);

        for(size_t i = 0; i < data.size(); ++i)
        {
            REQUIRE(b[i] == data[i]);
        }
    }

    SECTION("Buffer can be resized to a smaller size by truncating data.")
    {
        const size_t NEW_CAPACITY = 50;

        b.resize(NEW_CAPACITY);

        REQUIRE(b.capacity() == NEW_CAPACITY);

        for(size_t i = 0; i < b.capacity(); ++i)
        {
            REQUIRE(b[i] == data[i]);
        }
    }
}

TEST_CASE("Buffers can be read", "[Buffer]")
{
    const size_t BUFFER_SIZE = 128;
    Buffer<char> b(BUFFER_SIZE);

    std::iota(b.begin(), b.end(), 0);

    SECTION("read() reads the buffer's data out to the iterator")
    {
        std::vector<char> data;

        b.read(std::back_inserter(data));

        REQUIRE(data.size() == b.capacity());
        for(size_t i = 0; i < data.size(); ++i)
        {
            REQUIRE(data[i] == b[i]);
        }
    }

    SECTION("Buffer can be read with iterators")
    {
        size_t j = 0;
        for(auto i : b)
        {
            REQUIRE(i == j);
            ++j;
        }
    }
}