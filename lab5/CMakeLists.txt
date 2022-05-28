cmake_minimum_required(VERSION 3.0)

set(PROJECT sobel)

project(${PROJECT})

set(FLAGS "-g -fsanitize=address")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FLAGS}")

find_package(PNG REQUIRED)
find_package(Threads REQUIRED)

include_directories(${PNG_INCLUDE_DIR})
include_directories(${THREADS_INCLUDE_DIR})

add_executable(${PROJECT} main.c)

target_link_libraries(${PROJECT} ${PNG_LIBRARIES})
target_link_libraries(${PROJECT} ${THREADS_LIBRARIES})
target_link_libraries(${PROJECT} m)