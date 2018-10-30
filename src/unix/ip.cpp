//
// Created by avris on 9/27/2018.
//

#include <abl/ip.h>
#include <cstring>
#include <netdb.h>
#include <Error.h>
#include <abl/system.h>
#include <arpa/inet.h>

namespace sockets {
    namespace abl
    {
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

        IpAddress::IpAddress(sockets::abl::ip_family family, const std::string &address, uint16_t port)
        {
            if(family != ip_family::INET && family != ip_family::INET6)
                throw std::invalid_argument("Invalid Argument for family: not INET nor INET6");

            if (family == ip_family::INET) {
                this->addr.family = ip_family::INET;
                this->addr.v4addr = system::to_ipv4(system::from_ipv4_str(address, port));
            }

            if (family == ip_family::INET6) {
                this->addr.family = ip_family::INET6;
                this->addr.v6addr = system::to_ipv6(system::from_ipv6_str(address, port));
            }
        }

        addr_t &IpAddress::get_addr()
        {
            return this->addr;
        }

        const addr_t &IpAddress::get_addr() const
        {
            return this->addr;
        }

        ipv4_addr &IpAddress::get_as_ipv4()
        {
            if (addr.family != INET)
                throw InvalidStateError("IpAddress", __func__, "Contained address is not ipv4");
            return this->addr.v4addr;
        }

        const ipv4_addr &IpAddress::get_as_ipv4() const
        {
            if (addr.family != INET)
                throw InvalidStateError("IpAddress", __func__, "Contained address is not ipv4");
            return this->addr.v4addr;
        }

        ipv6_addr &IpAddress::get_as_ipv6()
        {
            if (addr.family != INET6)
                throw InvalidStateError("IpAddress", __func__, "Contained address is not ipv6");
            return this->addr.v6addr;
        }

        const ipv6_addr &IpAddress::get_as_ipv6() const
        {
            if (addr.family != INET6)
                throw InvalidStateError("IpAddress", __func__, "Contained address is not ipv6");
            return this->addr.v6addr;
        }

        bool IpAddress::is_ipv4() const
        {
            return addr.family == INET;
        }

        bool IpAddress::is_ipv6() const
        {
            return addr.family == INET6;
        }

        ip_family IpAddress::get_family() const
        {
            return addr.family;
        }

        bool IpAddress::is_loopback() const
        {
            if (this->addr.family == ip_family::INET) {
                return addr.v4addr.address[0] == 127;
            }

            if (this->addr.family == ip_family::INET6) {
                auto &addr = this->addr.v6addr.address;

                return addr[0] == 0 && addr[1] == 0 && addr[2] == 0 && addr[4] == 0 &&
                       addr[5] == 0 && addr[6] == 0 && addr[7] == 0 && addr[8] == 0 &&
                       addr[9] == 0 && addr[10] == 0 && addr[11] == 0 && addr[12] == 0 &&
                       addr[13] == 0 && addr[14] == 0 && addr[15] == 1;
            }

            return false;
        }

        std::string IpAddress::name() const
        {
            const static socklen_t buf_size = 50;
            std::unique_ptr<char[]> buf = std::make_unique<char[]>(buf_size);

            const char *result = inet_ntop(system::iftosys(addr.family),
                                           addr.family == INET ? addr.v4addr.address.data()
                                                               : addr.v6addr.address.data(),
                                           buf.get(),
                                           buf_size);
            if (result == nullptr)
                throw MethodError("IpAddress::name", "inet_ntop");

            return std::string(buf.release());
        }

        uint16_t IpAddress::port() const
        {
            return this->addr.v4addr.port;
        }

        void AddrInfoFlags::set_all()
        { flags = AI_ALL; }

        AddrInfoFlags &AddrInfoFlags::set_ipv4_mapping()
        {
            flags |= AI_V4MAPPED;
            return *this;
        }

        AddrInfoFlags &AddrInfoFlags::set_passive()
        {
            flags |= AI_PASSIVE;
            return *this;
        }

        AddrInfoFlags &AddrInfoFlags::set_addr_config()
        {
            flags |= AI_ADDRCONFIG;
            return *this;
        }

        AddrInfoFlags &AddrInfoFlags::set_numeric_host()
        {
            flags |= AI_NUMERICHOST;
            return *this;
        }

        AddrInfoFlags &AddrInfoFlags::set_numeric_serv()
        {
            flags |= AI_NUMERICSERV;
            return *this;
        }

        int AddrInfoFlags::get()
        {
            return flags;
        }

        std::vector<address_info>
        get_address_info(const std::string &host, const std::string &port, AddrInfoFlags &flags,
                         ip_family hint_family, sock_type hint_type, sock_proto hint_proto)
        {
            std::vector<address_info> rv;

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

            auto *addrinfo_ptr = new addrinfo;
            auto result = getaddrinfo(host.c_str(), port.c_str(), &hints, &addrinfo_ptr);
            if (result != 0) {
                delete addrinfo_ptr;
                throw MethodError(__func__, "getaddrinfo", result,
                                  [](int ec) { return std::string(gai_strerror(ec)); });
            }

            // Vector construction loop
            auto next = addrinfo_ptr;
            while (next != NULL) {
                // Copy sockaddr to avoid deletion
                auto *sockaddr_copy = new sockaddr_storage;
                std::memcpy(sockaddr_copy, next->ai_addr, next->ai_addrlen);

                address_info ai{
                        system::systoif(next->ai_family),
                        system::systost(next->ai_socktype),
                        system::systosp(next->ai_protocol),
                        system::to_ipaddress(next->ai_addr)
                };
                rv.push_back(ai);

                next = next->ai_next;
            }

            freeaddrinfo(addrinfo_ptr);
            return rv;
        }
    }
}