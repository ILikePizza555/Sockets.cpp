#pragma once

namespace sockets {
    namespace abl {
        /**
         * Define the supported address families
         */
        enum ip_family : int
        {
            ANY,
            INET,
            INET6
        };

        /**
         * Define the supported socket types
         */
        enum sock_type : int
        {
            STREAM,
            DATAGRAM,
            RAW
        };

        /**
         * Define the supported socket protocols
         */
        enum sock_proto : int
        {
            TCP,
            UDP
        };
    }
}
