//
// Created by avris on 10/12/2018.
//

#include <abl/system.h>
#include <in6addr.h>
#include <sockets/Error.h>
#include <inaddr.h>

std::vector<char> c_str_copy(const std::string& str)
{
    std::vector<char> copy(str.size());
    std::copy(str.cbegin(), str.cend(), copy.begin());
    // Add a null-terminator because std::string doesn't contain one
    copy.push_back('\0');
    return copy;
}

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

        sockaddr_in system::from_ipv4(const sockets::abl::ipv4_addr &address)
        {
            sockaddr_in rv{AF_INET, address.port, {}, {}};
            rv.sin_addr.s_addr = (address.address[0]) |
                                 (address.address[1] << 8) |
                                 (address.address[2] << 16) |
                                 (address.address[3] << 24);
            return rv;
        }

        sockaddr_in6 system::from_ipv6(const sockets::abl::ipv6_addr &address)
        {
            sockaddr_in6 rv{AF_INET6, address.port, address.flowinfo, {}, {address.scope_id}};
            std::copy(address.address.cbegin(), address.address.cend(), rv.sin6_addr.u.Byte);
            return rv;
        }

        ipv4_addr system::to_ipv4(const sockaddr_in &addr)
        {
            auto l_addr = addr.sin_addr.S_un.S_un_b;

            return ipv4_addr{addr.sin_port, {
                static_cast<unsigned char>(l_addr.s_b1),
                static_cast<unsigned char>(l_addr.s_b2),
                static_cast<unsigned char>(l_addr.s_b3),
                static_cast<unsigned char>(l_addr.s_b4)
            }};
        }

        ipv6_addr system::to_ipv6(const sockaddr_in6 &addr)
        {
            ipv6_addr rv{addr.sin6_port, addr.sin6_flowinfo, {}, addr.sin6_scope_id};
            std::copy(addr.sin6_addr.u.Byte, addr.sin6_addr.u.Byte + 16, rv.address.begin());
            return rv;
        }

        IpAddress system::to_ipaddress(const sockaddr *addr)
        {
            if(addr->sa_family == AF_INET)
            {
                return IpAddress(to_ipv4(*reinterpret_cast<const sockaddr_in *>(addr)));
            }

            if(addr->sa_family == AF_INET6)
            {
                return IpAddress(to_ipv6(*reinterpret_cast<const sockaddr_in6 *>(addr)));
            }

            throw std::invalid_argument("addr->sa_family is not set to AF_INET or AF_INET6");
        }

        sockaddr_in system::from_ipv4_str(const std::string &str, uint16_t port)
        {
            // Copy the string into a vector because std::string doesn't provide a non-const pointer
            auto str_copy = c_str_copy(str);

            sockaddr_in s{};
            s.sin_family = AF_INET;
            int s_size = sizeof(s);

            int result = WSAStringToAddressA(
                    str_copy.data(),
                    AF_INET,
                    NULL,
                    reinterpret_cast<LPSOCKADDR>(&s),
                    &s_size);

            if (result == SOCKET_ERROR)
                throw sockets::MethodError(__func__, "WSAStringToAddressA");

            s.sin_port = htons(port);

            return s;
        }

        sockaddr_in6 system::from_ipv6_str(const std::string &str, uint16_t port)
        {
            auto str_copy = c_str_copy(str);

            sockaddr_in6 s{};
            s.sin6_family = AF_INET6;
            int s_size = sizeof(s);

            int result = WSAStringToAddressA(
                    str_copy.data(),
                    AF_INET6,
                    NULL,
                    reinterpret_cast<LPSOCKADDR>(&s),
                    &s_size);

            if(result == SOCKET_ERROR)
                throw sockets::MethodError(__func__, "WSAStringToAddressA");

            s.sin6_port = htons(port);

            return s;
        }

        std::string system::to_string(const sockaddr_in &ipv4_addr)
        {
            sockaddr_in addr_copy(ipv4_addr);

            DWORD str_size = 20;
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

            size_t str_size = 50;
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