#include <utility>

#include <SocketException.h>
#include <sstream>

namespace sockets {
    MethodError::MethodError(std::string tfn, std::string efn, int ec, MethodError::lookup_t lookup) :
    throwing_function_name(std::move(tfn)), erroring_function_name(std::move(efn)), error_code(ec), lookup(lookup) {}

    std::string MethodError::string() const
    {
        std::stringstream ss;
        ss << "MethodError error thrown by " << this->throwing_function_name << " ";
        ss << "when calling " << erroring_function_name << ". ";
        ss << "Error code: " << error_code;

        if(lookup)
            ss << ": " << lookup(error_code);

        return ss.str();
    }

    const char*
    MethodError::what() const noexcept
    {
        return string().c_str();
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
}
