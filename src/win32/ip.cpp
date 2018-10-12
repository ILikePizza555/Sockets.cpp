//
// Created by avris on 10/11/2018.
//

#include <algorithm>
#include <abl/ip.h>
#include <abl/system.h>
#include <Error.h>
#include <inaddr.h>
#include <in6addr.h>
#include <Ws2tcpip.h>


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

            if(family == ip_family::INET)
            {
                this->addr_ptr = std::make_unique<addr_t>(system::system_to_ipv4(system::ipv4str_to_addr(address, port)));
            }

            if(family == ip_family::INET6)
            {
                this->addr_ptr = std::make_unique<addr_t>(system::system_to_ipv6(system::ipv6str_to_addr(address, port)));
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

        const std::unique_ptr<addr_t>& IpAddress::addr() const
        {
            return this->addr_ptr;
        }

        std::unique_ptr<addr_t>& IpAddress::addr()
        {
            return this->addr_ptr;
        }

        ip_family IpAddress::get_family() const
        {
            return this->family;
        }

        bool IpAddress::is_loopback() const
        {
            if(this->family == ip_family::INET)
            {
                return addr_ptr->v4addr.address[0] == 127;
            }

            if(this->family == ip_family::INET6)
            {
                return std::all_of(addr_ptr->v6addr.address.begin(),
                                   addr_ptr->v6addr.address.end() - 1,
                                   [](unsigned char b){return b == 0;}) &&
                       addr_ptr->v6addr.address[15] == 1;
            }

            return false;
        }

        std::string IpAddress::name() const
        {
            if (this->family == ip_family::INET)
                return system::to_string(system::ipv4_to_system(this->addr_ptr->v4addr));

            if (this->family == ip_family::INET6)
                return system::to_string(system::ipv6_to_system(this->addr_ptr->v6addr));

            throw InvalidStateError("IpAddress", __func__, "family not set to INET or INET6.");
        }

        uint16_t IpAddress::port() const
        {
            // Because this is the same for both structs in the union, we don't need to check for the correct struct.
            return this->addr_ptr->v4addr.port;
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
    }
}