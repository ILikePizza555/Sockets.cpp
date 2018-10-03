//
// Created by avris on 9/27/2018.
//

#pragma once

#include "abl.h"
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
         * @return The address's port as an integer, or -1 if there was an error.
         */
        int
        port() const;
    };

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