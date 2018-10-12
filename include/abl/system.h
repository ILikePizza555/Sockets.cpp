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

#elif __unix

#include <sys/socket.h>

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

#elif __unix
            int get_system_handle(const handle_t* handle);
            int get_system_handle(const UniqueHandle& handle);
            int get_system_handle(const SharedHandle& handle);

            handle_t* from_system_handle(int handle);
#endif

            int
            iftosys(ip_family family);

            int
            sttosys(sock_type type);

            int
            sptosys(sock_proto proto);

            sockaddr_in
            ipv4_to_system(const ipv4_addr &address);

            sockaddr_in6
            ipv6_to_system(const ipv6_addr &address);

            ipv4_addr
            system_to_ipv4(const sockaddr_in &addr);

            ipv6_addr
            system_to_ipv6(const sockaddr_in6 &addr);

            sockaddr_in
            ipv4str_to_addr(const std::string &str);

            sockaddr_in6
            ipv6str_to_addr(const std::string &str);

            std::string
            to_string(const sockaddr_in &ipv4_addr);

            std::string
            to_string(const sockaddr_in6 &ipv6_addr);
        }
    }
}