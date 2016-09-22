cmake_minimum_required(VERSION 2.8.5)
project(vm C CXX ASM)

find_package(Threads)
enable_testing()
find_package(GTest REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g3 -Wall -Wextra -Werror -Wswitch-enum -Wswitch-default -std=c++14")

set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")
set(CMAKE_BUILD_TYPE "Debug")

add_executable(interpreter
    interpreter.cpp
    opcodes.cpp
)
target_link_libraries(interpreter GTest::GTest GTest::Main)

add_executable(interpreter-test
    interpreter.cpp
    opcodes.cpp
)
target_link_libraries(interpreter-test GTest::GTest GTest::Main)

add_executable(assembler-test
    assembler_unittest.cpp
    assembler.cpp
    opcodes.cpp
)

target_link_libraries(assembler-test GTest::GTest GTest::Main)

enable_testing()

add_test(assembler-test assembler-test)
add_test(interpreter-test interpreter-test)

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
