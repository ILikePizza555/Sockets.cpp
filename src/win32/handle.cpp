//
// Created by avris on 10/4/2018.
//

#include <abl/handle.h>
#include <abl/system.h>

namespace sockets {
    namespace abl {
        struct handle_t
        {
            SOCKET socket;
        };

        void close_handle(handle_t* handle)
        {
            closesocket(handle->socket);
        }

        SOCKET get_system_handle(const handle_t* handle)
        {
            return handle->socket;
        }

        SOCKET get_system_handle(const UniqueHandle& handle)
        {
            return handle->socket;
        }

        SOCKET get_system_handle(const SharedHandle& handle)
        {
            return handle->socket;
        }

        handle_t* from_system_handle(SOCKET handle)
        {
            return new handle_t{handle};
        }

        UniqueHandle new_unique_handle(ip_family family, sock_type type, sock_proto protocol)
        {
            SOCKET s = socket(iftosys(family), sttosys(type), sptosys(protocol));
            return UniqueHandle(new handle_t{s}, &close_handle);
        }

        SharedHandle new_shared_handle(ip_family family, sock_type type, sock_proto protocol)
        {
            SOCKET s = socket(iftosys(family), sttosys(type), sptosys(protocol));
            return SharedHandle(new handle_t{s}, &close_handle);
        }
    }
}
