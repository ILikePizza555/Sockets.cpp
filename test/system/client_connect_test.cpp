//
// Tests that the library is able to create a connection to an ip address
//

#include <iostream>
#include <sockets/Connection.h>

#ifdef _WIN32
#include <abl/win32.h>
#endif

std::ostream& operator<<(std::ostream& oss, sockets::abl::IpAddress& addr)
{
    using std::endl;
    oss << "addr_t(ip=" << addr.name() <<"; port=" << addr.port() <<"; localhost=" << addr.is_loopback() << ")" << endl;
    return oss;
}

int main()
{
#ifdef _WIN32
    sockets::abl::win32::WinSockDLL dll;
    std::cout << "Initalized WinSock dll." << std::endl;
#endif

    sockets::TCPConnection connection;
    std::string request = "HEAD / HTTP/1.1\r\n\r\n";

    try
    {
        connection = sockets::connect_to("google.com", "80");
        auto local_addr = connection.get_socket().getsockname();
        auto remote_addr = connection.get_socket().getpeername();

        std::cout << "Connection established." << std::endl;
        std::cout << "Local: " << local_addr << std::endl;
        std::cout << "Remote: " << remote_addr << std::endl;
    }
    catch (std::exception& e)
    {
        std::cerr << "Error attempting to connect." << std::endl;
        std::cerr << e.what() << std::endl;

        return 1;
    }

    try
    {
        size_t bytes = connection.write(request.begin(), request.end());
        std::cout << "Request sent. (" << bytes << " bytes)" << std::endl;
    }
    catch (std::exception& e)
    {
        std::cerr << "Error sending request." << std::endl;
        std::cerr << e.what() << std::endl;

        return 1;
    }

    try
    {
        auto response = connection.read_until<4>({{'\r', '\n', '\r', '\n'}});
        auto expected_response = std::string("HTTP/1.1 200 OK\r\n");

        std::cout << "Received response: " << response << std::endl;

        auto compare = std::equal(response.begin(),
                                  response.begin() + expected_response.size(),
                                  expected_response.begin(),
                                  expected_response.end());
        if(compare)
        {
            std::cout << "Success!" << std::endl;
            return 0;
        }
        else
        {
            std::cout << "Fail." << std::endl;
            return 1;
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Error reading response." << std::endl;
        std::cerr << e.what() << std::endl;

        return 1;
    }
}