cmake_minimum_required(VERSION 3.10)
project(minic C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(include)

file(GLOB SOURCES src/*.c)

add_executable(minic ${SOURCES})
