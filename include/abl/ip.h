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
        socklen_t length = 0;

        sockaddr*
        as_sockaddr();

        sockaddr_in*
        as_sockaddr4();

        sockaddr_in6*
        as_sockaddr6();

        ip_family
        get_family();

        /**
         * Tests if the address is a loopback address.
         * @return
         */
        bool
        is_loopback();

        /**
         * Returns the textual representation of the address.
         */
        std::string
        name();
    };

    struct address_info
    {
        const ip_family family;
        const sock_type socket_type;
        const sock_proto protocol;
        const addr_t address;

        address_info(ip_family family, sock_type socket_type, sock_proto protocol, addr_t address);
        address_info(int family, int socket_type, int protocol, addr_t address);
    };

    std::vector<address_info>
    get_address_info(const std::string& host,
                    const std::string& port,
                    int flags,
                    ip_family hint_family,
                    sock_type hint_type,
                    sock_proto hint_proto);
}