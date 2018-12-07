#include <sockets/TCPServerSocket.h>
#include <sockets/Error.h>
#include <sockets/Byte.h>
#include <sockets/abl/win32.h>
#include <tuple>
#include <iostream>

using sockets::TCPServerSocket;
using sockets::TCPConnection;

using std::cout;
using std::endl;

int main(int argc, char** argv)
{
#ifdef _WIN32
    sockets::abl::win32::WinSockDLL dll;
    std::cout << "Initalized WinSock dll." << std::endl;
#endif

    TCPServerSocket server("127.0.0.1", "1022");
    cout << "Listening on port 1022..." << endl;

    TCPConnection conn;
    sockets::abl::IpAddress ip;
    std::tie(conn, ip) = server.acceptfrom();

    cout << "New connection from: " << ip.name() << " on port " << ip.port() << "." << endl;

    while (!conn.closed())
    {
        try {
            // Check connection state
            conn.write<1>(ByteString<1>{{1}});

            auto b = conn.read(128);
            cout << "[" << ip.name() << ":" << ip.port() << "] " << b << endl;
        } catch (std::exception& e)
        {
            std::cerr << "Error: " << e.what() << endl;
        }
    }

    return 0;
}