cmake_minimum_required(VERSION 3.14)
project(evalmath LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(evalmath evalmath.cpp)
target_include_directories(evalmath PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(evalm evalm.cpp)
target_link_libraries(evalm PRIVATE evalmath)

option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    FetchContent_MakeAvailable(googletest)

    enable_testing()
    add_executable(evalmath_tests evalmath_tests.cpp)
    target_link_libraries(evalmath_tests PRIVATE evalmath GTest::gtest_main)
    add_test(NAME evalmath_tests COMMAND evalmath_tests)
endif()
