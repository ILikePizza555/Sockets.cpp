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

