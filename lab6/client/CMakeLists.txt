cmake_minimum_required(VERSION 3.0)

set(PROJECT client)

project(${PROJECT})

#set(HEADERS)

set(SOURCES
        main.c
        )

set(GXX_FLAGS "-g -fsanitize=address")

find_package(Threads REQUIRED)
include_directories(${PTHREAD_INCLUDE_DIR})


find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GXX_FLAGS}")

add_executable(${PROJECT} ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT} ${PTHREAD_LIBRARIES})
target_link_libraries(${PROJECT} ${CURSES_LIBRARIES})
