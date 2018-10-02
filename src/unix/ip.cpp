//
// Created by avris on 9/27/2018.
//

#include <abl/ip.h>
#include <Error.h>

namespace sockets {
    sockaddr*
    addr_t::as_sockaddr()
    {
        return reinterpret_cast<sockaddr*>(addr_ptr.get());
    }

    sockaddr_in*
    addr_t::as_sockaddr4()
    {
        return reinterpret_cast<sockaddr_in*>(addr_ptr.get());
    }

    sockaddr_in6*
    addr_t::as_sockaddr6()
    {
        return reinterpret_cast<sockaddr_in6*>(addr_ptr.get());
    }

    bool
    addr_t::is_loopback()
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
    addr_t::name()
    {
        const static socklen_t size = 40;
        char *buf = new char[size];

        const char *result = inet_ntop(this->addr_ptr->ss_family, this->addr_ptr.get(), buf, size);
        if (result == nullptr)
            throw MethodError("addr_t::name", "inet_ntop", errno, get_error_message);

        return std::string(buf, size);
    }
}