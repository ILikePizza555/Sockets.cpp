//
// Created by avris on 10/11/2018.
//

#include <abl/ip.h>
#include <abl/system.h>
#include <Error.h>
#include <inaddr.h>
#include <in6addr.h>

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
                sockaddr_in s{};
                s.sin_family = static_cast<short>(iftosys(family));
                int s_size = sizeof(s);

                int result = WSAStringToAddressA(
                        string_cpy.get(),
                        iftosys(family),
                        NULL,
                        reinterpret_cast<LPSOCKADDR>(&s),
                        &s_size);

                if (result == SOCKET_ERROR)
                    throw MethodError("IpAddress::IpAddress", "WSAStringToAddressA");

                // Create an addr_t pointer
                auto p = new addr_t{};
                p->v4addr = ipv4_addr{port, {s.sin_addr.S_un.S_un_b.s_b1,
                                             s.sin_addr.S_un.S_un_b.s_b2,
                                             s.sin_addr.S_un.S_un_b.s_b3,
                                             s.sin_addr.S_un.S_un_b.s_b4}};
                this->addr_ptr = std::unique_ptr<addr_t>(p);
            }

            if(family == ip_family::INET6)
            {
                sockaddr_in6 s{};
                s.sin6_family = static_cast<short>(iftosys(family));
                int s_size = sizeof(s);

                int result = WSAStringToAddressA(
                        string_cpy.get(),
                        iftosys(family),
                        NULL,
                        reinterpret_cast<LPSOCKADDR>(&s),
                        &s_size);

                if(result == SOCKET_ERROR)
                    throw MethodError("IpAddress::IpAddress", "WSAStringToAddressA");

                // Create an addr_t pointer holding an ipv6_addr struct
                auto p = new addr_t{};
                p->v6addr = ipv6_addr{
                    port,
                    s.sin6_flowinfo,
                    {},
                    s.sin6_scope_id
                };

                // Copy the address from the struct to our struct
                std::copy(s.sin6_addr.u.Byte, s.sin6_addr.u.Byte + 16, p->v6addr.address.begin());

                this->addr_ptr = std::unique_ptr<addr_t>(p);
            }
        }
    }
}