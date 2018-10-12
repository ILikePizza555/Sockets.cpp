//
// Created by avris on 9/27/2018.
//

#pragma once

#include "enums.h"

#include <array>
#include <memory>
#include <vector>

namespace sockets {
    namespace abl {
        struct ipv4_addr
        {
            uint16_t port = 0;

            /** The ip address stored as an array of bytes in host byte order */
            std::array<unsigned char, 4> address = {};
        };

        struct ipv6_addr
        {
            uint16_t port = 0;

            uint32_t flowinfo;
            /** The ip address stored as an array of bytes in host byte order */
            std::array<unsigned char, 16> address = {};
            uint32_t scope_id;
        };

        union addr_t
        {
            ipv4_addr v4addr;
            ipv6_addr v6addr;
        };


        class IpAddress
        {
        private:
            ip_family family = ip_family::ANY;
            std::unique_ptr<addr_t> addr_ptr = nullptr;

        public:
            IpAddress() = default;
            IpAddress(std::unique_ptr<addr_t>&& addr_ptr, ip_family family);
            IpAddress(IpAddress&& other) noexcept;

            IpAddress& operator=(IpAddress&& other) noexcept;

            /**
             * Creates a new IpAddress from an ip address encoded as a string.
             *
             * @param family The family of the ip address.
             * @param address The textual representation of the ip address
             * @param port The port in host byte order
             */
            IpAddress(ip_family family, const std::string& address, uint16_t port);

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
         * Class that carries the flags for getaddrinfo
         */
        class AddrInfoFlags
        {
        private:
            int flags;
        public:
            /** Sets all flags */
            void
            set_all();

            /** Enables ipv4 to ipv6 mapping */
            AddrInfoFlags &
            set_ipv4_mapping();

            /** Indicates that the address returned is intented to be used for the bind() method */
            AddrInfoFlags &
            set_passive();

            /** Indicates that getaddrinfo should only resolve if a valid global address is configured */
            AddrInfoFlags &
            set_addr_config();

            /** Indicates that the system should not perform name resolution */
            AddrInfoFlags &
            set_numeric_host();

            /** Indicates that the port is numeric*/
            AddrInfoFlags &
            set_numeric_serv();

            /** Returns an integer to pass to the system */
            int
            get();
        };

        struct address_info
        {
            ip_family family;
            sock_type socket_type;
            sock_proto protocol;
            IpAddress address;

            address_info(ip_family family, sock_type socket_type, sock_proto protocol, IpAddress address);

            address_info(int family, int socket_type, int protocol, IpAddress address);
        };


        std::vector<address_info>
        get_address_info(const std::string &host,
                         const std::string &port,
                         AddrInfoFlags &flags,
                         ip_family hint_family,
                         sock_type hint_type,
                         sock_proto hint_proto);
    }
}