cmake_minimum_required(VERSION 3.1)
project(socketscpp)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")

add_library(socketscpp SHARED 
            src/common/ByteString.cpp)

set_property(TARGET socketscpp PROPERTY PUBLIC_HEADER
            include/Byte.h
            include/ByteString.h)

target_include_directories(socketscpp PUBLIC include)

add_subdirectory(test)
