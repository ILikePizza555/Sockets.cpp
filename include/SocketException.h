#pragma once

#include <exception>
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

    class _Exception : public std::exception
    {
    public:
        /** Throwing class name */
        const std::string class_name;
        /** Throwing function name */
        const std::string function_name;

        _Exception(std::string function_name);

        _Exception(std::string class_name, std::string function_name);

        std::string
        full_name() const;
    };

    /**
     * Thrown when an operation is attempted on an invalid socket
     */
    class InvalidSocketError : public _Exception
    {
    public:
        InvalidSocketError(std::string class_name, std::string function_name);

        virtual const char *
        what() const noexcept;
    };

    class ClosedError : public _Exception
    {
    public:
        ClosedError(std::string class_name, std::string function);

        virtual const char *
        what() const noexcept;
    };

    class SocketError : public _Exception
    {
    public:
        const int error_code;
        const std::string message;

        SocketError(std::string class_name, std::string function_name, std::string message, int error_code);

        SocketError(std::string function_name, std::string message, int error_code);

        virtual const char *
        what() const noexcept;
    };

    class AddressInfoError : public std::exception
    {
    public:
        std::string function_name;
        const int error_value;

        AddressInfoError(std::string function_name, int error_value);

        virtual const char *
        what() const noexcept;
    };
}
