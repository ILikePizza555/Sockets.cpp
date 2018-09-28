//
// Created by avris on 9/27/2018.
//

#pragma once

#include "abl.h"
#include <memory>

namespace sockets {
    struct addr_t
    {
        std::unique_ptr<sockaddr_storage> addr_ptr = nullptr;
        socklen_t length = 0;

        /**
         * Constructs a new addr_t to use as a buffer.
         * @return An addr_t structure
         */
        static addr_t
        new_buffer()
        {
            return addr_t{std::make_unique<sockaddr_storage>(), sizeof(sockaddr_storage)};
        }

        sockaddr*
        as_sockaddr();

        sockaddr_in*
        as_sockaddr4();

        sockaddr_in6*
        as_sockaddr6();

        /**
         * Tests if the address is a loopback address.
         * @return
         */
        bool
        is_loopback();

        /**
         * Returns the textual representation of the address.
         */
        std::string
        name();
    };
}