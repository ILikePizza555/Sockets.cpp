cmake_minimum_required(VERSION 3.1)
project(socketscpp)

set(CMAKE_CXX_STANDARD 14)

option(BUILD_TESTS "Enable building of tests" OFF)

# Library Header Files
# This should be set to all files in include/
set(INCLUDE_FILES include/sockets/Byte.h include/sockets/Connection.h include/sockets/Error.h include/sockets/Byte.h
        include/sockets/TCPSocket.h include/sockets/socket_type_traits.h
        include/sockets/abl/enums.h include/sockets/abl/handle.h include/sockets/abl/ip.h include/sockets/abl/system.h)

# Common Implementation Files
# These are all implementations that are common across platforms

set(IMPL_COMMON src/common/Error.cpp src/common/Byte.cpp src/common/TCPSocket.cpp src/common/Connection.cpp src/common/TCPServerSocket.cpp)

# Unix-Specific implementation files
# These are the implementations for *nix systems
set(IMPL_UNIX src/unix/Error.cpp src/unix/ip.cpp src/unix/handle.cpp src/unix/system.cpp)

# Windows-Specific implementation files
# These are the implementations for windows systems
set(IMPL_WIN32 src/win32/Error.cpp src/win32/handle.cpp src/win32/ip.cpp src/win32/system.cpp src/win32/win32.cpp)

# Create a shared library
if(UNIX)
    message(STATUS "Build for Unix has been selected")
    message(STATUS "Files are: " ${IMPL_COMMON} " " ${IMPL_UNIX})
    add_library(socketscpp ${IMPL_COMMON} ${IMPL_UNIX})
endif()

if(WIN32)
    list(APPEND INCLUDE_FILES include/sockets/abl/win32.h)
    message(STATUS "Build for Win32 has been selected")
    message(STATUS "Files are: " ${IMPL_COMMON} " " ${IMPL_WIN32})
    add_library(socketscpp ${IMPL_COMMON} ${IMPL_WIN32})
    target_link_libraries(socketscpp wsock32 ws2_32)
endif()

# Install directives
install(TARGETS socketscpp DESTINATION lib)
install(DIRECTORY include DESTINATION include)

target_include_directories(socketscpp PUBLIC include)

if(BUILD_TESTS)
    add_subdirectory(test)
endif()