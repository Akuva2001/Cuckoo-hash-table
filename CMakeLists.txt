cmake_minimum_required(VERSION 3.19)
project(CuckooHash)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_executable(CuckooHash main.cpp hash.h)