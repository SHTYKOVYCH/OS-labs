cmake_minimum_required(VERSION 3.0)

set(PROJECT server)

project(${PROJECT})

set(HEADERS
        client.h
        room.h)

set(SOURCES
        main.c
        client.c
        room.c
        )

set(GXX_FLAGS "-g -fsanitize=address")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GXX_FLAGS}")
find_package(Threads REQUIRED)

include_directories(${PTHREAD_INCLUDE_DIR})

add_executable(${PROJECT} ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT} ${PTHREAD_LIBRARIES})
