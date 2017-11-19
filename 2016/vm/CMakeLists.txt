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
    codegenerator.h
    compiler.cpp
    interpreter.cpp
    opcodes.cpp
    vm.h
)

macro(vm_test name)
    add_executable(${name} "${name}.cpp" ${VM_SOURCES})
    target_link_libraries(${name} GTest::GTest GTest::Main)
    add_test(${name} ${name})
endmacro(vm_test)

vm_test(interpreter_unittest)
vm_test(assembler_unittest)
vm_test(exec_integrationtest)
vm_test(compiler_unittest)
vm_test(codegenerator_unittest)


macro(vm_benchmark name)
    add_executable(${name} "${name}.cpp" ${VM_SOURCES})
    target_link_libraries(${name} "-lbenchmark -lpthread")
    add_test(${name} ${name})
endmacro(vm_benchmark)

vm_benchmark(integer_ops_benchmark)
