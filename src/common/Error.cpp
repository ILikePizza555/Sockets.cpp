#include <utility>

#include <Error.h>
#include <sstream>

namespace sockets {
    const char*
    StringError::what() const noexcept
    {
        std::stringstream s;
        return string(std::move(s)).c_str();
    }

    MethodError::MethodError(std::string tfn, std::string efn, int ec, MethodError::lookup_t lookup) :
    throwing_function_name(std::move(tfn)), erroring_function_name(std::move(efn)), error_code(ec), lookup(lookup) {}

    std::string MethodError::string(std::stringstream) const
    {
        std::stringstream ss;
        ss << "MethodError thrown by " << this->throwing_function_name << " ";
        ss << "when calling " << erroring_function_name << ". ";
        ss << "Error code: " << error_code;

        if(lookup)
            ss << ": " << lookup(error_code);

        return ss.str();
    }

    ClosedError::ClosedError(std::string rn, std::string on) :
    resource_name(std::move(rn)), operation_name(std::move(on)) {}

    std::string ClosedError::string(std::stringstream ss) const
    {
        ss << "ClosedError: Cannot perform " << this->operation_name << " on " << this->resource_name << ": ";
        ss << "The resource is closed.";

        return ss.str();
    }

    InvalidStateError::InvalidStateError(std::string class_name, std::string function_name, std::string message) :
    class_name(std::move(class_name)), function_name(std::move(function_name)), message(std::move(message)) {}

    std::string InvalidStateError::string(std::stringstream ss) const
    {
        ss << "InvalidStateError: when calling " << class_name << "::" << function_name << ": " << message;
        return ss.str();
    }

    InvalidSocketError::InvalidSocketError(std::string class_name, std::string function_name) :
    InvalidStateError(std::move(class_name), std::move(function_name), "invalid socket")
    {}

    std::string InvalidSocketError::string(std::stringstream ss) const
    {
        ss << class_name << "::" << function_name << ": invalid socket";
        return ss.str();
    }

    SocketReadError::SocketReadError(std::string tfn) : MethodError(std::move(tfn), "recv"),
                                                        type(map_error_type(get_error_code()))
    {
    }

    std::string SocketReadError::string(std::stringstream ss) const
    {
        ss << "SocketReadError thrown by " << throwing_function_name;
        ss << "Error code: " << error_code;

        if(lookup)
            ss << ": " << lookup(error_code);

        return ss.str();
    }

    SocketWriteError::SocketWriteError(std::string tfn) : MethodError(std::move(tfn), "send"),
                                                          type(map_error_type(get_error_code()))
    {
    }

    std::string SocketWriteError::string(std::stringstream ss) const
    {
        ss << "SocketWriteError thrown by " << throwing_function_name;
        ss << "Error code: " << error_code;

        if(lookup)
            ss << ": " << lookup(error_code);

        return ss.str();
    }
}
