cmake_minimum_required(VERSION 3.0)

set(PROJECT arhv)

project(${PROJECT})

set(HEADERS
        error_codes.h
        error_handler.h
        read-file.h
        parse-args.h
        )

set(SOURCES
        main.c
        error_handler.c
        read-file.c
        parse-args.c
        )

set( GXX_FLAGS "-g" )

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GXX_FLAGS}" )

add_executable(${PROJECT} ${HEADERS} ${SOURCES} )
