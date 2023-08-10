add_executable(FW_TEST_WINDOWS test_windows_main.cpp)
message("Target linked with FW")
target_link_libraries(FW_TEST_WINDOWS PUBLIC FW)