#include <SocketException.h>
#include <sstream>

namespace sockets {
    _Exception::_Exception(std::string function_name) : class_name(""), function_name(function_name) {}
    _Exception::_Exception(std::string class_name, std::string function_name) : class_name(class_name), function_name(function_name) {}
    std::string _Exception::full_name() const
    {
        if(class_name.empty()) return class_name + "::" + function_name;
        return function_name;
    }

    InvalidSocketError::InvalidSocketError(std::string class_name, std::string function_name) : _Exception(class_name, function_name) {}
    const char* InvalidSocketError::what() const noexcept
    {
        std::stringstream ss;
        ss << _Exception::full_name() << ": invalid socket";
        return ss.str().c_str();
    }

    ClosedError::ClosedError(std::string class_name, std::string function_name) : _Exception(class_name, function_name) {}
    const char* ClosedError::what() const noexcept
    {
        std::stringstream ss;
        ss << full_name() << ": " << class_name << " is closed";
        return ss.str().c_str();    
    }

    SocketError::SocketError(std::string class_name, std::string function_name, std::string message, int error_code) : _Exception(class_name, function_name), error_code(error_code), message(message) {}
    SocketError::SocketError(std::string function_name, std::string message, int error_code) : _Exception(function_name), error_code(error_code), message(message) {}
    const char* SocketError::what() const noexcept
    {
        std::stringstream ss;
        ss << full_name() << ": " << message << ": " << error_code << ": " << get_error_message(error_code);
        return ss.str().c_str();
    }

    AddressInfoError::AddressInfoError(std::string function_name, int error_value) : function_name(function_name), error_value(error_value)
    {}
    const char* AddressInfoError::what() const noexcept
    {
        std::stringstream ss;
        // TODO: gai_strerror() and WSAGetLastError on Windows
        ss << function_name << ": getaddrinfo: " << error_value;
        return ss.str().c_str();
    }
}
