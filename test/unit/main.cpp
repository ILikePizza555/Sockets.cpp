#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#ifdef _WIN32

#include <winsock2.h>

#endif

/*
 * Define our own main so that we can call WSAStartup() and WSACleanup() on windows systems.
 */
int main(int argc, char* argv[])
{
#ifdef _WIN32
    WSADATA discard{};
    WSAStartup(MAKEWORD(2, 2), &discard);
#endif

    int result = Catch::Session().run(argc, argv);

#ifdef _WIN32
    WSACleanup();
#endif

    return result;
}