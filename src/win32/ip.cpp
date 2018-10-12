//
// Created by avris on 10/11/2018.
//

#include <abl/ip.h>
#include <abl/system.h>
#include <Error.h>
#include <inaddr.h>
#include <in6addr.h>

sockaddr_in ipv4strtoaddr(char* str)
{
    sockaddr_in s{};
    s.sin_family = AF_INET;
    int s_size = sizeof(s);

    int result = WSAStringToAddressA(
            str,
            AF_INET,
            NULL,
            reinterpret_cast<LPSOCKADDR>(&s),
            &s_size);

    if (result == SOCKET_ERROR)
        throw sockets::MethodError(__func__, "WSAStringToAddressA");

    return s;
}

sockaddr_in6 ipv6strtoaddr(char* str)
{
    sockaddr_in6 s{};
    s.sin6_family = AF_INET6;
    int s_size = sizeof(s);

    int result = WSAStringToAddressA(
            str,
            AF_INET6,
            NULL,
            reinterpret_cast<LPSOCKADDR>(&s),
            &s_size);

    if(result == SOCKET_ERROR)
        throw sockets::MethodError("IpAddress::IpAddress", "WSAStringToAddressA");

    return s;
}

std::array<unsigned char, 4> to_array(sockaddr_in addr)
{
    return std::array<unsigned char, 4> { addr.sin_addr.S_un.S_un_b.s_b1,
                                          addr.sin_addr.S_un.S_un_b.s_b2,
                                          addr.sin_addr.S_un.S_un_b.s_b3,
                                          addr.sin_addr.S_un.S_un_b.s_b4 };
}

std::array<unsigned char, 16> to_array(sockaddr_in6 addr)
{
    std::array<unsigned char, 16> rv{};
    std::copy(addr.sin6_addr.u.Byte, addr.sin6_addr.u.Byte + 16, rv.begin());
    return rv;
}

namespace sockets {
    namespace abl {
        IpAddress::IpAddress(std::unique_ptr<addr_t> &&addr_ptr, ip_family family) : family(family),
                                                                                     addr_ptr(std::move(addr_ptr))
        {}

        IpAddress::IpAddress(IpAddress &&other) noexcept : family(other.family), addr_ptr(std::move(other.addr_ptr))
        {
            other.family = ip_family::ANY;
        }

        IpAddress::IpAddress(ip_family family, const std::string& address, uint16_t port) : family(family)
        {
            if(family != ip_family::INET && family != ip_family::INET6)
                throw std::invalid_argument("Invalid Argument for family: not INET nor INET6");

            // Copy a string into a unique pointer to a character array to use with WSAStringToAddress
            // and to manage memory for us.
            std::unique_ptr<char[]> string_cpy = std::make_unique<char[]>(address.size());
            std::copy(address.cbegin(), address.cend(), string_cpy.get());

            if(family == ip_family::INET)
            {
                sockaddr_in s = ipv4strtoaddr(string_cpy.get());

                this->addr_ptr = std::make_unique<addr_t>();
                this->addr_ptr->v4addr = ipv4_addr{port, to_array(s)};
            }

            if(family == ip_family::INET6)
            {
                sockaddr_in6 s = ipv6strtoaddr(string_cpy.get());

                // Create an addr_t pointer holding an ipv6_addr struct
                this->addr_ptr = std::unique_ptr<addr_t>();
                this->addr_ptr->v6addr = ipv6_addr{
                        port,
                        s.sin6_flowinfo,
                        to_array(s),
                        s.sin6_scope_id
                };
            }
        }

        IpAddress& IpAddress::operator=(sockets::abl::IpAddress&& other) noexcept
        {
            if(this != &other)
            {
                this->addr_ptr = std::move(other.addr_ptr);
                this->family = other.family;

                other.family = ip_family::ANY;
            }
            return *this;
        }
    }
}