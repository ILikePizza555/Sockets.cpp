//
// Created by avris on 10/2/2018.
//

#include <abl/abl.h>

void sockets::setup()
{
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
}

void sockets::teardown()
{
    WSACleanup();
}