//
// Created by avris on 10/4/2018.
//

#include <abl/socket.h>
#include <Error.h>

void sockets::set_ipv6_only(sock_t socket, bool enable)
{
    BOOL optval = enable;

    auto result = setsockopt(socket, IPPROTO_IPV6, IPV6_V6ONLY, reinterpret_cast<const char*>(optval), sizeof(BOOL));
    if(result == SOCKET_ERROR) throw MethodError(__func__, "setsockopt");
}

bool sockets::get_ipv6_only(sockets::sock_t socket)
{
    BOOL value = 0;
    int value_len = sizeof(BOOL);

    auto result = getsockopt(socket, IPPROTO_IPV6, IPV6_V6ONLY, reinterpret_cast<char *>(value), &value_len);
    if(result == SOCKET_ERROR) throw MethodError(__func__, "setsockopt");

    return value != 0;
}