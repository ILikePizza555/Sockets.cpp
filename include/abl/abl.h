#pragma once

#ifdef _WIN32
//Windows Headers
#include <winsock2.h>
#include <Ws2tcpip.h>
#else
//Unix Headers
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SOCKET_ERROR -1

#endif

namespace sockets {
    // Define the socket type
#ifdef _WIN32
    typedef SOCKET sock_t;
    const static sock_t invalid_socket = INVALID_SOCKET;

    typedef uint32_t socklen_t;
#else
    //Unix
    typedef int sock_t;
    const static sock_t invalid_socket = -1;
#endif
    /**
     * Define the supported address families
     */
    enum ip_family : int
    {
        ANY = AF_UNSPEC,
        INET = AF_INET,
        INET6 = AF_INET6
    };

    /**
     * Define the supported socket types
     */
    enum sock_type : int
    {
        STREAM = SOCK_STREAM,
        DATAGRAM = SOCK_DGRAM,
        RAW = SOCK_RAW
    };

    /**
     * Define the supported socket protocols
     */
    enum sock_proto : int
    {
        TCP = IPPROTO_TCP,
        UDP = IPPROTO_UDP
    };

    /**
     * Calls any functions that the implementation might require before using sockets. (i.e WSASetup for windows)
     */
    void
    setup();

    /**
     * Calls any functions that the implementation might require after using sockets.
     */
    void
    teardown();
}
