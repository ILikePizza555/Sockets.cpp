//
// Created by avris on 10/26/2018.
//

#include <abl/system.h>

namespace sockets {
    namespace abl {
        int system::iftosys(ip_family family)
        {
            switch (family)
            {
                case ANY: return AF_UNSPEC;
                case INET: return AF_INET;
                case INET6: return AF_INET6;
            }

            return -1;
        }

        ip_family system::systoif(int family)
        {
            switch (family) {
                case AF_INET:
                    return ip_family::INET;
                case AF_INET6:
                    return ip_family::INET6;
                default:
                    return ip_family::ANY;
            }
        }

        int system::sttosys(sockets::abl::sock_type type)
        {
            switch (type) {
                case sock_type::STREAM:
                    return SOCK_STREAM;
                case sock_type::DATAGRAM:
                    return SOCK_DGRAM;
                case sock_type::RAW:
                    return SOCK_RAW;
            }
            return -1;
        }

        sock_type system::systost(int type)
        {
            switch (type) {
                case SOCK_STREAM:
                    return sock_type::STREAM;
                case SOCK_DGRAM:
                    return sock_type::DATAGRAM;
                default:
                    return sock_type::RAW;
            }
        }

        int system::sptosys(sockets::abl::sock_proto proto)
        {
            switch (proto) {
                case sock_proto::TCP:
                    return IPPROTO_TCP;
                case sock_proto::UDP:
                    return IPPROTO_UDP;
            }
            return -1;
        }

        sock_proto system::systosp(int proto)
        {
            switch (proto) {
                case IPPROTO_TCP:
                    return sock_proto::TCP;
                case IPPROTO_UDP:
                    return sock_proto::UDP;
                default:
                    throw std::invalid_argument("proto");
            }
        }

        sockaddr_in system::from_ipv4(const ipv4_addr &address)
        {
            sockaddr_in rv{AF_INET, address.port, {}, {}};
            rv.sin_addr.s_addr = address.address[0] |
                                 (address.address[1] << 8) |
                                 (address.address[2] << 16) |
                                 (address.address[3] << 24);
            return rv;
        }

        sockaddr_in6 system::from_ipv6(const ipv6_addr &address)
        {
            sockaddr_in6 rv{AF_INET6, address.port, address.flowinfo, {}, address.scope_id};
            std::copy(address.address.cbegin(), address.address.cend(), rv.sin6_addr.s6_addr);
            return rv;
        }

        ipv4_addr system::to_ipv4(const sockaddr_in &addr)
        {
            ipv4_addr rv{addr.sin_port, {}};
            rv.address[0] = static_cast<unsigned char>(addr.sin_addr.s_addr & 0xFF);
            rv.address[1] = static_cast<unsigned char>(addr.sin_addr.s_addr >> 8 & 0xFF);
            rv.address[2] = static_cast<unsigned char>(addr.sin_addr.s_addr >> 16 & 0xFF);
            rv.address[3] = static_cast<unsigned char>(addr.sin_addr.s_addr >> 24 & 0xFF);
            return rv;
        }
    }
}