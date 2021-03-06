cmake_minimum_required(VERSION 3.0)

set(PROJECT arhv)

project(${PROJECT})

set(HEADERS
        error_codes.h
        error_handler.h
        read-file.h
        write_file.h
        parse-args.h
        string-2-json.h
        json_stringify.h
        read_json_from_file.h
        dir_tree.h
        archive.h
        depack.h
        )

set(SOURCES
        main.c
        error_handler.c
        read-file.c
        write_file.c
        parse-args.c
        string-2-json.c
        json_stringify.c
        read_json_from_file.c
        dir_tree.c
        depack.c
        archive.c
        )

set(GXX_FLAGS "-g -fsanitize=address")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GXX_FLAGS}")

add_executable(${PROJECT} ${HEADERS} ${SOURCES})
