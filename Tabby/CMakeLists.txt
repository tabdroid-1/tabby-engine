cmake_minimum_required(VERSION 3.22)
project(tabby)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(APPLE)
    message(STATUS "Including Objective-C++ for APPLE")
    list(APPEND LANGUAGES OBJC)
    list(APPEND LANGUAGES OBJCXX)
    set(CMAKE_OBJCXX_STANDARD 14)
    set(CMAKE_OBJCXX_STANDARD_REQUIRED ON)
    set(CMAKE_OBJCXX_EXTENSIONS OFF)
endif()

macro(append_linker_flags FLAGS)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${FLAGS}")
endmacro()

option(WEB "Enable Web Build" OFF)
if (EMSCRIPTEN)
    set(WEB ON CACHE BOOL "Enable Web")
endif (EMSCRIPTEN)


set(PROJECT_SOURCE_DIR "src")
set(PROJECT_VENDOR_DIR "vendor")

# ##############################################################################
# Add dependencies 
# ##############################################################################

set(SDL2 "${PROJECT_VENDOR_DIR}/SDL2")
set(GLAD "${PROJECT_VENDOR_DIR}/glad")
set(GLM "${PROJECT_VENDOR_DIR}/glm")
set(STB "${PROJECT_VENDOR_DIR}/stb")
set(SPDLOG "${PROJECT_VENDOR_DIR}/spdlog")
set(MSDF-ATLAS-GEN "${PROJECT_VENDOR_DIR}/msdf-atlas-gen")
set(YAML-CPP "${PROJECT_VENDOR_DIR}/yaml-cpp")
set(BOX2D "${PROJECT_VENDOR_DIR}/box2d")
set(ENTT "${PROJECT_VENDOR_DIR}/entt")
set(IMGUI "${PROJECT_VENDOR_DIR}/imgui")
set(IMGUIZMO "${PROJECT_VENDOR_DIR}/imguizmo")
set(MINIAUDIO "${PROJECT_VENDOR_DIR}/miniaudio")


# Box2d
set(BOX2D_BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(BOX2D_BUILD_TESTBED OFF CACHE BOOL "" FORCE)

# ##############################################################################
# Project executable setup
# ##############################################################################

file(GLOB_RECURSE PROJECT_SOURCES 
        "${PROJECT_SOURCE_DIR}/*.h"
        "${PROJECT_SOURCE_DIR}/*.cpp"
        )

# # Additional project sources (vendors, etc.)
list(APPEND PROJECT_SOURCES
        "${ENTT}/entt.hpp"
        "${SPDLOG}/include/spdlog/spdlog.h"
        "${STB}/stb_image.h"
        "${STB}/stb_image.cpp"
        "${SDL2}/include/SDL.h"
        )

add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC 
        ${PROJECT_SOURCES_DIR}
        ${PROJECT_VENDOR_DIR}

        ${GLM}
        ${SPDLOG}/include
        )

include_directories(SYSTEM src)
include_directories(${IMGUI})
include_directories(${IMGUIZMO})
include_directories(${STB})
include_directories(${SDL2_INCLUDE_DIRS})

if (NOT EMSCRIPTEN)
include_directories(${MSDF-ATLAS-GEN}/msdf-atlas-gen)
endif()

add_subdirectory(${SDL2} EXCLUDE_FROM_ALL)
add_subdirectory(${GLM})

set(MSDF_ATLAS_BUILD_STANDALONE OFF CACHE BOOL "Disable MSDF VCPKG Fetch" FORCE)
set(MSDF_ATLAS_USE_SKIA OFF CACHE BOOL "Disable MSDF VCPKG Fetch" FORCE)
set(MSDF_ATLAS_USE_VCPKG OFF CACHE BOOL "Disable MSDF VCPKG Fetch" FORCE)

if (NOT EMSCRIPTEN)
add_subdirectory(${MSDF-ATLAS-GEN} EXCLUDE_FROM_ALL)
endif()
add_subdirectory(${YAML-CPP})
add_subdirectory(${SPDLOG} EXCLUDE_FROM_ALL)
add_subdirectory(${BOX2D} EXCLUDE_FROM_ALL)
add_subdirectory(${IMGUI})
add_subdirectory(${IMGUIZMO})

if (NOT EMSCRIPTEN AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        imgui
        box2d
        spdlog
        SDL2::SDL2-static
        glm
        yaml-cpp
        imguizmo
        msdf-atlas-gen 
        )
elseif (EMSCRIPTEN)
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        imgui
        box2d
        spdlog
        SDL2::SDL2-static
        glm
        yaml-cpp
        imguizmo
        )
elseif (APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        imgui
        box2d
        spdlog
        SDL2::SDL2-static
        glm
        yaml-cpp
        msdf-atlas-gen 
        imguizmo
        )
endif()

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -DDEBUG -DTRACY_ENABLE")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# Ensure that hot-reload is enabled for VS
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
endif()

target_precompile_headers(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/tbpch.h")
