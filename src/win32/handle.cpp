//
// Created by avris on 10/4/2018.
//

#include <sockets/Error.h>
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

        SOCKET system::get_system_handle(const handle_t* handle)
        {
            if(handle != nullptr)
                return handle->socket;
            throw std::invalid_argument("handle is nullptr");
        }

        SOCKET system::get_system_handle(const UniqueHandle& handle)
        {
            if(handle != nullptr)
                return handle->socket;
            throw std::invalid_argument("handle is nullptr");
        }

        SOCKET system::get_system_handle(const SharedHandle& handle)
        {
            if(handle != nullptr)
                return handle->socket;
            throw std::invalid_argument("handle is nullptr");
        }

        handle_t* system::from_system_handle(SOCKET handle)
        {
            return new handle_t{handle};
        }

        UniqueHandle system::unique_from_system_handle(SOCKET handle)
        {
            return UniqueHandle(new handle_t{handle}, &close_handle);
        }

        SharedHandle system::shared_from_system_handle(SOCKET handle)
        {
            return SharedHandle(new handle_t{handle}, &close_handle);
        }

        UniqueHandle new_unique_handle(ip_family family, sock_type type, sock_proto protocol)
        {
            SOCKET s = socket(system::iftosys(family), system::sttosys(type), system::sptosys(protocol));

            if(s == INVALID_SOCKET)
                throw MethodError(__func__, "socket");

            return UniqueHandle(new handle_t{s}, &close_handle);
        }

        SharedHandle new_shared_handle(ip_family family, sock_type type, sock_proto protocol)
        {
            SOCKET s = socket(system::iftosys(family), system::sttosys(type), system::sptosys(protocol));

            if(s == INVALID_SOCKET)
                throw MethodError(__func__, "socket");

            return SharedHandle(new handle_t{s}, &close_handle);
        }
    }
}
