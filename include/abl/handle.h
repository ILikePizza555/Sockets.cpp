//
// Defines the abstraction for the system socket handle.
// Created by avris on 10/4/2018.
//

#pragma once

#include "enums.h"
#include <memory>

namespace sockets {
    namespace abl {
        /**
         * Struct that contains the system socket handle.
         */
        struct handle_t;

        /**
         * Destructor for the handle.
         * This function is not intended to be called directly. Use UniqueHandle or SharedHandle instead.
         * @param handle
         */
        void close_handle(handle_t handle);

        using UniqueHandle = std::unique_ptr<handle_t>;
        using SharedHandle = std::shared_ptr<handle_t>;

        using HandleRef = const handle_t *;

        UniqueHandle new_unique_handle(ip_family family, sock_type type, sock_proto protocol);
        SharedHandle new_shared_handle(ip_family family, sock_type type, sock_proto protocol);
    }
}