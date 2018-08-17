project(test)

add_executable(test main.cpp bytestring.cpp)

target_link_libraries(test socketscpp)