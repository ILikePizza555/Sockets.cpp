//
// Ensures that library data structures are stored in network byte order, and that conversions between the library and
// the system maintain the same order.
//
// Created by avris on 10/24/2018.
//

#include "catch.hpp"
#include <abl/system.h>
#include <inaddr.h>
#include <in6addr.h>
#include <algorithm>

using namespace sockets::abl::system;

const static std::string test_ipv4_str = "1.2.3.4";
const static unsigned long test_ipv4_l = htonl(0x01020304);
const static std::array<unsigned char, 4> test_ipv4_arr = {1, 2, 3, 4};

const static std::string test_ipv6_str = "0001:0203:0405:0607:0809:0A0B:0C0D:0E0F";
const static std::array<unsigned char, 16> test_ipv6_arr = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

TEST_CASE("from_ipv4_str creates a sockaddr_in structure in network byte order", "[Endianness][system][ipv4]")
{
    sockaddr_in addr = from_ipv4_str(test_ipv4_str, 0);

    unsigned long actual = addr.sin_addr.S_un.S_addr;

    // Network byte order means that the most significant byte has the lowest address
    REQUIRE(actual == test_ipv4_l);
}

TEST_CASE("from_ipv6_str creates a sockaddr_in6 structure in network byte order", "[Endianness][system][ipv6]")
{
    auto addr = from_ipv6_str(test_ipv6_str, 0);

    std::array<unsigned char, 16> actual{};
    std::copy(addr.sin6_addr.u.Byte, addr.sin6_addr.u.Byte + 16, actual.begin());

    REQUIRE(actual == test_ipv6_arr);
}

TEST_CASE("IpAddress string constructor creates an ipv4 address in network byte order", "[Endianness][IpAddress][ipv4]")
{
    sockets::abl::IpAddress addr(sockets::abl::ip_family::INET, test_ipv4_str, 0);

    auto actual = addr.get_as_ipv4();

    REQUIRE(actual.address == test_ipv4_arr);
}

TEST_CASE("IpAddress string constructor creates an ipv6 address in network byte order", "[Endianness][IpAddress][ipv6]")
{
    sockets::abl::IpAddress addr(sockets::abl::ip_family::INET6, test_ipv6_str, 0);

    auto actual = addr.get_as_ipv6();

    REQUIRE(actual.address == test_ipv6_arr);
}

TEST_CASE("from_ipv4 creates a sockaddr_in structure in network byte order", "[Endianness][system][ipv4]")
{
    sockets::abl::ipv4_addr addr{0, test_ipv4_arr};

    unsigned long actual = from_ipv4(addr).sin_addr.S_un.S_addr;
    REQUIRE(actual == test_ipv4_l);
}

TEST_CASE("from_ipv6 creates a sockaddr_in6 structure in network byte order", "[Endianness][IpAddress][ipv6]")
{
    sockets::abl::ipv6_addr addr{0, 0, test_ipv6_arr, 0};

    auto actual_struct = from_ipv6(addr).sin6_addr.u.Byte;
    std::array<unsigned char, 16> actual_arr{};
    std::copy(actual_struct, actual_struct + 16, actual_arr.begin());

    REQUIRE(actual_arr == test_ipv6_arr);
}