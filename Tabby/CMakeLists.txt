cmake_minimum_required(VERSION 3.20)
project(Tabby)
set(CMAKE_CXX_STANDARD 17)
if(APPLE)
    enable_language(OBJC)
    enable_language(OBJCXX)
    message(STATUS "Including Objective-C++ for APPLE")
    list(APPEND LANGUAGES OBJC)
    list(APPEND LANGUAGES OBJCXX)
    set(CMAKE_OBJCXX_STANDARD 14)
    set(CMAKE_OBJCXX_STANDARD_REQUIRED ON)
    set(CMAKE_OBJCXX_EXTENSIONS OFF)
endif()

###################################################################
# Local variables
###################################################################
set(PROJECT_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/src")
set(PROJECT_VENDOR_DIR "${CMAKE_CURRENT_LIST_DIR}/vendor")

find_package(PkgConfig REQUIRED)

# Vendor/Libraries
set(BOX2D "${PROJECT_VENDOR_DIR}/box2D")
set(IMGUI "${PROJECT_VENDOR_DIR}/imgui")
set(GLAD "${PROJECT_VENDOR_DIR}/glad")
set(GLFW "${PROJECT_VENDOR_DIR}/glfw")
set(GLM "${PROJECT_VENDOR_DIR}/glm")
set(SPDLOG "${PROJECT_VENDOR_DIR}/spdlog")
set(STB "${PROJECT_VENDOR_DIR}/stb_image")
set(ENTT "${PROJECT_VENDOR_DIR}/entt")
set(YAML "${PROJECT_VENDOR_DIR}/yaml-cpp")
set(IMGUIZMO "${PROJECT_VENDOR_DIR}/ImGuizmo")

###################################################################
# Project Sources and add library
###################################################################
file(GLOB_RECURSE PROJECT_SOURCES
        "${PROJECT_SOURCE_DIR}/*.h"
        "${PROJECT_SOURCE_DIR}/*.cpp"
        )

include_directories(${IMGUI})
include_directories(${STB})
include_directories(${ENTT})
include_directories(${IMGUIZMO})
include_directories(${GLFW})

# Additional project sources (vendors, etc.)
list(APPEND PROJECT_SOURCES
        # Vendor sources --------------------------
        "${STB}/stb_image.h"
        "${STB}/stb_image.cpp"
        # -----------------------------------------
        )

# Add library
add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME} PUBLIC
        "${PROJECT_SOURCE_DIR}"
        "${PROJECT_VENDOR_DIR}"
        )

###################################################################
# Libraries
###################################################################
add_subdirectory(${SPDLOG})
add_subdirectory(${IMGUIZMO})
add_subdirectory(${GLM})
add_subdirectory(${IMGUI})
add_subdirectory(${YAML})

#BOX2D-------------------------------------------
set(BOX2D_BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(BOX2D_BUILD_TESTBED OFF CACHE BOOL "" FORCE)
add_subdirectory(${BOX2D})
#GLFW--------------------------------------------
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${GLFW})
#----------------------------------------------

target_link_libraries(${PROJECT_NAME} PUBLIC
        spdlog
        glfw
        glm::glm
        imgui
        box2d
        glm
        imguizmo
		yaml-cpp
        )

###################################################################
# Compiler options
###################################################################
target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_INCLUDE_NONE=1 Dosmesa=true)

if(WIN32)

    target_compile_definitions(${PROJECT_NAME} PUBLIC TB_DEBUG=1 TB_PLATFORM_WINDOWS=1 TB_BUILD_DLL=1 BUILD_SHARED_LIBS=1)

    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    include_directories(GL)

endif(WIN32)

###################################################################
# Precompiled header
###################################################################
target_precompile_headers(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/tbpch.h")
