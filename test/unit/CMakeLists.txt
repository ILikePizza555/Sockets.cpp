project(libsocketscpp_unit_tests)

set(TEST_FILES main.cpp connection_test.cpp)
Create_library_test(unit_test ${TEST_FILES})