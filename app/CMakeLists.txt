cmake_minimum_required(VERSION 3.20)
project(app)
set(CMAKE_CXX_STANDARD 17)

###################################################################
# Local variables
###################################################################
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(PROJECT_SRC_DIR "src")
set(PROJECT_ASSETS_DIR "${CMAKE_CURRENT_LIST_DIR}/assets")

# Libraries

if (ANDROID)
    set(TABBY_DIR "${CMAKE_SOURCE_DIR}/../tabby")
    set(TABBY_LIB_DIR "${CMAKE_BINARY_DIR}/tabby")
else()
    set(TABBY_DIR "${CMAKE_SOURCE_DIR}/tabby")
    set(TABBY_LIB_DIR "${CMAKE_BINARY_DIR}/tabby")
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
        )
    include_directories("${TABBY_DIR}/vendor/glm")
    include_directories("${TABBY_DIR}/vendor/spdlog/include")
    include_directories("${TABBY_DIR}/vendor")

    add_subdirectory(
        ../tabby
        ./tabby
        )
else()
    add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
endif()
# Link libraries
target_link_libraries(${PROJECT_NAME}
        tabby
        )

include_directories(SYSTEM src)
include_directories(${TABBY_DIR}/src)

# Include directories
target_include_directories(${PROJECT_NAME} PUBLIC
        ${PROJECT_ASSETS_DIR}
        ${PROJECT_SOURCE_DIR}

        "${TABBY_DIR}/vendor/box2c/include"
        )

if (EMSCRIPTEN)

    # target_link_options(${PROJECT_NAME} PUBLIC -sGL_PREINITIALIZED_CONTEXT=1)
    # target_link_options(${PROJECT_NAME} PUBLIC -sOFFSCREEN_FRAMEBUFFER=1)
    # target_link_options(${PROJECT_NAME} PUBLIC -sMIN_WEBGL_VERSION=2)
    # target_link_options(${PROJECT_NAME} PUBLIC -sMAX_WEBGL_VERSION=2)
    # target_link_options(${PROJECT_NAME} PUBLIC -sEXIT_RUNTIME=1)
    # target_link_options(${PROJECT_NAME} PUBLIC -sUSE_SDL)
    # target_link_options(${PROJECT_NAME} PUBLIC -sFORCE_FILESYSTEM=1)
    # target_link_options(${PROJECT_NAME} PUBLIC -sFETCH=1)
    # target_link_options(${PROJECT_NAME} PUBLIC -sASYNCIFY)
    # target_link_options(${PROJECT_NAME} PUBLIC -sASSERTIONS=1)
    # target_link_options(${PROJECT_NAME} PUBLIC -sINVOKE_RUN=0)
    # target_link_options(${PROJECT_NAME} PUBLIC -O2)
    # target_link_options(${PROJECT_NAME} PUBLIC -FULL_ES3=1)
    # target_link_options(${PROJECT_NAME} PUBLIC -sMIN_FIREFOX_VERSION=79)
    # target_link_options(${PROJECT_NAME} PUBLIC -sMIN_SAFARI_VERSION=150000)
    # target_link_options(${PROJECT_NAME} PUBLIC -sMIN_IE_VERSION=0x7FFFFFFF)
    # target_link_options(${PROJECT_NAME} PUBLIC -sMIN_CHROME_VERSION=80)
    # target_link_options(${PROJECT_NAME} PUBLIC -lGL)
    # target_link_options(${PROJECT_NAME} PUBLIC -lopenal)

	append_linker_flags("-sOFFSCREEN_FRAMEBUFFER=1")
    append_linker_flags("-sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2")
    append_linker_flags("-sUSE_SDL=2")
    append_linker_flags("-FULL_ES3=1")
    append_linker_flags("-lGL")
    append_linker_flags("-lopenal")
    append_linker_flags("-sFORCE_FILESYSTEM=1")
    append_linker_flags("-sFETCH=1")
    append_linker_flags("-sASSERTIONS=1")
    append_linker_flags("--preload-file assets")
	append_linker_flags("--use-port=sdl2")
	# append_linker_flags("-sWASM_WORKERS")
    
    target_link_options(${PROJECT_NAME} PUBLIC -pthread)
    target_compile_options(${PROJECT_NAME} PUBLIC -pthread)
 
	append_linker_flags("-Oz")
	append_linker_flags("-sMIN_FIREFOX_VERSION=79")
	append_linker_flags("-sMIN_SAFARI_VERSION=150000")
	append_linker_flags("-sMIN_IE_VERSION=0x7FFFFFFF") # Do not support Internet Explorer at all (this is the Emscripten default, shown here for posterity)
	# append_linker_flags("-sMIN_EDGE_VERSION=79")
	append_linker_flags("-sMIN_CHROME_VERSION=80")

    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    # This line is used to set your executable to build with the emscripten html
    # template so that you can directly open it.
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
    target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="/assets/")
    # set(CMAKE_EXECUTABLE_SUFFIX ".wasm")
  # Set the asset path macro in release mode to a relative path that assumes the
  # assets folder is in the same directory as the game executable
endif (EMSCRIPTEN)

###################################################################
# Copy executable dependencies to CMake runtime output directory
###################################################################
# if (WIN32)
#     # Copy library dlls -------------------------------------------
#     file(GLOB_RECURSE LIB_DLLS
#             "${TABBY_LIB_DIR}/*.dll"
#             )
#
#     foreach (dll ${LIB_DLLS})
#         file(COPY ${dll}
#                 DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
#     endforeach (dll)
#     # -------------------------------------------------------------
# endif ()


add_custom_target(copy_assets_sandbox
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_ASSETS_DIR} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets
        )

add_dependencies(${PROJECT_NAME} copy_assets_sandbox)
# -----------------------------------------------------------------
