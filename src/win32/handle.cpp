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
    }
}
