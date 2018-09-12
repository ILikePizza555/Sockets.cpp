#pragma once

#include <algorithm>
#include <Connection.h>
#include <SocketException.h>


namespace sockets {
    namespace
    {
        void
        check_connection_state(const std::string &function_name, sockets::Socket &socket, bool closed)
        {
            if (closed) throw sockets::ClosedError("Connection_c", function_name);
            if (socket.socket == sockets::invalid_socket)
                throw sockets::SocketError("Connection_c", function_name, "invalid socket", EBADF);
        }

        bool
        check_delimiter(const Buffer<byte>& buffer, size_t last_index, const ByteString &delim)
        {
            if (last_index < delim.size()) return false;

            size_t buffer_index = last_index - delim.size();
            for (size_t j = 0; j < delim.size(); ++j, ++buffer_index)
            {
                if (buffer[buffer_index] != delim.at(j)) return false;
            }
            return true;
        }
    }
}
