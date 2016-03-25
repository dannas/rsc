cmake_minimum_required(VERSION 2.8.5)
project(vm C CXX ASM)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g3 -Wall -Wextra -Werror -Wswitch-enum -Wswitch-default -std=c++14")

add_executable(interpreter
    interpreter.cpp
)

add_executable(assembler
    assembler.cpp
)
