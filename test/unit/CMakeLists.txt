project(libsocketscpp_tests_unit)

set(TEST_FILES main.cpp connection_test.cpp ipaddress_test.cpp free_system_func_test.cpp)
set(TEST_NAME libsocketcpp_unit_test)

add_executable(${TEST_NAME} ${TEST_FILES})
target_link_libraries(${TEST_NAME} PRIVATE socketscpp)

add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

add_custom_command(TARGET ${TEST_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:socketscpp>
        $<TARGET_FILE_DIR:${TEST_NAME}>)