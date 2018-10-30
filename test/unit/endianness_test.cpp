//
// Ensures that library data structures are stored in network byte order, and that conversions between the library and
// the system maintain the same order.
//
// Created by avris on 10/24/2018.
//

#include "catch.hpp"
#include <abl/system.h>
#include <algorithm>

#ifdef _WIN32
#include <inaddr.h>
#include <in6addr.h>
#else
#include <netinet/in.h>
#endif

using namespace sockets::abl::system;

const static std::string test_ipv4_str = "1.2.3.4";
/* Binary representation stored in network byte order */
const static unsigned long test_ipv4_l = htonl(0x01020304);
/* Array representation stored in network byte order */
const static std::array<unsigned char, 4> test_ipv4_arr = {1, 2, 3, 4};

const static std::string test_ipv6_str = "0001:0203:0405:0607:0809:0A0B:0C0D:0E0F";
const static std::array<unsigned char, 16> test_ipv6_arr = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

unsigned short test_port = 1234;

unsigned long get_ipv4_as_long(const sockaddr_in& addr)
{
#ifdef _WIN32
    return addr.sin_addr.S_un.S_addr;
#elif __unix
    return addr.sin_addr.s_addr;
#else
    return 0;
#endif
}

std::array<unsigned char, 16> get_ipv6_as_array(const sockaddr_in6& addr)
{
    std::array<unsigned char, 16> rv{};
#ifdef _WIN32
    std::copy(addr.sin6_addr.u.Byte, addr.sin6_addr.u.Byte + 16, rv.begin());
#elif __unix
    std::copy(addr.sin6_addr.s6_addr, addr.sin6_addr.s6_addr + 16, rv.begin());
#endif
    return rv;
}

sockaddr_in ipv4_from_long(unsigned long addr)
{
    sockaddr_in rv{};
#ifdef _WIN32
    rv.sin_family = AF_INET;
    rv.sin_port = 0;
    rv.sin_addr.S_un.S_addr = addr;
#elif __unix
    rv.sin_family = AF_INET;
    rv.sin_port = 0;
    rv.sin_addr.s_addr = addr;
#endif
    return rv;
}

sockaddr_in6 ipv6_from_array(const std::array<unsigned char, 16>& addr)
{
    sockaddr_in6 rv{};
#ifdef _WIN32
    rv.sin6_family = AF_INET6;
    std::copy(addr.begin(), addr.end(), rv.sin6_addr.u.Byte);
#elif __unix
    rv.sin6_family = AF_INET6;
    std::copy(addr.begin(), addr.end(), rv.sin6_addr.s6_addr);
#endif
    return rv;
}

TEST_CASE("from_ipv4_str creates a sockaddr_in structure in network byte order", "[Endianness][system][ipv4]")
{
    sockaddr_in addr = from_ipv4_str(test_ipv4_str, test_port);

    unsigned long actual = get_ipv4_as_long(addr);

    // Network byte order means that the most significant byte has the lowest address
    REQUIRE(actual == test_ipv4_l);
    REQUIRE(addr.sin_port == htons(test_port));
}

TEST_CASE("from_ipv6_str creates a sockaddr_in6 structure in network byte order", "[Endianness][system][ipv6]")
{
    auto addr = from_ipv6_str(test_ipv6_str, test_port);

    auto actual = get_ipv6_as_array(addr);

    REQUIRE(actual == test_ipv6_arr);
    REQUIRE(addr.sin6_port == htons(test_port));
}

TEST_CASE("IpAddress string constructor creates an ipv4 address in network byte order", "[Endianness][IpAddress][ipv4]")
{
    sockets::abl::IpAddress addr(sockets::abl::ip_family::INET, test_ipv4_str, test_port);

    auto actual = addr.get_as_ipv4();

    REQUIRE(actual.address == test_ipv4_arr);
    REQUIRE(actual.port == htons(test_port));

    REQUIRE(addr.port() == htons(test_port));
}

TEST_CASE("IpAddress string constructor creates an ipv6 address in network byte order", "[Endianness][IpAddress][ipv6]")
{
    sockets::abl::IpAddress addr(sockets::abl::ip_family::INET6, test_ipv6_str, test_port);

    auto actual = addr.get_as_ipv6();

    REQUIRE(actual.address == test_ipv6_arr);
    REQUIRE(actual.port == htons(test_port));

    REQUIRE(addr.port() == htons(test_port));
}

TEST_CASE("from_ipv4 creates a sockaddr_in structure in network byte order", "[Endianness][system][ipv4]")
{
    sockets::abl::ipv4_addr addr{htons(test_port), test_ipv4_arr};

    auto actual = from_ipv4(addr);
    unsigned long actual_l = get_ipv4_as_long(actual);

    REQUIRE(actual_l == test_ipv4_l);
    REQUIRE(actual.sin_port == htons(test_port));
}

TEST_CASE("from_ipv6 creates a sockaddr_in6 structure in network byte order", "[Endianness][system][ipv6]")
{
    sockets::abl::ipv6_addr addr{htons(test_port), 0, test_ipv6_arr, 0};

    auto actual = from_ipv6(addr);
    auto actual_a = get_ipv6_as_array(actual);

    REQUIRE(actual_a == test_ipv6_arr);
    REQUIRE(actual.sin6_port == htons(test_port));
}

TEST_CASE("to_ipv4 creates an ipv4_addr object with an address in network byte order", "[Endianness][system][ipv4]")
{
    sockaddr_in addr = ipv4_from_long(test_ipv4_l);
    addr.sin_port = htons(test_port);

    auto actual = to_ipv4(addr);

    REQUIRE(actual.address == test_ipv4_arr);
    REQUIRE(actual.port == htons(test_port));
}

TEST_CASE("to_ipv6 creates an ipv6_addr object with an address in network byte order", "[Endianness][system][ipv6]")
{
    sockaddr_in6 addr = ipv6_from_array(test_ipv6_arr);
    addr.sin6_port = htons(test_port);

    auto actual = to_ipv6(addr);

    REQUIRE(actual.address == test_ipv6_arr);
    REQUIRE(actual.port == htons(test_port));
}