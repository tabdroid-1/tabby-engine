cmake_minimum_required(VERSION 3.20)
project(TabbyEngine)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 17)
set(CMAKE_VERBOSE_MAKEFILE 0) 

if (WIN32)
    cmake_policy(SET CMP0091 NEW)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS 1)
endif ()

# this runs when its now WIN32 because -w breaks windres build
if (NOT TB_DIST AND NOT WIN32)
    add_definitions(-w) # disables warnings
endif()


add_subdirectory(tabby)
add_subdirectory(app)
