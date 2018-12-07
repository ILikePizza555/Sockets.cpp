#include <sockets/TCPServerSocket.h>
#include <sockets/Error.h>
#include <tuple>
#include <iostream>

using sockets::TCPServerSocket;
using sockets::TCPConnection;

using std::cout;
using std::endl;

int main(int argc, char** argv)
{
    TCPServerSocket server("127.0.0.1", "22");
    cout << "Listening on port 22..." << endl;

    TCPConnection conn;
    sockets::abl::IpAddress ip;
    std::tie(conn, ip) = server.acceptfrom();

    cout << "New connection from: " << ip.name() << " on port " << ip.port() << "." << endl;

    while (!conn.closed())
    {
        try {
            // Check connection state
            conn.write<1>({1});

            auto b = conn.read(128);
            cout << "[" << ip.name() << ":" << ip.port() << "] " << b << endl;
        } catch (std::exception& e)
        {
            std::cerr << "Error: " << e.what() << endl;
        }
    }

    return 0;
}