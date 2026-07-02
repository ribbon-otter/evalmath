cmake_minimum_required(VERSION 3.14)
project(evalmath LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(evalmath evalmath.cpp)
target_compile_options(evalmath PRIVATE -Wall -Wextra -Wpedantic
    $<$<CXX_COMPILER_ID:GNU>: -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wundef -fdiagnostics-show-option >
    $<$<CXX_COMPILER_ID:Clang>: -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-c++20-compat -Wno-pre-c++20-compat-pedantic -Wno-covered-switch-default -Wno-padded>
)
target_include_directories(evalmath PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


option(BUILD_TESTS "Build tests" Off)
if(BUILD_TESTS)
    # evalm is only for fuzzing and development pretty much
    # and this way we avoid superfluous compilation failures if std::print isn't supported
    # by the library user's compiler
    add_executable(evalm evalm.cpp)
    target_link_libraries(evalm PRIVATE evalmath)
    target_compile_options(evalm PRIVATE -g)
    
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    FetchContent_MakeAvailable(googletest)

    enable_testing()
    add_executable(evalmath_tests evalmath_tests.cpp)
    target_compile_options(evalmath_tests PRIVATE -g)
    target_compile_options(evalmath PRIVATE -g)
    target_link_libraries(evalmath_tests PRIVATE evalmath GTest::gtest_main)
    add_test(NAME evalmath_tests COMMAND evalmath_tests)
endif()
# vim: ts=4 sw=4 expandtab

