cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "-O3 -std=c++14 ${CMAKE_CXX_FLAGS}")

add_executable(benchmark_append benchmark_append.cpp vector.cpp)
add_executable(benchmark_construct benchmark_construct.cpp vector.cpp)
target_link_libraries(benchmark_append "-lbenchmark -lpthread")
target_link_libraries(benchmark_construct "-lbenchmark -lpthread")

