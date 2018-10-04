//
// Created by avris on 10/4/2018.
//

#pragma once

#include "abl.h"

namespace sockets {
    // Define the socket type
#ifdef _WIN32
    typedef SOCKET sock_t;
    const static sock_t invalid_socket = INVALID_SOCKET;

    typedef int socklen_t;
#else
    //Unix
    typedef int sock_t;
    const static sock_t invalid_socket = -1;
#endif


    void set_ipv6_only(sock_t socket, bool enable);

    bool get_ipv6_only(sock_t socket);
}