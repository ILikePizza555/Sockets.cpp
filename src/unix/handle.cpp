//
// Created by avris on 10/26/2018.
//

#include <sockets/abl/handle.h>
#include <sockets/abl/system.h>
#include <sockets/Error.h>
#include <unistd.h>

namespace sockets {
    namespace abl {
        struct handle_t
        {
            int socket;
        };

        void close_handle(handle_t* handle)
        {
            close(handle->socket);
            handle->socket = 0;
        }

        UniqueHandle new_unique_handle(ip_family family, sock_type type, sock_proto protocol)
        {
            int s = socket(system::iftosys(family), system::sttosys(type), system::sptosys(protocol));

            if(s == -1)
                throw MethodError(__func__, "socket");

            return UniqueHandle(new handle_t{s}, &close_handle);
        }

        SharedHandle new_shared_handle(ip_family family, sock_type type, sock_proto protocol)
        {
            int s = socket(system::iftosys(family), system::sttosys(type), system::sptosys(protocol));

            if(s == -1)
                throw MethodError(__func__, "socket");

            return SharedHandle(new handle_t{s}, &close_handle);
        }

        int system::get_system_handle(HandleRef handle)
        {
            if(handle != nullptr)
                return handle->socket;
            throw std::invalid_argument("handle is nullptr");
        }

        int system::get_system_handle(const sockets::abl::UniqueHandle &handle)
        {
            if(handle != nullptr)
                return handle->socket;
            throw std::invalid_argument("handle is nullptr");
        }

        int system::get_system_handle(const sockets::abl::SharedHandle &handle)
        {
            if(handle != nullptr)
                return handle->socket;
            throw std::invalid_argument("handle is nullptr");
        }

       handle_t* system::from_system_handle(int handle)
       {
            return new handle_t{handle};
       }

       UniqueHandle system::unique_from_system_handle(int handle)
       {
           return UniqueHandle(new handle_t{handle}, &close_handle);
       }

       SharedHandle system::shared_from_system_handle(int handle)
       {
            return SharedHandle(new handle_t{handle}, &close_handle);
       }
    }
}