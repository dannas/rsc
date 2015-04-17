cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5Core)

find_package(PkgConfig REQUIRED)
pkg_check_modules(PKG REQUIRED libglog)
include_directories(${PKG_INCLUDE_DIRS})

add_executable(stringtest stringtest.cpp)
target_link_libraries(stringtest Qt5::Core "${PKG_LDFLAGS} -lfolly -lfollybenchmark")

add_executable(String String.cpp)

