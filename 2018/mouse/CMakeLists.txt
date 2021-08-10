cmake_minimum_required(VERSION 2.8.12)
project(mouse C)

# Optimization level -Og does not preserve macros
set(CMAKE_C_FLAGS_DEBUG "-O0 -g3 -Wall -Wextra")
set(CMAKE_C_FLAGS_RELEASE "-O2 -g3 -Wall -Wextra")
set(PARTS )

set_source_files_properties(${PARTS} PROPERTIES HEADER_FILE_ONLY TRUE)
add_executable(main main.c ${PARTS})
