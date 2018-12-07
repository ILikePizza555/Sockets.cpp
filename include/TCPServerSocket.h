//
// Created by avris on 12/6/2018.
//
#pragma once

#include "TCPSocket.h"
#include "Connection.h"
#include "abl/ip.h"

namespace sockets {

    /**
     * Wrapper around a TCPSocket that binds to a local port and listens for incoming connections.
     */
    class TCPServerSocket
    {
    protected:
        TCPSocket _serverSocket;
    public:
        /**
         * Creates a new TCPServerSocket bound to the specified ip address
         * @param addr
         */
        explicit TCPServerSocket(const abl::IpAddress& addr, int backlog = 1024);
        TCPServerSocket(const std::string& ip, const std::string& port, int backlog = 1024);

        TCPConnection accept() const;
        std::tuple<TCPConnection, abl::IpAddress> acceptfrom() const;
    };
}