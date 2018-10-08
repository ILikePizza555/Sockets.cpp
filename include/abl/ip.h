//
// Created by avris on 9/27/2018.
//

#pragma once

#include "enums.h"
#include <memory>
#include <vector>

namespace sockets {
    struct addr_t
    {
        std::unique_ptr<sockaddr_storage> addr_ptr = nullptr;
        size_t length = 0;

        sockaddr*
        as_sockaddr() const;

        sockaddr_in*
        as_sockaddr4() const;

        sockaddr_in6*
        as_sockaddr6() const;

        ip_family
        get_family() const;

        /**
         * Tests if the address is a loopback address.
         * @return
         */
        bool
        is_loopback() const;

        /**
         * Returns the textual representation of the address.
         */
        std::string
        name() const;

        /*
         * Returns the address's port.
         *
         * @return The address's port.
         */
        uint16_t
        port() const;
    };

    /**
     * Generates an addr_t structure from an ip address encoded as a string.
     *
     * @param family The family of ip address to use (either INET or INET6)
     * @param ip_address A string holding a textual representation of an ip address
     * @param port A port in host byte order
     * @return An addr_t structure
     */
    addr_t from_string(ip_family family, std::string ip_address, unsigned short port);

    /**
     * Class that carries the flags for getaddrinfo
     */
    class AddrInfoFlags
    {
    private:
        int flags;
    public:
        /** Sets all flags */
        void set_all();

        /** Enables ipv4 to ipv6 mapping */
        AddrInfoFlags& set_ipv4_mapping();

        /** Indicates that the address returned is intented to be used for the bind() method */
        AddrInfoFlags& set_passive();

        /** Indicates that getaddrinfo should only resolve if a valid global address is configured */
        AddrInfoFlags& set_addr_config();

        /** Indicates that the system should not perform name resolution */
        AddrInfoFlags& set_numeric_host();

        /** Indicates that the port is numeric*/
        AddrInfoFlags& set_numeric_serv();

        /** Returns an integer to pass to the system */
        int get();
    };

    struct address_info
    {
        ip_family family;
        sock_type socket_type;
        sock_proto protocol;
        addr_t address;

        address_info(ip_family family, sock_type socket_type, sock_proto protocol, addr_t address);
        address_info(int family, int socket_type, int protocol, addr_t address);
    };


    std::vector<address_info>
    get_address_info(const std::string& host,
                    const std::string& port,
                    AddrInfoFlags& flags,
                    ip_family hint_family,
                    sock_type hint_type,
                    sock_proto hint_proto);
}