cmake_minimum_required(VERSION 2.8.5)
project(vm C CXX)

find_package(Threads)
enable_testing()
find_package(GTest REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g3 -Wall -Wextra -Werror -Wswitch-enum -Wswitch-default -std=c++14")

set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")
set(CMAKE_BUILD_TYPE "Debug")

set(SOURCES
    algorithm.h
    unique_ptr.h
)


add_executable(algorithm-test
    algorithm_unittest.cpp
    algorithm.h
)

target_link_libraries(algorithm-test GTest::GTest GTest::Main)

add_test(algorithm-test   algorithm-test)

