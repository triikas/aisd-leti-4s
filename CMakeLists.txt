cmake_minimum_required(VERSION 3.26)
project(aisd_leti_4s)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXE_LINKER_FLAGS "-static")

add_executable(lab2 lab2/figure.cpp lab2/figure.h)
add_executable(lab4 lab4/lab3.cc)
