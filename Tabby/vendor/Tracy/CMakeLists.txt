cmake_minimum_required(VERSION 3.19)

project(Tracy)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE PROJECT_SOURCES
        "*.h"
        "*.hpp"
        "*.cpp"
        )

add_library(Tracy STATIC
        ${PROJECT_SOURCES}
        )
