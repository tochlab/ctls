cmake_minimum_required(VERSION 3.10)
project(ctls C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-Wall -Wextra -O3 -march=native")

add_executable(hex2bin hex2bin.c)
add_executable(rot13 rot13.c)
add_executable(xor xor.c)
add_executable(listdir lib/listdir.c listdir.c)
add_executable(randomq randomq.c)
add_executable(cbuf cbuf.c lib/cbuf.c)
#add_executable(rawtest lib/rawfs.c lib/queue.c rawtest.c)
#target_link_libraries(rawtest pthread)
