cmake_minimum_required(VERSION 3.22)
project(tabby
	VERSION 0.0.1
    DESCRIPTION "Game engine"
	HOMEPAGE_URL "https://github.com/tabdroid-1/TabbyEngine"
    LANGUAGES CXX
)

if(APPLE)
    message(STATUS "Including Objective-C++ for APPLE")
    list(APPEND LANGUAGES OBJC)
    list(APPEND LANGUAGES OBJCXX)
    set(CMAKE_OBJCXX_STANDARD 14)
    set(CMAKE_OBJCXX_STANDARD_REQUIRED ON)
    set(CMAKE_OBJCXX_EXTENSIONS OFF)
endif()

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
# set(BOX2D "${PROJECT_VENDOR_DIR}/box2d")
set(BOX2C "${PROJECT_VENDOR_DIR}/box2c")
set(ENTT "${PROJECT_VENDOR_DIR}/entt")
set(IMGUI "${PROJECT_VENDOR_DIR}/imgui")
set(IMGUIZMO "${PROJECT_VENDOR_DIR}/imguizmo")
set(TRACY "${PROJECT_VENDOR_DIR}/tracy")
set(OPENLAL "${PROJECT_VENDOR_DIR}/openal-soft")
set(LIBSNDFILE "${PROJECT_VENDOR_DIR}/libsndfile")
set(TINYOBJLOADER "${PROJECT_VENDOR_DIR}/tinyobjloader")
set(TINYGLTF "${PROJECT_VENDOR_DIR}/tinygltf")

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
        "${TINYOBJLOADER}/tiny_obj_loader.cc"
        "${TINYOBJLOADER}/tiny_obj_loader.h"
        "${TINYGLTF}/tiny_gltf.h"
        "${TINYGLTF}/tiny_gltf.cpp"
        )


add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC 
        ${PROJECT_SOURCES_DIR}
        ${PROJECT_VENDOR_DIR}

        ${GLM}
        ${SPDLOG}/include
        ${OPENLAL}/include
        ${LIBSNDFILE}/include
        ${BOX2C}/include
        # ${ASSIMP}/include
        )

include_directories(SYSTEM src)
include_directories(${IMGUI})
include_directories(${IMGUIZMO})
include_directories(${STB})
include_directories(${SDL2_INCLUDE_DIRS})

include_directories(${MSDF-ATLAS-GEN}/msdf-atlas-gen)
if (NOT EMSCRIPTEN AND NOT ANDROID)
    include_directories(${TRACY})
endif()

# ####################
# Add Subdirectories
# ####################

add_subdirectory(${SDL2} EXCLUDE_FROM_ALL)
add_subdirectory(${GLM})

set(MSDF_ATLAS_BUILD_STANDALONE OFF CACHE BOOL "Disable building MSDF Standalone executable" FORCE)
set(MSDF_ATLAS_USE_SKIA OFF CACHE BOOL "Disable Skia from MSDF" FORCE)
set(MSDF_ATLAS_USE_VCPKG OFF CACHE BOOL "Disable MSDF VCPKG Fetch" FORCE)
set(MSDFGEN_DISABLE_SVG ON CACHE BOOL "Disable MSDF VCPKG Fetch" FORCE)
set(MSDFGEN_DISABLE_PNG ON CACHE BOOL "Disable MSDF VCPKG Fetch" FORCE)

add_subdirectory(${MSDF-ATLAS-GEN} EXCLUDE_FROM_ALL)
if (NOT EMSCRIPTEN AND NOT ANDROID)
    add_subdirectory(${TRACY})
endif()
add_subdirectory(${YAML-CPP})
add_subdirectory(${SPDLOG})

# Box2d
set(BOX2D_SAMPLES OFF CACHE BOOL "" FORCE)
set(BOX2D_DOCS OFF CACHE BOOL "" FORCE)
set(BOX2D_PROFILE OFF CACHE BOOL "" FORCE)
add_subdirectory(${BOX2C})

add_subdirectory(${IMGUI})
add_subdirectory(${IMGUIZMO})

add_subdirectory(${LIBSNDFILE})

if (WIN32)

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /experimental:c11atomics)
endif()
    message(STATUS "Generating scripts for Windows")
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        imgui
        box2d
        spdlog
        SDL2::SDL2-static
        glm
        yaml-cpp
        imguizmo
        msdf-atlas-gen 
        tracy 
        )
elseif (UNIX AND NOT APPLE AND NOT ANDROID AND NOT EMSCRIPTEN)

    find_package(OpenAL)

    message(STATUS "Generating scripts for Linux")
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        imgui
        box2d
        spdlog
        SDL2::SDL2-static
        glm
        yaml-cpp
        imguizmo
        msdf-atlas-gen 
        tracy 
        OpenAL::OpenAL
        sndfile
        )
elseif (APPLE)
    message(STATUS "Generating scripts for Mac")
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        tracy 
        imgui
        box2d
        spdlog
        SDL2::SDL2-static
        glm
        yaml-cpp
        msdf-atlas-gen 
        imguizmo
        )
elseif (EMSCRIPTEN)
    message(STATUS "Generating scripts for EMSCRIPTEN")
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        imgui
        box2d
        spdlog
        SDL2::SDL2-static
        glm
        yaml-cpp
        imguizmo
        )
elseif (ANDROID)
    message(STATUS "Generating scripts for Android")
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        imgui
        box2d
        spdlog
        SDL2::SDL2
        SDL2::SDL2main
        glm
        yaml-cpp
        imguizmo
        GLESv2
        GLESv3
        EGL
        log
        android
        )
endif()

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    if(EMSCRIPTEN)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -DDEBUG -DTB_PROFILE=1")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -DDEBUG -DTRACY_ENABLE=1 -DTB_DEBUG=1 -DTB_PROFILE=1")
    endif()
    target_compile_definitions(${PROJECT_NAME} PUBLIC TRACY_ENABLE=1 )
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# Ensure that hot-reload is enabled for VS
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
endif()

target_precompile_headers(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/tbpch.h")
