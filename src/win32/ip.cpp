//
// Created by avris on 9/27/2018.
//

#include <abl/abl.h>
#include <abl/ip.h>
#include <cstring>
#include <Error.h>

#define AI_ALL 0x0100
#define AI_V4MAPPED 0x0800
#define AI_ADDRCONFIG 0x0400

namespace sockets {
    addr_t::addr_t(sockets::addr_t &&o) noexcept : addr_ptr(std::move(o.addr_ptr)), length(o.length)
    {
        o.length = 0;
    }

    addr_t& addr_t::operator=(sockets::addr_t &&o) noexcept
    {
        if(this != &o)
        {
            addr_ptr = std::move(o.addr_ptr);
            length = o.length;

            o.length = 0;
        }
        return *this;
    }


    sockaddr *
    addr_t::as_sockaddr() const
    {
        return reinterpret_cast<sockaddr *>(addr_ptr.get());
    }

    sockaddr_in *
    addr_t::as_sockaddr4() const
    {
        return reinterpret_cast<sockaddr_in *>(addr_ptr.get());
    }

    sockaddr_in6 *
    addr_t::as_sockaddr6() const
    {
        return reinterpret_cast<sockaddr_in6 *>(addr_ptr.get());
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
            return (a->sin_addr.S_un.S_addr & 0xFF000000) == 0x7F000000;
        }

        if (this->addr_ptr->ss_family == ip_family::INET6)
        {
            auto *a = reinterpret_cast<const sockaddr_in6 *>(this->addr_ptr.get());
            auto &ip_bytes = a->sin6_addr.u.Word;

            return ip_bytes[0] == 0 && ip_bytes[1] == 0 && ip_bytes[2] == 0 &&
                   ip_bytes[3] == 0 && ip_bytes[4] == 0 && ip_bytes[5] == 0 &&
                   ip_bytes[6] == 0 && ip_bytes[7] == 1;
        }

        return false;
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
        // Not supported, so do nothing
        return *this;
    }

    int AddrInfoFlags::get()
    {
        return flags;
    }

    std::string
    addr_t::name() const
    {
        static unsigned long size = 40;
        char *rv = new char[size];

        int result = WSAAddressToStringA(reinterpret_cast<sockaddr *>(this->addr_ptr.get()),
                                         this->length,
                                         nullptr,
                                         rv,
                                         &size);
        if (result != 0)
            throw MethodError("addr_t::name", "WSAAddressToStringA");

        return std::string(rv);
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
    get_address_info(const std::string& host, const std::string& port, AddrInfoFlags& flags,
                     ip_family hint_family, sock_type hint_type, sock_proto hint_proto)
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