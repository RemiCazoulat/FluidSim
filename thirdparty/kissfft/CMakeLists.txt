# thirdparty/kissfft/CMakeLists.txt
cmake_minimum_required(VERSION 3.0)
project(kissfft)

add_library(kissfft STATIC
        kiss_fft.c
        tools/kiss_fftr.c
)

target_include_directories(kissfft PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})