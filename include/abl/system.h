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

namespace sockets {
    namespace abl {
#ifdef _WIN32
        SOCKET get_system_handle(const handle_t* handle);
        SOCKET get_system_handle(const UniqueHandle& handle);
        SOCKET get_system_handle(const SharedHandle& handle);

        handle_t* from_system_handle(SOCKET handle);
#elif __unix
        int get_system_handle(const handle_t* handle);
        int get_system_handle(const UniqueHandle& handle);
        int get_system_handle(const SharedHandle& handle);

        handle_t* from_system_handle(int handle);
#endif

        int iftosys(ip_family family);
        int sttosys(sock_type type);
        int sptosys(sock_proto proto);

        sockaddr_in ipv4_to_system(ipv4_addr address);
        sockaddr_in6 ipv6_to_system(ipv6_addr address);
    }
}