
cmake_minimum_required(VERSION 2.8)

project(SimpleHttpServer)

set (SimpleHttpServer_VERSION_MAJOR 0)
set (SimpleHttpServer_VERSION_MINOR 1)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

include_directories("include")

set(SOURCE_FILES
    src/main.c
    src/cmdargs.c
)

add_executable (simple_http_server ${SOURCE_FILES})
