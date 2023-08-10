cmake_minimum_required(VERSION 3.5)
project(FW LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(TEST_STATUS ON)

add_subdirectory(src)

target_include_directories(FW PUBLIC ${CMAKE_CURRENT_LIST_DIR}/external/c_library_v2)
target_include_directories(FW PUBLIC ${CMAKE_CURRENT_LIST_DIR}/external/hashMap/inc)

if(TEST_STATUS)

    if(LINUX)
        enable_testing()
        message("Tests for FW is starting in linux environment.")
        add_subdirectory(test_linux)
    endif()

    if(WIN32)
        message("Tests for FW is starting in windows environment.")
        add_subdirectory(test_windows)
    endif()

endif()