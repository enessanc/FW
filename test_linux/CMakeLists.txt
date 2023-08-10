include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        release-1.11.0
)
FetchContent_MakeAvailable(googletest)

add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)

add_executable(FW_TEST_LINUX
        test_main.cpp)

target_link_libraries(FW_TEST_LINUX
        PRIVATE
        GTest::GTest
        FW)


add_test(FW_TESTS FW_TEST_LINUX)



