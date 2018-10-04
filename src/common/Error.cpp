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

    InvalidSocketError::InvalidSocketError(std::string class_name, std::string function_name) :
    class_name(std::move(class_name)), function_name(std::move(function_name))
    {}

    const char* InvalidSocketError::what() const noexcept
    {
        std::stringstream ss;
        ss << class_name << "::" << function_name << ": invalid socket";
        return ss.str().c_str();
    }

    ConnectionResetError::ConnectionResetError()
    {}

    const char* ConnectionResetError::what() const noexcept
    {
        return "Connection reset by peer.";
    }
}
