project(libsocketscpp_tests_system)

function(new_test TEST_NAME)
    add_executable(${TEST_NAME} ${TEST_NAME}.cpp)
    target_link_libraries(${TEST_NAME} PRIVATE socketscpp)

    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

    add_custom_command(TARGET ${TEST_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:socketscpp>
            $<TARGET_FILE_DIR:${TEST_NAME}>)
endfunction()

new_test(client_connect_test)
new_test(tcp_server_test)