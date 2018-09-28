//
// Created by avris on 9/27/2018.
//

#include <abl/ip.h>

bool sockets::addr_t::is_local()
{

}

std::string sockets::addr_t::name()
{
    static unsigned long size = 40;
    char* rv = new char[size];

    int result = WSAAddressToStringA(this->addr_ptr.get(), this->length, nullptr, rv, &size);
    if(result != 0)
    {
        // TODO: Throw error
    }

    return std::string(rv);
}