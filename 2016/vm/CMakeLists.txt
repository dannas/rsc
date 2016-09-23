cmake_minimum_required(VERSION 2.8.5)
project(vm C CXX ASM)

find_package(Threads)
enable_testing()
find_package(GTest REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g3 -Wall -Wextra -Werror -Wswitch-enum -Wswitch-default -std=c++14")

set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")
set(CMAKE_BUILD_TYPE "Debug")

add_executable(interpreter-test
    interpreter_unittest.cpp
    interpreter.cpp
    opcodes.cpp
)

add_executable(assembler-test
    assembler_unittest.cpp
    assembler.cpp
    opcodes.cpp
)

target_link_libraries(interpreter-test GTest::GTest GTest::Main)
target_link_libraries(assembler-test GTest::GTest GTest::Main)

add_test(assembler-test assembler-test)
add_test(interpreter-test interpreter-test)

