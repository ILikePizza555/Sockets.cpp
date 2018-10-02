//
// Created by avris on 9/27/2018.
//

#include <Connection.h>
#include <netdb.h>

namespace sockets
{

    TCPConnection connect_to(std::string host, std::string port)
    {
        auto addresses = get_address_info(host,
                port,
                (AI_V4MAPPED | AI_ADDRCONFIG),
                ip_family::ANY,
                sock_type::STREAM,
                sock_proto::TCP);


        TCPSocket s(addresses[0].family);
        s.connect(addresses[0].address);

        return TCPConnection(s);
    }
}