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
        /**
         * Struct that holds data for an ipv4 address.
         *
         * This struct must be trivially defined for use in the addr_t union.
         *
         * All values are stored in network byte order.
         */
        struct ipv4_addr
        {
            uint16_t port;

            /** The ip address stored as an array of bytes*/
            std::array<unsigned char, 4> address;
        };

        /**
         * Struct that holds data for an ipv6 address.
         *
         * This struct must be trivially defined for use in the addr_t union.
         *
         * All values are stored in network byte order.
         */
        struct ipv6_addr
        {
            uint16_t port;

            uint32_t flowinfo;
            /** The ip address stored as an array of bytes in big-endian format */
            std::array<unsigned char, 16> address;
            uint32_t scope_id;
        };

        struct addr_t
        {
            ip_family family = ip_family::ANY;

            union
            {
                ipv4_addr v4addr;
                ipv6_addr v6addr;
            };
        };


        class IpAddress
        {
        private:
            addr_t addr{};

        public:
            IpAddress() = default;

            explicit IpAddress(ipv4_addr addr);
            explicit IpAddress(ipv6_addr addr);
            /**
             * Creates a new IpAddress from an ip address encoded as a string.
             *
             * @param family The family of the ip address.
             * @param address The textual representation of the ip address
             * @param port The port in host byte order
             */
            IpAddress(ip_family family, const std::string& address, uint16_t port);

            addr_t& get_addr();
            const addr_t& get_addr() const;

            /**
             * Gets the address as an ipv4 struct. Throws an exception if the internal address is not an ipv4 address.
             * @return
             */
            ipv4_addr& get_as_ipv4();
            const ipv4_addr& get_as_ipv4() const;

            /**
             * Gets the address as an ipv6 struct. Throws an exception if the internal address is not an ipv6 address.
             * @return
             */
            ipv6_addr& get_as_ipv6();
            const ipv6_addr& get_as_ipv6() const;

            bool is_ipv4() const;

            bool is_ipv6() const;

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