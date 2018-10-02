//
// Tests that the library is able to create a connection to an ip address
//

#include <iostream>
#include <Connection.h>

int main()
{
    sockets::setup();

    sockets::TCPConnection connection;
    std::string request = "HEAD / HTTP/1.1\r\n\r\n";

    try
    {
        connection = sockets::connect_to("google.com", "80");
        std::cout << "Connection established." << std::endl;
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
        connection.write(request.begin(), request.end());
        std::cout << "Request sent." << std::endl;
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