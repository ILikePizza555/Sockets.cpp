#pragma once

#include <string>
#include "abl/abl.h"
#include "abl/ip.h"
#include "Connection.h"

namespace sockets {
    namespace {
        inline unsigned short get_port_from_string(const std::string& port)
        {
            unsigned long raw_port_num = std::stoul(port);

            if (raw_port_num >= std::numeric_limits<unsigned short>::max())
                throw std::out_of_range("port " + port + " is out of range");

            return static_cast<unsigned short>(raw_port_num);
        }
    }

    template<typename T = TCPSocket>
    class ServerSocket
    {
    private:
        T _socket;
    public:
        /**
         * Creates a new socket and binds it to localhost with the specified port.
         * @param port The port to bind on
         * @param family The address family the socket should support.
         * @param enable_v6_map Whether or not ipv6 mapping should be enabled on the socket. This parameter is ignored
         * if family is not set to ip_family::INET6.
         */
        explicit ServerSocket(const std::string& port,
                              ip_family family = ip_family::INET6,
                              bool enable_v6_map = true) : _socket(family)
        {
            unsigned short port_num = get_port_from_string(port);
            std::string ip;

            if(family == ip_family::INET) ip = "127.0.0.1";
            else if(family == ip_family::INET6) ip = "::1";

            auto addr = from_string(family, ip, port_num);

            set_ipv6_only(_socket.socket, !enable_v6_map);

            _socket.bind(addr);
            _socket.listen();
        }

        /**
         * Creates a new socket and binds it to the specified host on the specified port.
         */
        ServerSocket(const std::string& host, const std::string& port, bool enable_v6_map = true)
        {
            AddrInfoFlags flags = AddrInfoFlags{}.set_addr_config().set_ipv4_mapping();

            auto addr = get_address_info(host, port, flags, ip_family::ANY, sock_type::STREAM, sock_proto::TCP);

            set_ipv6_only(_socket.socket, !enable_v6_map);

            _socket.bind(addr[0]);
            _socket.listen();
        }

        /**
         * Creates a new socket and binds it using the address
         */
        explicit ServerSocket(addr_t address, bool enable_v6_map = true)
        {
            set_ipv6_only(_socket.socket, !enable_v6_map);

            _socket.bind(address);
            _socket.listen();
        }

        explicit ServerSocket(T socket) : _socket(socket) {}

        ~ServerSocket()
        {
            _socket.close();
        }

        // Delete copy constructor
        ServerSocket(ServerSocket& other) = delete;

        // Delete copy assignment
        ServerSocket& operator=(ServerSocket& other) = delete;

        /**
         * Establishes a connection with the first available client. If none exist, this function blocks until a connection is available.
         */
        Connection<T>
        accept()
        {
            T client_socket = _socket.accept();

            return Connection<T>(client_socket);
        }

        T& get_socket()
        {
            return _socket;
        }
    };
}
