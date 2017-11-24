cmake_minimum_required(VERSION 2.8.5)
project(danstl CXX)

find_package(Threads)
enable_testing()
find_package(GTest REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g3 -Wall -Wextra -Werror -Wswitch-enum -Wswitch-default  -std=c++14")

set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")
set(CMAKE_BUILD_TYPE "Debug")

set(SOURCES
    algorithm.h
    unique_ptr.h
    string.h
    map.h
)

add_executable(algorithm-test
    algorithm_unittest.cpp
    algorithm.h
)

add_executable(unique_ptr-test
    unique_ptr_unittest.cpp
    unique_ptr.h
)

add_executable(string-test
    string_unittest.cpp
    string.h
)

add_executable(map-test
    map_unittest.cpp
    map.h
)

target_link_libraries(algorithm-test GTest::GTest GTest::Main)
target_link_libraries(unique_ptr-test GTest::GTest GTest::Main)
target_link_libraries(string-test GTest::GTest GTest::Main)
target_link_libraries(map-test GTest::GTest GTest::Main)

add_test(algorithm-test   algorithm-test)
add_test(unique_ptr-test   unique_ptr-test)
add_test(string-test   string-test)
add_test(map-test   map-test)

