//
// Created by avris on 9/5/2018.
//

#include <Error.h>
#include <winbase.h>

int sockets::get_error_code()
{
    return WSAGetLastError();
}

std::string sockets::get_error_message(int code)
{
    char* message = nullptr;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                  nullptr,
                  static_cast<DWORD>(code),
                  0,
                  (LPTSTR) &message,
                  0,
                  nullptr);
    return std::string(message);
}