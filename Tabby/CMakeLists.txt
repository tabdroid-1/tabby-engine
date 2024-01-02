cmake_minimum_required(VERSION 3.22)
project(tabby CXX)
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
set(YAML-CPP "${PROJECT_VENDOR_DIR}/yaml-cpp")
set(BOX2D "${PROJECT_VENDOR_DIR}/box2d")
set(ENTT "${PROJECT_VENDOR_DIR}/entt")
set(IMGUI "${PROJECT_VENDOR_DIR}/imgui")
set(IMGUIZMO "${PROJECT_VENDOR_DIR}/imguizmo")

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
include_directories(${IMGUIZMO})
include_directories(${IMGUI})
include_directories(${STB})
include_directories(${GLM})

add_subdirectory(${GLFW})
add_subdirectory(${GLM})
add_subdirectory(${FREETYPE})
add_subdirectory(${YAML-CPP})
add_subdirectory(${SPDLOG})
add_subdirectory(${BOX2D})
add_subdirectory(${IMGUI})
add_subdirectory(${IMGUIZMO})

target_link_libraries(${PROJECT_NAME} PRIVATE 
        imgui
        box2d
        spdlog
        glfw
        glm
        # glm::glm
        yaml-cpp
        freetype
        imguizmo
        )

# ##############################################################################
# Project build settings
# ##############################################################################

add_definitions(-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})

# Setting ASSETS_PATH
if(CMAKE_BUILD_TYPE MATCHES "Debug")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -DDEBUG")

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

# if(${PLATFORM} STREQUAL "Web")
#   # Tell Emscripten to build an .html file.
#   set(CMAKE_EXECUTABLE_SUFFIX ".html")
#   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os")
#   set(CMAKE_EXE_LINKER_FLAGS
#       "${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -Os -Wall -s TOTAL_MEMORY=67108864 -s FORCE_FILESYSTEM=1 --preload-file assets/ --shell-file ../src/minshell.html"
#   )
#   set(CMAKE_EXECUTABLE_SUFFIX ".html")
#   # This line is used to set your executable to build with the emscripten html
#   # template so that you can directly open it.
#   set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
#   target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="/assets/")
#   # Set the asset path macro in release mode to a relative path that assumes the
#   # assets folder is in the same directory as the game executable
# endif()

# Ensure that hot-reload is enabled for VS
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
endif()

target_precompile_headers(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/tbpch.h")
