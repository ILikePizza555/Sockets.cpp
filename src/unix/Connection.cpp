//
// Created by avris on 9/27/2018.
//

#include <Connection.h>
#include <netdb.h>

namespace sockets
{
    Connection<> connect_to(std::string host, std::string port)
    {
        addrinfo hints{
            0,
            AF_UNSPEC,
            SOCK_STREAM,
            IPPROTO_TCP,
            0,
            nullptr,
            nullptr,
            nullptr
        };

        // First we create a new addrinfo struct to use to open a socket, and then pass it to getaddrinfo to fill it.
        // Then we transfer ownership of the raw pointer into a unique pointer to take care of memory management
        // automatically.
        auto result_ptr = new addrinfo;
        auto value = getaddrinfo(host.c_str(), port.c_str(), &hints, &result_ptr);
        auto result = std::unique_ptr<addrinfo, decltype(&freeaddrinfo)>(result_ptr, freeaddrinfo);
        result_ptr = nullptr;

        if(value != 0) throw AddressInfoError(__func__, value);

        Socket s(result->ai_family, result->ai_socktype, result->ai_protocol);
        s.connect(result->ai_addr, result->ai_addrlen);

        return Connection<>(s);
    }
}