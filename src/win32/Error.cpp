//
// Created by avris on 9/5/2018.
//

#include <Error.h>
#include <abl/system.h>

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

sockets::SocketReadError::ErrorType sockets::SocketReadError::map_error_type(int code)
{
    switch (code)
    {
        case WSANOTINITIALISED:
            return ErrorType::NOT_INITIALIZED;
        case WSAEWOULDBLOCK:
            return ErrorType::WOULD_BLOCK;
        case WSAENOTSOCK:
            return ErrorType::NOT_SOCKET;
        case WSAETIMEDOUT:
            return ErrorType::TIMED_OUT;
        case WSAENOTCONN:
            return ErrorType::NOT_CONNECTED;
        default:
            return ErrorType::OTHER;
    }
}

sockets::SocketWriteError::ErrorType sockets::SocketWriteError::map_error_type(int code)
{
    switch (code)
    {
        case WSANOTINITIALISED:
            return ErrorType::NOT_INITIALIZED;
        case WSAETIMEDOUT:
            return ErrorType::TIMED_OUT;
        case WSAEWOULDBLOCK:
            return ErrorType::WOULD_BLOCK;
        case WSAENOTSOCK:
            return ErrorType::NOT_SOCKET;
        case WSAENOTCONN:
            return ErrorType::NOT_CONNECTED;
        case WSAECONNRESET:
            return ErrorType::CONNECTION_RESET;
        default:
            return ErrorType::OTHER;
    }
}