#include <Error.h>
#include <errno.h>
#include <string.h>

namespace sockets {
    int get_error_code() 
    {
        return errno;
    }

    std::string get_error_message(int code)
    {
        return std::string(strerror(code));
    }

    bool check_connection_closed()
    {
        return errno == ECONNRESET;
    }

    SocketReadError::ErrorType SocketReadError::map_error_type(int code)
    {
        switch (code)
        {
            case EWOULDBLOCK:
                return ErrorType::WOULD_BLOCK;
            case EBADF:
                return ErrorType::INVALID_HANDLE;
            case ENOTSOCK:
                return ErrorType::NOT_SOCKET;
            case ETIMEDOUT:
                return ErrorType::TIMED_OUT;
            case EINTR:
                return ErrorType::INTERRUPTED;
            case ENOTCONN:
                return ErrorType::NOT_CONNECTED;
            default:
                return ErrorType::OTHER;
        }
    }

    SocketWriteError::ErrorType SocketWriteError::map_error_type(int code)
    {
        switch (code)
        {
            case ETIMEDOUT:
                return ErrorType::TIMED_OUT;
            case EWOULDBLOCK:
                return ErrorType::WOULD_BLOCK;
            case EBADF:
                return ErrorType::INVALID_HANDLE;
            case ENOTSOCK:
                return ErrorType::NOT_SOCKET;
            case ENOTCONN:
                return ErrorType::NOT_CONNECTED;
            case EINTR:
                return ErrorType::INTERRUPTED;
            case ECONNRESET:
                return ErrorType::CONNECTION_RESET;
            default:
                return ErrorType::OTHER;
        }
    }
}
