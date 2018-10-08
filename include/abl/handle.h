//
// Defines the abstraction for the system socket handle.
// Created by avris on 10/4/2018.
//

#pragma once

#include <memory>

namespace sockets {
    namespace abl {
        /**
         * Struct that contains the system socket handle.
         */
        struct handle_t;

        namespace {
            /**
             * Type unsafe method for getting the system handle type.
             * @param handle
             * @return
             */
            void* get_system_handle(const handle_t* handle);
            void* get_system_handle(const std::unique_ptr<handle_t>& handle);
            void* get_system_handle(const std::shared_ptr<handle_t>& handle);
        }

        /**
         * Returns the system handle stored in handle_t
         * @tparam SystemType The type of the handle the system uses.
         * @return
         */
        template<typename SystemType>
        SystemType get_handle(const handle_t* handle)
        {
            return *reinterpret_cast<SystemType*>(get_system_handle(handle));
        }

        template<typename SystemType>
        SystemType get_handle(const std::unique_ptr<handle_t>& handle)
        {
            return *reinterpret_cast<SystemType>(get_system_handle(handle));
        }

        template<typename SystemType>
        SystemType get_handle(const std::shared_ptr<handle_t>& handle)
        {
            return *reinterpret_cast<SystemType>(get_system_handle(handle));
        }
    }
}