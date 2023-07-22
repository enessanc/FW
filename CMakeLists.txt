cmake_minimum_required(VERSION 3.25)
project(FW LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(TEST_STATUS ON)

add_subdirectory(src)

target_include_directories(FW PRIVATE ${CMAKE_CURRENT_LIST_DIR}/external/c_library_v2)

add_library(asio INTERFACE)
target_include_directories(asio INTERFACE ${CMAKE_CURRENT_LIST_DIR}/external/asio/asio/include)
target_link_libraries(asio INTERFACE <threads>)

target_link_libraries(FW PRIVATE asio)
target_include_directories(FW INTERFACE ${CMAKE_CURRENT_LIST_DIR}/external/asio/asio/include)

if(TEST_STATUS)
    enable_testing()
    message("Tests for FW is starting.")
    add_subdirectory(test)
endif()