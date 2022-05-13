cmake_minimum_required(VERSION 3.0)

set(PROJECT server)

project(${PROJECT})

#set(HEADERS)

set(SOURCES
        main.c
        )

set(GXX_FLAGS "-g -fsanitize=address")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GXX_FLAGS}")

add_executable(${PROJECT} ${HEADERS} ${SOURCES})
