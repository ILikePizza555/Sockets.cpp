//
// Created by avris on 9/27/2018.
//

#include <abl/ip.h>
#include <Connection.h>

namespace sockets
{

    TCPConnection connect_to(std::string host, std::string port)
    {
        AddrInfoFlags flags = AddrInfoFlags();
        flags.set_ipv4_mapping().set_addr_config();

        auto addresses = get_address_info(host,
                port,
                flags,
                ip_family::ANY,
                sock_type::STREAM,
                sock_proto::TCP);


        TCPSocket s(addresses[0].family);
        s.connect(addresses[0].address);

        return TCPConnection(s);
    }
}