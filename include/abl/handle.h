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

        using UniqueHandle = std::unique_ptr<handle_t>;
        using SharedHandle = std::shared_ptr<handle_t>;
    }
}