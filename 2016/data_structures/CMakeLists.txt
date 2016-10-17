cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "-O2 -fno-omit-frame-pointer -std=c++14 ${CMAKE_CXX_FLAGS}")

add_executable(CyclicQueue CyclicQueue.cpp)

