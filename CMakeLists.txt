cmake_minimum_required(VERSION 3.25)
project(FW LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(TEST_STATUS ON)

add_subdirectory(src)

target_include_directories(FW PUBLIC ${CMAKE_CURRENT_LIST_DIR}/external/c_library_v2)

if(TEST_STATUS)
    enable_testing()
    message("Tests for FW is starting.")
    add_subdirectory(test)
endif()