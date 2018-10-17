//
// Created by avris on 10/16/2018.
//

#include "catch.hpp"
#include <abl/ip.h>
#include <Error.h>

using sockets::abl::IpAddress;

TEST_CASE("IpAddress constructor", "[IpAddress]")
{
    SECTION("Default constructor should create an invalid state")
    {
        IpAddress addr;

        REQUIRE(!addr.is_ipv4());
        REQUIRE(!addr.is_ipv6());
    }

    SECTION("ipv4 constructor should copy and tag the address")
    {
        sockets::abl::ipv4_addr test_addr{777, {{13, 37, 4, 20}}};
        IpAddress test_ip_addr(test_addr);

        REQUIRE(test_ip_addr.is_ipv4());
        REQUIRE(!test_ip_addr.is_ipv6());

        REQUIRE(test_ip_addr.get_as_ipv4().port == test_addr.port);
        REQUIRE(test_ip_addr.get_as_ipv4().address == test_addr.address);
    }

    SECTION("ipv6 constructor should copy and tag the address")
    {
        sockets::abl::ipv6_addr test_addr{77, 0, {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}}, 0};
        IpAddress test_ip_addr(test_addr);

        REQUIRE(!test_ip_addr.is_ipv4());
        REQUIRE(test_ip_addr.is_ipv6());

        REQUIRE(test_ip_addr.get_as_ipv6().port == test_addr.port);
        REQUIRE(test_ip_addr.get_as_ipv6().address == test_addr.address);
        REQUIRE(test_ip_addr.get_as_ipv6().scope_id == test_addr.scope_id);
        REQUIRE(test_ip_addr.get_as_ipv6().flowinfo== test_addr.flowinfo);
    }

    SECTION("ipv4 string constructor should correctly parse a string containing an address in dot-decimal notation")
    {
        std::string addr_str = "13.37.4.20";
        IpAddress test_addr(sockets::abl::ip_family::INET, addr_str, 777);

        REQUIRE(test_addr.is_ipv4());
        REQUIRE(!test_addr.is_ipv6());

        const auto& v4addr = test_addr.get_as_ipv4();
        REQUIRE(v4addr.port == 777);
        REQUIRE(v4addr.address == std::array<unsigned char, 4>{{13, 37, 4, 20}});
    }

    SECTION("ipv6 string constructor should correctly parse a string containing an address in ipv6 notation")
    {
        std::string addr_str = "1337:0:0eab:DEAD::abcd:4e";
        IpAddress test_addr(sockets::abl::ip_family ::INET6, addr_str, 777);

        REQUIRE(!test_addr.is_ipv4());
        REQUIRE(test_addr.is_ipv6());

        const auto& v6addr = test_addr.get_as_ipv6();
        REQUIRE(v6addr.port == 777);
        REQUIRE(v6addr.address == std::array<unsigned char, 16>{{0x13, 0x37, 0, 0, 0x0e, 0xab, 0xDE, 0xAD, 0, 0, 0, 0, 0xab, 0xcd, 0, 0x4e}});
    }
}

TEST_CASE("IpAddress::get_as_ipv4", "[IpAddress]")
{
    SECTION("should return an ipv4_addr struct if initialized with an ipv4 address")
    {
        const IpAddress test_addr(sockets::abl::ipv4_addr{777, {{13, 37, 4, 20}}});

        REQUIRE(test_addr.is_ipv4());
        REQUIRE_NOTHROW(test_addr.get_as_ipv4());
    }

    SECTION("should throw an error if initialized without an ipv4 address")
    {
        const IpAddress test_addr(sockets::abl::ipv6_addr{777, 0, {{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}}, 0});

        REQUIRE(!test_addr.is_ipv4());
        REQUIRE_THROWS(test_addr.get_as_ipv4());
    }
}

TEST_CASE("IpAddress::get_as_ipv6", "[IpAddress]")
{
    SECTION("should return an ipv6_addr struct if initialized with an ipv6 address")
    {
        const IpAddress test_addr(sockets::abl::ipv6_addr{777, 0, {{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}}, 0});

        REQUIRE(test_addr.is_ipv6());
        REQUIRE_NOTHROW(test_addr.get_as_ipv6());
    }

    SECTION("should throw an error if initialized without and ipv6 address")
    {
        const IpAddress test_addr(sockets::abl::ipv4_addr{777, {{13, 37, 4, 20}}});

        REQUIRE(!test_addr.is_ipv6());
        REQUIRE_THROWS(test_addr.get_as_ipv6());
    }
}