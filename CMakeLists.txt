cmake_minimum_required(VERSION 3.10)
project(ezim)

# 设置 C 标准
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

include_directories(inc)

file(GLOB SOURCES "src/*.c")

add_executable(ezim ${SOURCES})
