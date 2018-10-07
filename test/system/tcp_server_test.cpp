//
// Created by avris on 10/4/2018.
//

#include <ServerSocket.h>
#include <Connection.h>
#include <Error.h>
#include <thread>
#include <mutex>
#include <iostream>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
void server_thread_func()
{
    sockets::TCPServer server("1111");

    while(true)
    {
        auto client = server.accept();
        std::cout << "Server: Client " << client.get_name() << " connected." << std::endl;

        while(true)
        {
            try
            {
                auto buffer = client.read(1024);
                std::cout << "Server: Client sent " << buffer.size() << " bytes." << std::endl;

                auto amount = client.write(buffer.rbegin(), buffer.rend());
                std::cout << "Server: Wrote " << amount << " bytes back." << std::endl;
            }
            catch (sockets::SocketReadError &e)
            {
                std::cout << "Server: Error: " << e.what() << std::endl;
                break;
            }
            catch (sockets::SocketWriteError &e)
            {
                if (e.type == e.CONNECTION_RESET)
                    std::cout << "Server: Connection reset by client." << std::endl;
                else
                    std::cout << "Server: Error: " << e.what() << std::endl;

                break;
            }
        }

        std::cout << "Connection closed." << std::endl;
    }
}
#pragma clang diagnostic pop

int main()
{
    std::thread server_thread(server_thread_func);

    auto connection = sockets::connect_to("localhost", "1111");
    while(true)
    {
        std::string line;
        std::getline(std::cin, line);

        try
        {
            auto write_amt = connection.write(line.begin(), line.end());

            auto buffer = connection.read(write_amt);
            std::string server_line(buffer.begin(), buffer.end());
            std::cout << "Client: Received " << server_line << std::endl;
        }
        catch (sockets::MethodError& e)
        {
            std::cout << "Client: " << e.what() << std::endl;
            return 1;
        }
    }
}