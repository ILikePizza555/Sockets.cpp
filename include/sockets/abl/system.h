//
// Defines methods to convert library types to system types safely
// Created by avris on 10/7/2018.
//

#pragma once

#include "ip.h"
#include "handle.h"
#include "enums.h"

#ifdef _WIN32

#include <winsock2.h>
#include <ws2ipdef.h>

using socklen_t = int;

#elif __unix

#include <sys/socket.h>
#include <netinet/in.h>

#endif

#include <string>

namespace sockets {
    namespace abl {
        namespace system {
#ifdef _WIN32

            SOCKET
            get_system_handle(const handle_t *handle);

            SOCKET
            get_system_handle(const UniqueHandle &handle);

            SOCKET
            get_system_handle(const SharedHandle &handle);

            handle_t *
            from_system_handle(SOCKET handle);

            UniqueHandle
            unique_from_system_handle(SOCKET handle);

            SharedHandle
            shared_from_system_handle(SOCKET handle);

#elif __unix
            int get_system_handle(const handle_t* handle);
            int get_system_handle(const UniqueHandle& handle);
            int get_system_handle(const SharedHandle& handle);

            handle_t* from_system_handle(int handle);

            UniqueHandle
            unique_from_system_handle(int handle);

            SharedHandle
            shared_from_system_handle(int handle);
#endif

            /**
             * Converts the ip_family enum into a valid parameter for the system socket call.
             */
            int
            iftosys(ip_family family);

            ip_family
            systoif(int family);

            /**
             * Converts the sock_type enum into a valid parameter for the system socket call.
             */
            int
            sttosys(sock_type type);

            sock_type
            systost(int type);

            /**
             * Converts the sock_proto enum into a valid parameter for the system socket call.
             */
            int
            sptosys(sock_proto proto);

            sock_proto
            systosp(int proto);

            sockaddr_in
            from_ipv4(const ipv4_addr &address);

            sockaddr_in6
            from_ipv6(const ipv6_addr &address);

            ipv4_addr
            to_ipv4(const sockaddr_in &addr);

            ipv6_addr
            to_ipv6(const sockaddr_in6 &addr);

            IpAddress
            to_ipaddress(const sockaddr *addr);

            /**
             * Creates a sockaddr_in object from a string and a port.
             *
             * @param str A string containing an ipv4 address in dot-decimal notation.
             * @param port The port in host byte order.
             * @return
             */
            sockaddr_in
            from_ipv4_str(const std::string &str, uint16_t port);

            sockaddr_in6
            from_ipv6_str(const std::string &str, uint16_t port);

            std::string
            to_string(const sockaddr_in &ipv4_addr);

            std::string
            to_string(const sockaddr_in6 &ipv6_addr);
        }
    }
}