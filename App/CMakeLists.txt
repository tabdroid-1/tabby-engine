cmake_minimum_required(VERSION 3.20)
project(App)
set(CMAKE_CXX_STANDARD 17)

###################################################################
# Local variables
###################################################################
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(PROJECT_SRC_DIR "src")
set(PROJECT_ASSETS_DIR "${CMAKE_CURRENT_LIST_DIR}/assets")

# Libraries
set(TABBY_DIR "${CMAKE_SOURCE_DIR}/Tabby")
set(TABBY_LIB_DIR "${CMAKE_BINARY_DIR}/Tabby")

###################################################################
# Target sources and create executable
###################################################################
file(GLOB_RECURSE PROJECT_SOURCES
        "${PROJECT_SRC_DIR}/*.h"
        "${PROJECT_SRC_DIR}/*.cpp"
        )

list(APPEND PROJECT_SOURCES

        )

# Create executable
add_executable(${PROJECT_NAME} "${PROJECT_SOURCES}")

# Link libraries
target_link_libraries(${PROJECT_NAME}
        "tabby"
        )

include_directories(SYSTEM "src")
include_directories("${TABBY_DIR}/src")

# Include directories
target_include_directories(${PROJECT_NAME} PUBLIC
        "${PROJECT_ASSETS_DIR}"
        "minshell.html"
        "${PROJECT_SOURCE_DIR}"

        "${TABBY_DIR}/vendor/box2d/include"
        )

###################################################################
# Copy executable dependencies to CMake runtime output directory
###################################################################
if (WIN32)
    # Copy library dlls -------------------------------------------
    file(GLOB_RECURSE LIB_DLLS
            "${TABBY_LIB_DIR}/*.dll"
            )

    foreach (dll ${LIB_DLLS})
        file(COPY ${dll}
                DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    endforeach (dll)
    # -------------------------------------------------------------
endif ()

if (DEFINED EMSCRIPTEN)
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os -sGL_ENABLE_GET_PROC_ADDRESS=1")
    # set(CMAKE_EXE_LINKER_FLAGS
    #   "${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 -s USE_WEBGL2=1 -s FULL_ES3=1 -s ASSERTIONS=1 -s WASM=1 -Os -Wall -s TOTAL_MEMORY=67108864 -s FORCE_FILESYSTEM=1 --preload-file assets/ --shell-file ../src/minshell.html"
    # )
target_link_options(${PROJECT_NAME} PRIVATE
   "-s USE_GLFW=3"
   "-s FULL_ES3=1"
   "-s MIN_WEBGL_VERSION=2"
   "-s MAX_WEBGL_VERSION=2"
   "-s WASM=1"
   "-s NO_DISABLE_EXCEPTION_CATCHING=1"
   "-s ALLOW_MEMORY_GROWTH=1"
   "-Os"
   "-sGL_ENABLE_GET_PROC_ADDRESS=1"
   "-Wall"
   "-s GL_SUPPORT_AUTOMATIC_ENABLE_EXTENSIONS=1"
    "-s --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/src/minshell.html"
)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    # This line is used to set your executable to build with the emscripten html
    # template so that you can directly open it.
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
    target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="/assets/")
    # set(CMAKE_EXECUTABLE_SUFFIX ".wasm")
  # Set the asset path macro in release mode to a relative path that assumes the
  # assets folder is in the same directory as the game executable

endif (DEFINED EMSCRIPTEN)
add_custom_target(copy_assets_sandbox
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_ASSETS_DIR} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets
        )

add_dependencies(${PROJECT_NAME} copy_assets_sandbox)
# -----------------------------------------------------------------
