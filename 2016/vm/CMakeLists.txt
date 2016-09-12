cmake_minimum_required(VERSION 2.8.5)
project(vm C CXX ASM)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g3 -Wall -Wextra -Werror -Wswitch-enum -Wswitch-default -std=c++14")

add_executable(interpreter
    interpreter.cpp
    opcodes.cpp
)

add_executable(assembler
    assembler.cpp
    opcodes.cpp
)
add_executable(disassembler
    assembler.cpp
    opcodes.cpp
)

add_executable(assembler-test
    assembler.cpp
    opcodes.cpp
)

enable_testing()

add_test(assembler-test assembler-test)

set(ASSEMBLER_TEST_FILES
    testdata/add.asm
    testdata/call.asm
    testdata/halt.asm
    testdata/print.asm
)

foreach(test ${ASSEMBLER_TEST_FILES})
    add_test(${test} 
        python ${CMAKE_SOURCE_DIR}/testdriver.py ${CMAKE_SOURCE_DIR} ${PROJECT_BINARY_DIR} ${test})
endforeach(test)
