cmake_minimum_required(VERSION 3.0)

set(PROJECT client)

project(${PROJECT})

#set(HEADERS)

set(SOURCES
        main.c
        )

set(GXX_FLAGS "-g -fsanitize=address -mfpmath=sse -msse -msse2 -pthread -I/usr/include/gtk-4.0 -I/usr/include/gio-unix-2.0 -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/include/harfbuzz -I/usr/include/pango-1.0 -I/usr/include/fribidi -I/usr/include/harfbuzz -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/x86_64-linux-gnu -I/usr/include/cairo -I/usr/include/pixman-1 -I/usr/include/uuid -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/graphene-1.0 -I/usr/lib/x86_64-linux-gnu/graphene-1.0/include -I/usr/include/libmount -I/usr/include/blkid -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -lgtk-4 -lpangocairo-1.0 -lpango-1.0 -lharfbuzz -lgdk_pixbuf-2.0 -lcairo-gobject -lcairo -lgraphene-1.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0")

find_package(Threads REQUIRED)
include_directories(${PTHREAD_INCLUDE_DIR})


find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})
find_package(GTK REQUIRED)
include_directories(${GTK_INCLUDE_DIR})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GXX_FLAGS}")

add_executable(${PROJECT} ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT} ${PTHREAD_LIBRARIES})
target_link_libraries(${PROJECT} ${CURSES_LIBRARIES})
target_link_libraries(${PROJECT} ${GTK_LIBRARIES})