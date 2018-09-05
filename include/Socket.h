//
// Created by avris on 9/5/2018.
//

#pragma once

#include "sockets.h"

namespace sockets {
    /**
     * Class wrapper for bsd style sockets.
     *
     * This class does not perform any kind of resource management. This is because this class is intended as a very
     * thin layer of abstraction between the platform and the rest of this library.
     */
    struct Socket {
        sock_t socket;

        Socket(sock_addr_family, sock_type, sock_proto);

        
    };
}