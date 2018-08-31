#pragma once

#ifdef _WIN32
//Windows Headers
#include <winsock2.h>
#else
//Unix Headers
#include <sys/socket.h>
#endif

namespace sockets {
    #ifdef _WIN32
    // Windows-specific
    typedef SOCKET sock_t;
    const static sock_t invalid_socket = INVALID_SOCKET
    #else
    //Unix
    typedef int sock_t;
    const static sock_t invalid_socket = -1;
    #endif
    
}