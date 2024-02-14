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

if (ANDROID)
    set(TABBY_DIR "${CMAKE_SOURCE_DIR}/../Tabby")
    set(TABBY_LIB_DIR "${CMAKE_BINARY_DIR}/Tabby")
else()
    set(TABBY_DIR "${CMAKE_SOURCE_DIR}/Tabby")
    set(TABBY_LIB_DIR "${CMAKE_BINARY_DIR}/Tabby")
endif()

macro(append_linker_flags FLAGS)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${FLAGS}")
endmacro()
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
if (ANDROID)
    add_library(${PROJECT_NAME} SHARED 
        ${PROJECT_SOURCES}
        # ${TABBY_DIR}/vendor/SDL2/src/main/android/SDL_android_main.c
        )
    include_directories("${TABBY_DIR}/vendor/glm")
    include_directories("${TABBY_DIR}/vendor/spdlog/include")
    include_directories("${TABBY_DIR}/vendor")

    add_subdirectory(
        "../Tabby"
        "./Tabby"
        )
else()
    add_executable(${PROJECT_NAME} "${PROJECT_SOURCES}")
endif()
# Link libraries
target_link_libraries(${PROJECT_NAME}
        "tabby"
        )

include_directories(SYSTEM "src")
include_directories("${TABBY_DIR}/src")

# Include directories
target_include_directories(${PROJECT_NAME} PUBLIC
        "${PROJECT_ASSETS_DIR}"
        "${PROJECT_SOURCE_DIR}"

        "${TABBY_DIR}/vendor/box2d/include"
        )

if (EMSCRIPTEN)
	append_linker_flags("-sOFFSCREEN_FRAMEBUFFER=1")
	append_linker_flags("-sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2")
    append_linker_flags("-sUSE_SDL=2")
    append_linker_flags("-FULL_ES3=1")
	append_linker_flags("-lGL")
    append_linker_flags("-sFORCE_FILESYSTEM=1")
	append_linker_flags("-sFETCH=1")
	append_linker_flags("-sASYNCIFY")
	append_linker_flags("-sASSERTIONS=1")
    append_linker_flags("--preload-file assets/shaders/gles3/Renderer2D_Quad.glsl")
    append_linker_flags("--preload-file assets/shaders/gles3/Renderer2D_Line.glsl")
    append_linker_flags("--preload-file assets/shaders/gles3/Renderer2D_Circle.glsl")
    append_linker_flags("--preload-file assets/shaders/gles3/Renderer2D_Text.glsl")
    append_linker_flags("--preload-file assets/spritesheets/player/player_packed.png")

	append_linker_flags("-Oz")
	append_linker_flags("-sMIN_FIREFOX_VERSION=70")
	append_linker_flags("-sMIN_SAFARI_VERSION=130000")
	append_linker_flags("-sMIN_IE_VERSION=0x7FFFFFFF") # Do not support Internet Explorer at all (this is the Emscripten default, shown here for posterity)
	# append_linker_flags("-sMIN_EDGE_VERSION=79") # Require Chromium-based Edge browser
	append_linker_flags("-sMIN_CHROME_VERSION=80")

endif (EMSCRIPTEN)

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
