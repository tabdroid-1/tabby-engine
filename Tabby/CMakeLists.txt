cmake_minimum_required(VERSION 3.22)
project(tabby CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ##############################################################################
# Add dependencies with FetchContent
# ##############################################################################

# function(add_git_dependency libName gitURL gitTag) FetchContent_Declare(
# ${libName} GIT_REPOSITORY ${gitURL} GIT_TAG ${gitTag} GIT_SHALLOW TRUE
# GIT_PROGRESS TRUE) FetchContent_MakeAvailable(${libName})
# target_compile_options(${libName} PRIVATE "-w") endfunction()
set(PROJECT_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/src")
set(PROJECT_VENDOR_DIR "${CMAKE_CURRENT_LIST_DIR}/vendor")

set(RAYLIB "${PROJECT_VENDOR_DIR}/raylib")
set(BOX2D "${PROJECT_VENDOR_DIR}/box2d")
set(RAYLIBTMX "${PROJECT_VENDOR_DIR}/raylib-tmx")
set(LDTKLOADER "${PROJECT_VENDOR_DIR}/LDtkLoader")
set(FMT "${PROJECT_VENDOR_DIR}/fmt")
set(ENTT "${PROJECT_VENDOR_DIR}/entt")

# Add Raylib
set(BUILD_EXAMPLES
    OFF
    CACHE BOOL "" FORCE) # don't build the supplied examples
set(BUILD_GAMES
    OFF
    CACHE BOOL "" FORCE) # don't build the supplied example games
# Add Box2d
set(BOX2D_BUILD_UNIT_TESTS
    OFF
    CACHE BOOL "" FORCE)
set(BOX2D_BUILD_TESTBED
    OFF
    CACHE BOOL "" FORCE)

# ##############################################################################
# Project executable setup
# ##############################################################################

# Adding our source files Define PROJECT_SOURCES as a list of all source files
# file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
#      "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/src/*.h"
#      "${CMAKE_CURRENT_LIST_DIR}/src/*.hpp")

file(GLOB_RECURSE PROJECT_SOURCES
        "${PROJECT_SOURCE_DIR}/*.h"
        "${PROJECT_SOURCE_DIR}/*.cpp"
        )

# Define PROJECT_INCLUDE to be the path to the include directory of the project
set(PROJECT_INCLUDE 
    "${CMAKE_CURRENT_LIST_DIR}/src/"
    "${ENTT}/"
    )

# Additional project sources (vendors, etc.)
set(HEADERS
    "${ENTT}/entt.hpp"
    # Add more header files here
)

# Declaring our executable
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${HEADERS})
target_sources(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_INCLUDE})

add_subdirectory(${RAYLIB})
add_subdirectory(${RAYLIBTMX})
add_subdirectory(${LDTKLOADER})
add_subdirectory(${BOX2D})
add_subdirectory(${FMT})

target_link_libraries(${PROJECT_NAME} PUBLIC raylib)
target_link_libraries(${PROJECT_NAME} PUBLIC raylib-tmx)
target_link_libraries(${PROJECT_NAME} PUBLIC LDtkLoader)
target_link_libraries(${PROJECT_NAME} PUBLIC box2d)
target_link_libraries(${PROJECT_NAME} PUBLIC fmt)

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

if(${PLATFORM} STREQUAL "Web")
  # Tell Emscripten to build an .html file.
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os")
  set(CMAKE_EXE_LINKER_FLAGS
      "${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -Os -Wall -s TOTAL_MEMORY=67108864 -s FORCE_FILESYSTEM=1 --preload-file assets/ --shell-file ../src/minshell.html"
  )
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
  # This line is used to set your executable to build with the emscripten html
  # template so that you can directly open it.
  set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
  target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="/assets/")
  # Set the asset path macro in release mode to a relative path that assumes the
  # assets folder is in the same directory as the game executable
endif()

# Ensure that hot-reload is enabled for VS
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
endif()

target_precompile_headers(${PROJECT_NAME} PRIVATE
                          "${PROJECT_SOURCE_DIR}/tbpch.h")
