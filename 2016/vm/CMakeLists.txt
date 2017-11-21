cmake_minimum_required(VERSION 2.8.5)
project(vm C CXX ASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")
set(CMAKE_BUILD_TYPE "Debug")
enable_testing()

find_package(Threads REQUIRED)
find_package(GTest REQUIRED)

# I'd like to use type OBJECT type but those appears to not obey the expected transitive relation.
# https://stackoverflow.com/questions/38832528/transitive-target-include-directories-on-object-libraries
add_library(sources STATIC
    assembler.cpp
    codegenerator.h
    compiler.cpp
    interpreter.cpp
    opcodes.cpp
    vm.h
)

macro(vm_test name)
    add_executable(${name} "${name}.cpp")
    target_link_libraries(${name} sources)
    target_link_libraries(${name} GTest::GTest GTest::Main)
    add_test(${name} ${name})
endmacro(vm_test)

macro(vm_benchmark name)
    add_executable(${name} "${name}.cpp")
    target_link_libraries(${name} sources)
    target_link_libraries(${name} "-lbenchmark -lpthread")
    add_test(${name} ${name})
endmacro(vm_benchmark)

target_compile_options(sources PUBLIC -Og)
target_compile_options(sources PUBLIC -fno-omit-frame-pointer)
target_compile_options(sources PUBLIC -g3)
target_compile_options(sources PUBLIC -Wall)
target_compile_options(sources PUBLIC -Wextra)
target_compile_options(sources PUBLIC -Werror)
target_compile_options(sources PUBLIC -Wswitch-enum)
target_compile_options(sources PUBLIC -Wswitch-default)
target_compile_options(sources PUBLIC -std=c++14)

vm_test(interpreter_unittest)
vm_test(assembler_unittest)
vm_test(exec_integrationtest)
vm_test(compiler_unittest)
vm_test(codegenerator_unittest)


vm_benchmark(integer_ops_benchmark)
