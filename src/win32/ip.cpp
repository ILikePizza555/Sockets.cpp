//
// Created by avris on 9/27/2018.
//

#include <abl/ip.h>
#include <Error.h>
#include <inaddr.h>
#include <ws2ipdef.h>
#include <in6addr.h>

namespace sockets {
    sockaddr *
    addr_t::as_sockaddr()
    {
        return reinterpret_cast<sockaddr *>(addr_ptr.get());
    }

    sockaddr_in *
    addr_t::as_sockaddr4()
    {
        return reinterpret_cast<sockaddr_in *>(addr_ptr.get());
    }

    sockaddr_in6 *
    addr_t::as_sockaddr6()
    {
        return reinterpret_cast<sockaddr_in6 *>(addr_ptr.get());
    }

    ip_family
    addr_t::get_family()
    {
        return ip_family(addr_ptr->ss_family);
    }

    bool
    addr_t::is_loopback()
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

    std::string
    addr_t::name()
    {
        static unsigned long size = 40;
        char *rv = new char[size];

        int result = WSAAddressToStringA(reinterpret_cast<sockaddr *>(this->addr_ptr.get()), this->length, nullptr, rv,
                                         &size);
        if (result != 0)
            throw MethodError("addr_t::name", "WSAAddressToStringA");

        return std::string(rv);
    }
}