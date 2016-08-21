cmake_minimum_required(VERSION 2.8.5)
project(cryptopals CXX)

find_package(Threads)
enable_testing()
find_package(GTest REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g3 -Wall -std=c++14 -fsanitize=address -fno-omit-frame-pointer -lm -lasan")

function (add_executables)
	foreach (file ${ARGV})
		get_filename_component (name_without_extension ${file} NAME_WE)
		add_executable(${name_without_extension} ${file} util.cpp)
	endforeach ()
endfunction ()

add_executables(
	1_1_hex_to_base64.cpp
	1_2_fixed_xor.cpp
	1_3_single_byte_xor_cipher.cpp
	1_4_detect_single_character_xor.cpp
	1_5_implement_repeating_key_xor.cpp
	1_6_break_repeating_key_xor.cpp
	1_7_aes_in_ecb_mode.cpp
	1_8_detect_aes_in_ecb_mode.cpp
	2_10_implement_cbc_mode.cpp
	2_11_an_ecb_cbc_oracle.cpp
	2_9_implement_pkcs7_padding.cpp
)

add_executable(util_test util_test.cpp util.cpp)
target_link_libraries(util_test GTest::GTest GTest::Main)

add_test(util_test util_test)
