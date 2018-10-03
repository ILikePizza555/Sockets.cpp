//
// Created by avris on 9/27/2018.
//

#include <abl/ip.h>
#include <cstring>
#include <netdb.h>
#include <Error.h>

namespace sockets {
    sockaddr*
    addr_t::as_sockaddr() const
    {
        return reinterpret_cast<sockaddr*>(addr_ptr.get());
    }

    sockaddr_in*
    addr_t::as_sockaddr4() const
    {
        return reinterpret_cast<sockaddr_in*>(addr_ptr.get());
    }

    sockaddr_in6*
    addr_t::as_sockaddr6() const
    {
        return reinterpret_cast<sockaddr_in6*>(addr_ptr.get());
    }

    ip_family
    addr_t::get_family() const
    {
        return ip_family(addr_ptr->ss_family);
    }

    bool
    addr_t::is_loopback() const
    {
        if (this->addr_ptr->ss_family == ip_family::INET)
        {
            auto *a = reinterpret_cast<const sockaddr_in *>(this->addr_ptr.get());
            uint32_t addr = a->sin_addr.s_addr;
            return (addr & 0xFF000000) == 0x7F000000;
        }

        if (this->addr_ptr->ss_family == ip_family::INET6)
        {
            auto *a = reinterpret_cast<const sockaddr_in6 *>(this->addr_ptr.get());
            auto &addr = a->sin6_addr.s6_addr;

            return addr[0] == 0 && addr[1] == 0 && addr[2] == 0 && addr[4] == 0 &&
                   addr[5] == 0 && addr[6] == 0 && addr[7] == 0 && addr[8] == 0 &&
                   addr[9] == 0 && addr[10] == 0 && addr[11] == 0 && addr[12] == 0 &&
                   addr[13] == 0 && addr[14] == 0 && addr[15] == 0;
        }

        return false;
    }

    std::string
    addr_t::name() const
    {
        const static socklen_t size = 40;
        char *buf = new char[size];

        const char *result = inet_ntop(this->addr_ptr->ss_family, this->addr_ptr.get(), buf, size);
        if (result == nullptr)
            throw MethodError("addr_t::name", "inet_ntop", errno, get_error_message);

        return std::string(buf, size);
    }

    void AddrInfoFlags::set_all() { flags = AI_ALL; }

    AddrInfoFlags& AddrInfoFlags::set_ipv4_mapping()
    {
        flags |= AI_V4MAPPED;
        return *this;
    }

    AddrInfoFlags& AddrInfoFlags::set_passive()
    {
        flags |= AI_PASSIVE;
        return *this;
    }

    AddrInfoFlags& AddrInfoFlags::set_addr_config()
    {
        flags |= AI_ADDRCONFIG;
        return *this;
    }

    AddrInfoFlags& AddrInfoFlags::set_numeric_host()
    {
        flags |= AI_NUMERICHOST;
        return *this;
    }

    AddrInfoFlags& AddrInfoFlags::set_numeric_serv()
    {
        flags |= AI_NUMERICSERV;
        return *this;
    }

    int AddrInfoFlags::get()
    {
        return flags;
    }

    address_info::address_info(sockets::ip_family family,
                               sockets::sock_type socket_type,
                               sockets::sock_proto protocol,
                               sockets::addr_t address) :
                               family(family), socket_type(socket_type), protocol(protocol), address(std::move(address))
    {}

    address_info::address_info(int family, int socket_type, int protocol, sockets::addr_t address) :
        family(sockets::ip_family(family)),
        socket_type(sockets::sock_type(socket_type)),
        protocol(sockets::sock_proto(protocol)),
        address(std::move(address))
    {}

    std::vector<address_info>
    get_address_info(const std::string &host, const std::string &port, AddrInfoFlags& flags,
                     sockets::ip_family hint_family, sockets::sock_type hint_type, sockets::sock_proto hint_proto)
    {
        std::vector<address_info> rv;

        addrinfo hints
        {
            flags.get(),
            hint_family,
            hint_type,
            hint_proto,
            0,
            nullptr,
            nullptr,
            nullptr
        };

        auto* addrinfo_ptr = new addrinfo;
        auto result = getaddrinfo(host.c_str(), port.c_str(), &hints, &addrinfo_ptr);
        if(result != 0)
            throw MethodError(__func__, "getaddrinfo", result, [](int ec){return std::string(gai_strerror(ec));});

        // Vector construction loop
        auto next = addrinfo_ptr;
        while(next != NULL)
        {
            // Copy sockaddr to avoid deletion
            auto* sockaddr_copy = new sockaddr_storage;
            std::memcpy(sockaddr_copy, next->ai_addr, next->ai_addrlen);

            rv.emplace_back(next->ai_family,
                    next->ai_socktype,
                    next->ai_protocol,
                    addr_t{std::unique_ptr<sockaddr_storage>(sockaddr_copy), next->ai_addrlen});

            next = next->ai_next;
        }

        freeaddrinfo(addrinfo_ptr);
        return rv;
    }
}