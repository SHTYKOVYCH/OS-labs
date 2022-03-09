cmake_minimum_required(VERSION 3.0)

set(PROJECT arhv)

project(${PROJECT})

set(HEADERS
        error_codes.h
        error_handler.h
        read-file.h
        parse-args.h
        dir_tree.h
        depack.h
        string-2-json.h
        read_json_from_file.h
        write_file.h
        )

set(SOURCES
        main.c
        error_handler.c
        read-file.c
        parse-args.c
        dir_tree.c
        depack.c
        string-2-json.c
        read_json_from_file.c
        write_file.c
        )

set( GXX_FLAGS "-g -fsanitize=address" )

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GXX_FLAGS}" )

add_executable(${PROJECT} ${HEADERS} ${SOURCES} )
