project(libsocketscpp_tests)

enable_testing()

function(Create_library_test test_name files)
    message(STATUS "Building test " ${test_name} " with files " ${files})

    add_executable(${test_name} ${files})

    add_test(NAME ${test_name} COMMAND ${test_name})

    target_link_libraries(${test_name} PRIVATE socketscpp)

    add_custom_command(TARGET ${test_name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:socketscpp>
            $<TARGET_FILE_DIR:unit_test>)
endfunction()

add_subdirectory(system)
add_subdirectory(unit)