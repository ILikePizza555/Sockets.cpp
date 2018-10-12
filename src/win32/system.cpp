//
// Created by avris on 10/12/2018.
//

#include <abl/system.h>
#include <in6addr.h>
#include <Error.h>
#include "../../../../AppData/Local/Packages/CanonicalGroupLimited.UbuntuonWindows_79rhkp1fndgsc/LocalState/rootfs/usr/include/netinet/in.h"

namespace sockets {
    namespace abl {
        int system::iftosys(sockets::abl::ip_family family)
        {
            switch (family) {
                case ip_family::INET:
                    return AF_INET;
                case ip_family::INET6:
                    return AF_INET6;
                case ip_family::ANY:
                    return AF_UNSPEC;
            }
            return -1;
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

        sockaddr_in system::ipv4_to_system(const sockets::abl::ipv4_addr &address)
        {
            sockaddr_in rv{AF_INET, address.port, {}, {}};
            rv.sin_addr.s_addr = (address.address[0] << 24) |
                                 (address.address[1] << 16) |
                                 (address.address[2] << 8)  |
                                 (address.address[3]);
            return rv;
        }

        sockaddr_in6 system::ipv6_to_system(const sockets::abl::ipv6_addr &address)
        {
            sockaddr_in6 rv{AF_INET6, address.port, address.flowinfo, {}, {address.scope_id}};
            std::copy(address.address.cbegin(), address.address.cend(), rv.sin6_addr.u.Byte);
            return rv;
        }

        ipv4_addr system::system_to_ipv4(const sockaddr_in &addr)
        {
            return ipv4_addr{addr.sin_port, {
                static_cast<unsigned char>((addr.sin_addr.s_addr >> 24) & 0xFF),
                static_cast<unsigned char>((addr.sin_addr.s_addr >> 16) & 0xFF),
                static_cast<unsigned char>((addr.sin_addr.s_addr >> 8) & 0xFF),
                static_cast<unsigned char>(addr.sin_addr.s_addr & 0xFF)
            }};
        }

        ipv6_addr system::system_to_ipv6(const sockaddr_in6 &addr)
        {
            ipv6_addr rv{addr.sin6_port, addr.sin6_flowinfo, {}, addr.sin6_scope_id};
            std::copy(addr.sin6_addr.u.Byte, addr.sin6_addr.u.Byte + 16, rv.address.begin());
            return rv;
        }

        sockaddr_in system::ipv4str_to_addr(const std::string &str)
        {
            std::unique_ptr<char[]> str_copy = std::make_unique<char[]>(str.size());
            std::copy(str.cbegin(), str.cend(), str_copy.get());

            sockaddr_in s{};
            s.sin_family = AF_INET;
            int s_size = sizeof(s);

            int result = WSAStringToAddressA(
                    str_copy.get(),
                    AF_INET,
                    NULL,
                    reinterpret_cast<LPSOCKADDR>(&s),
                    &s_size);

            if (result == SOCKET_ERROR)
                throw sockets::MethodError(__func__, "WSAStringToAddressA");

            return s;
        }

        sockaddr_in6 system::ipv6str_to_addr(const std::string &str)
        {
            std::unique_ptr<char[]> str_copy = std::make_unique<char[]>(str.size());
            std::copy(str.cbegin(), str.cend(), str_copy.get());

            sockaddr_in6 s{};
            s.sin6_family = AF_INET6;
            int s_size = sizeof(s);

            int result = WSAStringToAddressA(
                    str_copy.get(),
                    AF_INET6,
                    NULL,
                    reinterpret_cast<LPSOCKADDR>(&s),
                    &s_size);

            if(result == SOCKET_ERROR)
                throw sockets::MethodError(__func__, "WSAStringToAddressA");

            return s;
        }

        std::string system::to_string(const sockaddr_in &ipv4_addr)
        {
            sockaddr_in addr_copy(ipv4_addr);

            size_t str_size = 16;
            std::unique_ptr<char[]> str_ptr = std::make_unique<char[]>(str_size);

            int result = WSAAddressToStringA(
                    reinterpret_cast<LPSOCKADDR>(&addr_copy),
                    sizeof(sockaddr_in),
                    NULL,
                    str_ptr.get(),
                    reinterpret_cast<LPDWORD>(&str_size));

            if (result == SOCKET_ERROR)
                throw sockets::MethodError(__func__, "WSAAddressToStringA");

            return std::string(str_ptr.release(), str_size);
        }

        std::string system::to_string(const sockaddr_in6 &ipv6_addr)
        {
            sockaddr_in6 addr_copy(ipv6_addr);

            size_t str_size = 46;
            std::unique_ptr<char[]> str_ptr = std::make_unique<char[]>(str_size);

            int result = WSAAddressToStringA(
                    reinterpret_cast<LPSOCKADDR>(&addr_copy),
                    sizeof(sockaddr_in),
                    NULL,
                    str_ptr.get(),
                    reinterpret_cast<LPDWORD>(&str_size));

            if (result == SOCKET_ERROR)
                throw sockets::MethodError(__func__, "WSAAddressToStringA");

            return std::string(str_ptr.release(), str_size);
        }
    }
}