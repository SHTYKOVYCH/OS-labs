cmake_minimum_required(VERSION 3.0)

set(PROJECT encoder)

project(${PROJECT})

set(GXX_FLAGS "-g -fsanitize=address")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GXX_FLAGS}")

add_executable(${PROJECT} encoder.c)
