project(unit_test)

add_executable(unit_test main.cpp bytestring.cpp)

target_link_libraries(unit_test PRIVATE socketscpp)
