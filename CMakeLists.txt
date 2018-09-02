cmake_minimum_required(VERSION 3.1)
project(socketscpp)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")

# Library Header Files
# This should be set to all files in include/
set(INCLUDE_FILES include/Byte.h include/ByteString.h include/Connection.h include/ServerSocket.h include/Socket.h include/SocketException.h)

# Common Implementation Files
# These are all implementations that are common across platforms

set(IMPL_COMMON src/common/ByteString.cpp src/common/SocketException.cpp)

# Unix-Specific implementation files
# These are the implementations for *nix systems
set(IMPL_UNIX src/unix/Connection.cpp src/unix/SocketException.cpp)

# Windows-Specific implementation files
# These are the implementations for windows systems
set(IMPL_WIN32)

# Create a shared library
if(UNIX)
    message(STATUS "Build for Unix has been selected")
    message(STATUS "Files are: " ${IMPL_COMMON} " " ${IMPL_UNIX})
    add_library(socketscpp SHARED ${IMPL_COMMON} ${IMPL_UNIX})
endif()

if(WIN32)
    message(STATUS "Build for Win32 has been selected")
    message(STATUS "Files are: " ${IMPL_COMMON} " " ${IMPL_WIN32})
    add_library(socketscpp SHARED ${IMPL_COMMON} ${IMPL_WIN32})
endif()

# Export the header files
set_property(TARGET socketscpp PROPERTY PUBLIC_HEADER ${INCLUDE_FILES})

target_include_directories(socketscpp PUBLIC include)

add_subdirectory(test)
