#pragma once

#include <string>
#include "sockets.h"

namespace sockets {
    class ServerSocket 
    {
        using std::string;

        private:
            std::string host;
            std::string port;

            sock_t _socket;
        public:
            ServerSocket(string host, string port);
            ~ServerSocket();

            /**
             * Enables the socket to begin listening for connections
             */
            void listen(int backlog) const; 
            
            /**
             * Establishes a connection with the first available client. If none exist, this function blocks until a connection is available.
             */
            Connection accept(size_t bufferCapacity);
    };
}
