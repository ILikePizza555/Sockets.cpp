//
// Tests that the library is able to create a connection to an ip address
//

#include <iostream>
#include <Connection.h>

void print_addr(sockets::addr_t& addr)
{
    using std::cout;
    using std::endl;
    cout << "addr_t(ip=" << addr.name() <<"; port=" << addr.port() <<"; localhost=" << addr.is_loopback() << ")" << endl;
}

int main()
{
    sockets::setup();

    sockets::TCPConnection connection;
    std::string request = "GET / HTTP/1.1\r\n\r\n";

    try
    {
        connection = sockets::connect_to("google.com", "80");
        auto local_addr = connection.get_socket().getsockname();
        auto remote_addr = connection.get_socket().getpeername();

        std::cout << "Connection established." << std::endl;
        std::cout << "Local: ";
        print_addr(local_addr);
        std::cout << "Remote: ";
        print_addr(remote_addr);
    }
    catch (std::exception& e)
    {
        std::cerr << "Error attempting to connect." << std::endl;
        std::cerr << e.what() << std::endl;

        sockets::teardown();
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

        sockets::teardown();
        return 1;
    }

    try
    {
        auto response = connection.read(50);
        auto expected_response = std::string("HTTP/1.1 200 OK\r\n");

        std::cout << "Received response: " << response << std::endl;

        auto compare = std::equal(response.begin(),
                                  response.begin() + expected_response.size(),
                                  expected_response.begin(),
                                  expected_response.end());
        sockets::teardown();
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

        sockets::teardown();
        return 1;
    }
}