//
// Created by avris on 10/22/2018.
//

#include <abl/win32.h>
#include <winsock2.h>
#include <sstream>

namespace sockets {
    abl::win32::WSAError::WSAError(int ec, std::string f) : error_code(ec), function(std::move(f))
    {
    }

    const char* abl::win32::WSAError::what() const noexcept
    {
        std::stringstream ss;
        ss << function << " failed with error code: " << error_code;
        return ss.str().c_str();
    }

    struct abl::win32::WinSockDLL::Imp {
        WSAData wsa_data{};
    };

    abl::win32::WinSockDLL::WinSockDLL(unsigned char hi, unsigned char low) : _imp(new Imp{})
    {
        WORD version_requested = MAKEWORD(low, hi);

        int error = WSAStartup(version_requested, &_imp->wsa_data);
        if(error != 0)
            throw WSAError(error, "WSAStartup");
    }

    abl::win32::WinSockDLL::~WinSockDLL()
    {
        WSACleanup();
    }
}