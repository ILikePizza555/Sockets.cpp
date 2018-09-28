#pragma once

#include <exception>
#include <functional>
#include <string>
#include "abl/abl.h"

namespace sockets {

    /**
     * Gets the last error code set by the system
     */
    int
    get_error_code();

    std::string
    get_error_message(int code);

    /**
     * Error thrown when a C-library function returns an error.
     */
    class MethodError : public std::exception
    {
    public:
        /** Name of the function throwing the error */
        const std::string throwing_function_name;
        /** Name of the function causing the error */
        const std::string erroring_function_name;
        /** Error code the function gave. */
        const int error_code;

        using lookup_t = std::function<std::string(int)>;
        const lookup_t lookup;

        MethodError(std::string tfn, std::string efn, int ec, lookup_t = {});

        /**
         * @return A string containing the error message.
         */
        virtual std::string
        string() const;

        const char*
        what() const noexcept override;
    };

    /**
     * Thrown when an operation is attempted on an invalid socket
     */
    class InvalidSocketError : std::exception
    {
    public:
        const std::string class_name;
        const std::string function_name;

        InvalidSocketError(std::string class_name, std::string function_name);

        const char *
        what() const noexcept override;
    };
}
