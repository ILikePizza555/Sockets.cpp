//
// Created by avris on 9/27/2018.
//

#pragma once

#include "abl.h"

namespace sockets {
    struct addr_t
    {
        socklen_t length = 0;
        std::unique_ptr<sockaddr> addr = nullptr;

        /**
         * Tests if the address is a local address.
         * @return
         */
        bool
        is_local();

        /**
         * Returns the textual representation of the address.
         */
        std::string
        name();
    };
}