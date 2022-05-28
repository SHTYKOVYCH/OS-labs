cmake_minimum_required(VERSION 3.0)

set(PROJECT term)

project(${PROJECT})

set(HEADERS
        )

set(SOURCES
        main.c
        )

find_package(Curses REQUIRED)

set(GXX_FLAGS "-g -fsanitize=address")
include_directories(${CURSES_INCLUDE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GXX_FLAGS}")

add_executable(${PROJECT} ${HEADERS} ${SOURCES})

target_link_libraries(term ${CURSES_LIBRARIES})