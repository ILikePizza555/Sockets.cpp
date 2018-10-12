//
// Created by avris on 10/10/2018.
//

#pragma once

#include "handle.h"
#include <string>

namespace sockets {
    namespace abl {
        namespace options {
            enum class option_level {
                SOCKET,
                TCP,
                UDP,
                IP,
                IPv6
            };

            /**
             * Sets an option on the socket.
             * @param handle The socket handle.
             * @param level The level of the option.
             * @param name The name of the option.
             * @param value The value of the option.
             */
            void set(HandleRef handle, option_level level, const std::string& name, const void* value);

            void set_bool(HandleRef handle, option_level level, const std::string& name, bool value);

            void set_int(HandleRef handle, option_level level, const std::string& name, int value);

            /**
             * Gets an option on the socket.
             * @param handle The socket handle
             * @param level The level of the option.
             * @param name The name of the option.
             * @return
             */
            void* get(HandleRef handle, option_level level, const std::string& name);

            bool get_bool(HandleRef handle, option_level level, const std::string& name);

            int get_int(HandleRef handle, option_level level, const std::string& name);

            /**
             * Enables or disables the debug option on a socket.
             * @param handle The socket handle.
             * @param enable
             */
            void set_debug(HandleRef handle, bool enable);

            /**
             * Enable or disable the ability to send broadcast messages on a socket.
             * @param handle
             * @param enable
             */
            void set_broadcast(HandleRef handle, bool enable);

            /**
             * Enables or disables the reuse of local address when calling bind() on a local socket.
             * @param handle
             * @param enable
             */
            void set_reuse_addr(HandleRef handle, bool enable);

            /**
             * Enable or disable the keeping alive of connections by sending periodic messages.
             * @param handle
             * @param enable
             */
            void set_keepalive(HandleRef handle, bool enable);

            /**
             * Sets the send buffer size.
             * @param handle
             * @param size
             */
            void set_send_buffer_size(HandleRef handle, int size);

            /**
             * Sets the recieve buffer size.
             * @param handle
             * @param size
             */
            void set_recv_buffer_size(HandleRef handle, int size);

            /**
             * Enable ipv6 mapping on the socket. This only applies to sockets with the ipv6 family.
             * @param handle
             * @param size
             */
            void enable_ipv6_mapping(HandleRef handle, int size);
        }
    }
}