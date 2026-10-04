cmake_minimum_required(VERSION 4.3)
project(ext2_FileSystem C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(ext2_FileSystem main.c structures.h
        main.c
        bitmap.h
        bitmap.c
        loader.h
        loader.c
        operations.c
        operations.h)
