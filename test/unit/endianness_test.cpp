//
// Created by avris on 10/24/2018.
//

#include "catch.hpp"
#include <abl/system.h>
#include <inaddr.h>
#include <in6addr.h>
#include <algorithm>

using namespace sockets::abl::system;

TEST_CASE("from_ipv4_str creates a sockaddr_in structure in network byte order", "[Endianness][system][ipv4]")
{
    sockaddr_in addr = from_ipv4_str("1.2.3.4", 0);

    unsigned long actual = addr.sin_addr.S_un.S_addr;

    // Network byte order means that the most significant byte has the lowest address
    REQUIRE(actual == 0x0000000004030201);
}

TEST_CASE("from_ipv6_str creates a sockaddr_in6 structure in network byte order", "[Endianness][system][ipv6]")
{
    auto addr = from_ipv6_str("0:1:2:3:4:5:6:7:8:9:10:11:12:13:14:15", 0);

    auto actual = addr.sin6_addr.u.Byte;
    std::array<unsigned char, 16> expected = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

    REQUIRE(std::equal(actual, actual + 16, expected.begin(), expected.end()));
}

TEST_CASE("IpAddress string constructor creates an ipv4 address in network byte order", "[Endianness][IpAddress][ipv4]")
{
    sockets::abl::IpAddress addr(sockets::abl::ip_family::INET, "1.2.3.4", 0);

    auto actual = addr.get_as_ipv4();
    std::array<unsigned char, 4> expected = {1, 2, 3, 4};

    REQUIRE(actual.address == expected);
}

TEST_CASE("IpAddress string constructor creates an ipv6 address in network byte order", "[Endianness][IpAddress][ipv6]")
{
    sockets::abl::IpAddress addr(sockets::abl::ip_family::INET6, "0:1:2:3:4:5:6:7:8:9:10:11:12:13:14:15", 0);

    auto actual = addr.get_as_ipv6();
    std::array<unsigned char, 16> expected = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

    REQUIRE(actual.address == expected);
}

TEST_CASE("from_ipv4 creates a sockaddr_in structure in network byte order", "[Endianness][system][ipv4]")
{
    sockets::abl::ipv4_addr addr{0, {{1, 2, 3, 4}}};

    unsigned long actual = from_ipv4(addr).sin_addr.S_un.S_addr;
    REQUIRE(actual == 0x0000000004030201);
}