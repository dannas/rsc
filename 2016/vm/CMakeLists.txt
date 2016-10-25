cmake_minimum_required(VERSION 2.8.5)
project(vm C CXX ASM)

find_package(Threads)
enable_testing()
find_package(GTest REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g3 -Wall -Wextra -Werror -Wswitch-enum -Wswitch-default -std=c++14")

set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")
set(CMAKE_BUILD_TYPE "Debug")

set(VM_SOURCES
    assembler.cpp
    interpreter.cpp
    opcodes.cpp
    optimizer.cpp
    vm.h
)


add_executable(interpreter-test
    interpreter_unittest.cpp
    ${VM_SOURCES}
)

add_executable(assembler-test
    assembler_unittest.cpp
    ${VM_SOURCES}
)

add_executable(exec-test
    exec_integrationtest.cpp
    ${VM_SOURCES}
)

add_executable(optimizer-test
    optimizer_unittest.cpp
    ${VM_SOURCES}
)

target_link_libraries(interpreter-test GTest::GTest GTest::Main)
target_link_libraries(assembler-test   GTest::GTest GTest::Main)
target_link_libraries(exec-test        GTest::GTest GTest::Main)
target_link_libraries(optimizer-test   GTest::GTest GTest::Main)

add_test(assembler-test   assembler-test)
add_test(interpreter-test interpreter-test)
add_test(exec-test        interpreter-test)
add_test(optimizer-test   optimizer-test)

