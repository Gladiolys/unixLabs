cmake_minimum_required(VERSION 3.15)
project(threads_messaging)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS -pthread)

add_executable(threads_messaging main.cpp)