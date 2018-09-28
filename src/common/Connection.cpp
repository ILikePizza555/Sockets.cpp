#include <algorithm>
#include <Connection.h>
#include <Error.h>


namespace sockets {
    namespace
    {
        void
        check_connection_state(const std::string& function_name, sockets::Socket &socket, bool closed)
        {
            if (closed) throw sockets::ClosedError("Connection", function_name);
            if (socket.socket == sockets::invalid_socket)
                throw sockets::SocketError("Connection", function_name, "invalid socket", EBADF);
        }
    }
}
