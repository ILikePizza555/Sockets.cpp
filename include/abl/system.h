//
// Defines methods to convert library types to system types safely
// Created by avris on 10/7/2018.
//

#pragma once

#include "handle.h"

#ifdef _WIN32

#include <winsock2.h>

#elif __unix

#include <sys/socket.h>

#endif

namespace sockets {
    namespace abl {
#ifdef _WIN32
        SOCKET get_system_handle(const handle_t* handle);
        SOCKET get_system_handle(const UniqueHandle& handle);
        SOCKET get_system_handle(const SharedHandle& handle);
#elif __unix
        int get_system_handle(const handle_t* handle);
        int get_system_handle(const UniqueHandle& handle);
        int get_system_handle(const SharedHandle& handle);
#endif
    }
}