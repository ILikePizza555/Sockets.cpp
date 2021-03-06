#pragma once

#include <exception>
#include <functional>
#include <string>
#include <sstream>
#include "sockets/abl/enums.h"

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

        const char* what() const noexcept final;
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

    class InvalidStateError : public StringError
    {
    public:
        const std::string class_name;
        const std::string function_name;
        const std::string message;

        InvalidStateError(std::string class_name, std::string function_name, std::string message);

        std::string
        string(std::stringstream ss) const override;
    };

    /**
     * Thrown when an operation is attempted on an invalid socket
     */
    class InvalidSocketError : public InvalidStateError
    {
    public:
        InvalidSocketError(std::string class_name, std::string function_name);

        std::string
        string(std::stringstream ss) const final;
    };

    /**
     * Thrown by a socket class if there is an error with a call to recv()
     */
    class SocketReadError : public MethodError
    {
    public:
        enum ErrorType
        {
            NOT_INITIALIZED,
            WOULD_BLOCK,
            INVALID_HANDLE,
            NOT_SOCKET,
            TIMED_OUT,
            INTERRUPTED,
            NOT_CONNECTED,
            OTHER
        };
    protected:
        ErrorType map_error_type(int code);

    public:
        /** Type of error */
        const ErrorType type;

        SocketReadError(std::string tfn);

        std::string string(std::stringstream ss) const final;
    };

    class SocketWriteError : public MethodError
    {
    public:
        enum ErrorType
        {
            NOT_INITIALIZED,
            TIMED_OUT,
            WOULD_BLOCK,
            INVALID_HANDLE,
            NOT_SOCKET,
            NOT_CONNECTED,
            INTERRUPTED,
            CONNECTION_RESET,
            CONNECTION_ABORTED,
            OTHER
        };

    protected:
        ErrorType map_error_type(int code);

    public:
        /** Type of error */
        ErrorType type;

        SocketWriteError(std::string tfn);

        std::string string(std::stringstream ss) const final;
    };
}
