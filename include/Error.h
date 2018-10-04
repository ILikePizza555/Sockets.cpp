#pragma once

#include <exception>
#include <functional>
#include <string>
#include <sstream>
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
     * @return True if the last error messages signifies that the connection was closed. False otherwise.
     */
    bool check_connection_reset();

    class StringError : public std::exception
    {
    public:
        virtual std::string string(std::stringstream ss) const = 0;

        const char* what() const noexcept override;
    };

    /**
     * Error thrown when a C-library function returns an error.
     */
    class MethodError : public StringError
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

        MethodError(std::string tfn, std::string efn, int ec = get_error_code(), lookup_t = get_error_message);

        /**
         * @return A string containing the error message.
         */
        std::string
        string(std::stringstream ss) const override;
    };

    /**
     * Thrown when an operation is attempted on a closed resource
     */
    class ClosedError : public StringError
    {
    public:
        const std::string resource_name;
        const std::string operation_name;

        ClosedError(std::string rn, std::string on);

        std::string
        string(std::stringstream ss) const override;
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
