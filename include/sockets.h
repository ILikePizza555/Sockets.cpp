#pragma once

#ifdef _WIN32
//Windows Headers
#include <winsock2.h>
#else
//Unix Headers
#include <sys/socket.h>
#endif

namespace sockets {
    // Define the socket type
    #ifdef _WIN32
    typedef SOCKET sock_t;
    const static sock_t invalid_socket = INVALID_SOCKET;
    #else
    //Unix
    typedef int sock_t;
    const static sock_t invalid_socket = -1;
    #endif

    // Define address types
    #ifdef _WIN32
    typedef sockaddr sockaddr_t;
    typedef int sockaddr_len_t;
    #else
    typedef sockaddr sockaddr_t;
    typedef socklen_t sockaddr_len_t;
    #endif

    /**
     * Define the supported address families
     */
    enum  sock_addr_family : int {
        INET = AF_INET,
        INET6 = AF_INET
    };

    /**
     * Define the supported socket types
     */
    enum sock_type : int {
        STREAM = SOCK_STREAM,
        DATAGRAM = SOCK_DGRAM,
        RAW = SOCK_RAW
    };

    /**
     * Define the supported socket protocols
     */
     enum sock_proto : int {
         TCP = IPPROTO_TCP,
         UDP = IPPROTO_UDP
     };

    /**
     * Calls any functions that the implementation might require before using sockets. (i.e WSASetup for windows)
     */
    void setup();

    /**
     * Calls any functions that the implementation might require after using sockets.
     */
    void teardown();
}
