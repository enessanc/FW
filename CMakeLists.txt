cmake_minimum_required(VERSION 3.25)
project(FW LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(FW
        main.cpp
        src/MavlinkInterface.cpp
        src/MavlinkInterface.h
        src/ports/GenericPort.h
        src/ports/GenericPort.h src/ports/UDPPort.h src/ports/UDPPort.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/external/c_library_v2)