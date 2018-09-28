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
}
