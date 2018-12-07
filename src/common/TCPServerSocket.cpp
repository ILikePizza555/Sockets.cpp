//
// Created by avris on 12/6/2018.
//

#include <sockets/TCPServerSocket.h>
#include <sockets/Error.h>

namespace sockets {
    TCPServerSocket::TCPServerSocket(const abl::IpAddress &addr, int backlog) : _serverSocket(addr.get_family())
    {
        _serverSocket.bind(addr);
        _serverSocket.listen(backlog);
    }

    TCPServerSocket::TCPServerSocket(const std::string &ip, const std::string& port, int backlog)
    {
        abl::AddrInfoFlags flags = abl::AddrInfoFlags();
        flags.set_ipv4_mapping().set_passive();

        auto addresses = abl::get_address_info(
                ip,
                port,
                flags,
                abl::ip_family::ANY,
                abl::sock_type::STREAM,
                abl::sock_proto::TCP);

        if (addresses.empty())
            throw InvalidStateError("TCPServerSocket", __func__, "get_address_info returned no addresses for " + ip + "!");

        _serverSocket = TCPSocket(addresses[0].family);
        _serverSocket.bind(addresses[0].address);
        _serverSocket.listen(backlog);
    }

    TCPConnection TCPServerSocket::accept() const
    {
        return TCPConnection(_serverSocket.accept());
    }

    std::tuple<TCPConnection, abl::IpAddress> TCPServerSocket::acceptfrom() const
    {
        TCPSocket peer;
        abl::IpAddress addr;
        std::tie(peer, addr) = _serverSocket.acceptfrom();

        return std::make_tuple(TCPConnection(std::move(peer)), addr);
    }
}