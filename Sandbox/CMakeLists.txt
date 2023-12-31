cmake_minimum_required(VERSION 3.20)
project(Sandbox)
set(CMAKE_CXX_STANDARD 17)

###################################################################
# Local variables
###################################################################
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(PROJECT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/src")
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

if (WIN32)
    # Include glad w/ executable for linker
    list(APPEND PROJECT_SOURCES
            "${TABBY_DIR}/vendor/Glad/src/glad.c"
            "${TABBY_DIR}/vendor/Glad/include/glad/glad.h"
            )
endif ()

# Create executable
add_executable(${PROJECT_NAME} "${PROJECT_SOURCES}")

# Link libraries
target_link_libraries(${PROJECT_NAME}
        "Tabby"
        )

# Include directories
target_include_directories(${PROJECT_NAME} PUBLIC
        "${PROJECT_ASSETS_DIR}"
        "${PROJECT_SOURCE_DIR}"

        "${TABBY_LIB_DIR}/src"
        "${TABBY_LIB_DIR}/vendor"
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

add_custom_target(copy_assets_sandbox
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_ASSETS_DIR} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets
        )

add_dependencies(${PROJECT_NAME} copy_assets_sandbox)
# -----------------------------------------------------------------
