//
// Created by avris on 10/11/2018.
//

#include <algorithm>
#include <abl/ip.h>
#include <abl/system.h>
#include <Error.h>
#include <inaddr.h>
#include <in6addr.h>
#include <ws2tcpip.h>


namespace sockets {
    namespace abl {

        IpAddress::IpAddress(sockets::abl::ipv4_addr addr)
        {
            this->addr.family = ip_family::INET;
            this->addr.v4addr = addr;
        }

        IpAddress::IpAddress(sockets::abl::ipv6_addr addr)
        {
            this->addr.family = ip_family::INET6;
            this->addr.v6addr = addr;
        }

        IpAddress::IpAddress(ip_family family, const std::string& address, uint16_t port)
        {
            if(family != ip_family::INET && family != ip_family::INET6)
                throw std::invalid_argument("Invalid Argument for family: not INET nor INET6");

            if(family == ip_family::INET)
            {
                addr.family = family;
                addr.v4addr = system::to_ipv4(system::from_ipv4_str(address, port));
            }

            if(family == ip_family::INET6)
            {
                addr.family = family;
                addr.v6addr = system::to_ipv6(system::from_ipv6_str(address, port));
            }
        }

        const addr_t&
        IpAddress::get_addr() const
        {
            return this->addr;
        }

        addr_t&
        IpAddress::get_addr()
        {
            return this->addr;
        }

        ip_family IpAddress::get_family() const
        {
            return this->addr.family;
        }

        bool IpAddress::is_loopback() const
        {
            if(this->addr.family == ip_family::INET)
            {
                return this->addr.v4addr.address[0] == 127;
            }

            if(this->addr.family == ip_family::INET6)
            {
                return std::all_of(this->addr.v6addr.address.begin(),
                                   this->addr.v6addr.address.end() - 1,
                                   [](unsigned char b){return b == 0;}) &&
                       this->addr.v6addr.address[15] == 1;
            }

            return false;
        }

        std::string IpAddress::name() const
        {
            if (this->addr.family == ip_family::INET)
                return system::to_string(system::from_ipv4(this->addr.v4addr));

            if (this->addr.family == ip_family::INET6)
                return system::to_string(system::from_ipv6(this->addr.v6addr));

            throw InvalidStateError("IpAddress", __func__, "family not set to INET or INET6.");
        }

        uint16_t IpAddress::port() const
        {
            // Because this is the same for both structs in the union, we don't need to check for the correct struct.
            return this->addr.v4addr.port;
        }

        void AddrInfoFlags::set_all()
        {
            this->flags |= 0x0100;
        }

        AddrInfoFlags& AddrInfoFlags::set_ipv4_mapping()
        {
            this->flags |= 0x0800;
            return *this;
        }

        AddrInfoFlags& AddrInfoFlags::set_passive()
        {
            this->flags |= 0x01;
            return *this;
        }

        AddrInfoFlags& AddrInfoFlags::set_addr_config()
        {
            this->flags |= 0x0400;
            return *this;
        }

        AddrInfoFlags& AddrInfoFlags::set_numeric_host()
        {
            this->flags |= 0x04;
            return *this;
        }

        AddrInfoFlags& AddrInfoFlags::set_numeric_serv()
        {
            return *this;
        }

        int AddrInfoFlags::get()
        {
            return this->flags;
        }

        std::vector<address_info>
        get_address_info(const std::string &host, const std::string &port, AddrInfoFlags &flags,
                         ip_family hint_family, sock_type hint_type, sock_proto hint_proto)
        {
            addrinfo hints{
                flags.get(),
                system::iftosys(hint_family),
                system::sttosys(hint_type),
                system::sptosys(hint_proto),
                0,
                nullptr,
                nullptr,
                nullptr
            };

            addrinfo* addr_result = nullptr;

            int result_code = getaddrinfo(host.data(), port.data(), &hints, &addr_result);
            if(result_code != 0)
            {
                freeaddrinfo(addr_result);
                throw MethodError(__func__, "getaddrinfo");
            }

            std::vector<address_info> rv;
            addrinfo* next = addr_result;
            while(next != nullptr)
            {
                address_info ai{
                    system::systoif(next->ai_family),
                    system::systost(next->ai_socktype),
                    system::systosp(next->ai_protocol),
                    system::to_ipaddress(next->ai_addr)
                };
                rv.push_back(ai);

                next = next->ai_next;
            }

            freeaddrinfo(addr_result);
            return rv;
        }
    }
}