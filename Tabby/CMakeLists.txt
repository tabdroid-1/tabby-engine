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

	set(CMAKE_EXECUTABLE_SUFFIX ".html")
    # set(GLFW_VULKAN_STATIC OFF CACHE BOOL "Assume Vulkan is a dynamic library" FORCE)
    # set(GLFW_USE_OSMESA OFF CACHE BOOL "Use OSMesa for offscreen context creation" FORCE)
    # set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "Use Wayland for window creation" FORCE)
    set(GLFW_BUILD_X11 OFF CACHE BOOL "Use X11 for window creation" FORCE)
    # set(GLFW_BUILD_SHARED_LIBS OPT_BUILD_SHARED_LIBS CACHE BOOL "Build GLFW as shared library.")
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Do not build GLFW examples.")
    set(GLFW_BUILD_TESTS    OFF CACHE BOOL "Do not run GLFW tests.")
    set(GLFW_BUILD_DOCS     OFF CACHE BOOL "Do not build GLFW documentation.")
    # set(GLFW_INSTALL        OFF CACHE BOOL "Do not generate install targets.")
    find_package(glfw3)

    append_linker_flags("-USE_GLFW=3")
    append_linker_flags("-FULL_ES3=1")
	append_linker_flags("-sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2")
	append_linker_flags("-lGL")
	append_linker_flags("-sFORCE_FILESYSTEM=1")
	append_linker_flags("-sFETCH=1")
	append_linker_flags("-sASYNCIFY=1")
	# append_linker_flags("-sMINIMAL_RUNTIME=2")
	append_linker_flags("-Oz")
	append_linker_flags("-sENVIRONMENT=web")

	add_definitions(-DMAX_WEBGL_VERSION=2)

endif (EMSCRIPTEN)


set(PROJECT_SOURCE_DIR "src")
set(PROJECT_VENDOR_DIR "vendor")

# ##############################################################################
# Add dependencies 
# ##############################################################################

set(GLFW "${PROJECT_VENDOR_DIR}/glfw")
set(GLAD "${PROJECT_VENDOR_DIR}/glad")
set(GLM "${PROJECT_VENDOR_DIR}/glm")
set(STB "${PROJECT_VENDOR_DIR}/stb")
set(SPDLOG "${PROJECT_VENDOR_DIR}/spdlog")
set(FREETYPE "${PROJECT_VENDOR_DIR}/freetype")
set(MSDF-ATLAS-GEN "${PROJECT_VENDOR_DIR}/msdf-atlas-gen")
set(YAML-CPP "${PROJECT_VENDOR_DIR}/yaml-cpp")
set(BOX2D "${PROJECT_VENDOR_DIR}/box2d")
set(ENTT "${PROJECT_VENDOR_DIR}/entt")
set(IMGUI "${PROJECT_VENDOR_DIR}/imgui")
set(IMGUIZMO "${PROJECT_VENDOR_DIR}/imguizmo")
set(MINIAUDIO "${PROJECT_VENDOR_DIR}/miniaudio")


set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build GLFW examples" FORCE)
# Box2d
set(BOX2D_BUILD_UNIT_TESTS
    OFF
    CACHE BOOL "" FORCE)
set(BOX2D_BUILD_TESTBED
    OFF
    CACHE BOOL "" FORCE)

# ##############################################################################
# Project executable setup
# ##############################################################################

file(GLOB_RECURSE PROJECT_SOURCES 
        "${PROJECT_SOURCE_DIR}/*.h"
        "${PROJECT_SOURCE_DIR}/*.cpp"
        )

# # Additional project sources (vendors, etc.)
list(APPEND PROJECT_SOURCES
        "${ENTT}/entt.hpp" # entity component system

        "${SPDLOG}/include/spdlog/spdlog.h" # For logging
        "${STB}/stb_image.h"
        "${STB}/stb_image.cpp"
        )


# Declaring our executable
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
include_directories(${MSDF-ATLAS-GEN}/msdf-atlas-gen)

if (NOT EMSCRIPTEN)
add_subdirectory(${GLFW})
endif()
add_subdirectory(${GLM})

set(MSDF_ATLAS_BUILD_STANDALONE OFF CACHE BOOL "Disable MSDF VCPKG Fetch" FORCE)
set(MSDF_ATLAS_USE_SKIA OFF CACHE BOOL "Disable MSDF VCPKG Fetch" FORCE)
if (NOT WINDOWS)
set(MSDF_ATLAS_USE_VCPKG OFF CACHE BOOL "Disable MSDF VCPKG Fetch" FORCE)
endif()
add_subdirectory(${MSDF-ATLAS-GEN})
add_subdirectory(${YAML-CPP})
add_subdirectory(${SPDLOG})
add_subdirectory(${BOX2D})
add_subdirectory(${IMGUI})
add_subdirectory(${IMGUIZMO})

if (NOT EMSCRIPTEN AND NOT APPLE)
target_link_libraries(${PROJECT_NAME} PRIVATE 
        imgui
        box2d
        spdlog
        glfw
        glm
        yaml-cpp
        imguizmo
        msdf-atlas-gen 
        )
elseif (EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
    target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="/assets/")

    target_link_libraries(${PROJECT_NAME} PRIVATE 
        imgui
        box2d
        spdlog
        glfw3
        glm
        yaml-cpp
        imguizmo
        msdf-atlas-gen 
        )
elseif (APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        imgui
        box2d
        spdlog
        glfw
        glm
        yaml-cpp
        msdf-atlas-gen 
        imguizmo
        )
endif()

# Setting ASSETS_PATH
if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -DDEBUG -DTRACY_ENABLE")

  # Set the asset path macro to the absolute path on the dev machine
    target_compile_definitions(
        ${PROJECT_NAME} PUBLIC ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/")
else()
# Set the asset path macro in release mode to a relative path that assumes the
# assets folder is in the same directory as the game executable
    target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="./assets/")
endif()

# Set common compiler flags SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall
# -Wswitch")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")




# Ensure that hot-reload is enabled for VS
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
endif()

target_precompile_headers(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/tbpch.h")
