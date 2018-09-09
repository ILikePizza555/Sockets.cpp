project(unit_test)

add_executable(unit_test main.cpp bytestring.cpp buffer_test.cpp)

target_link_libraries(unit_test PRIVATE socketscpp)

add_custom_command(TARGET unit_test POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	$<TARGET_FILE:socketscpp>
	$<TARGET_FILE_DIR:unit_test>)
