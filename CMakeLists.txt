cmake_minimum_required(VERSION 3.22)
project(tabby
	VERSION 0.0.1
    DESCRIPTION "Game engine"
	HOMEPAGE_URL "https://github.com/tabdroid-1/TabbyEngine"
    LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 17)

if(APPLE)
    message(STATUS "Including Objective-C++ for APPLE")
    list(APPEND LANGUAGES OBJC)
    list(APPEND LANGUAGES OBJCXX)
    set(CMAKE_OBJCXX_STANDARD 14)
    set(CMAKE_OBJCXX_STANDARD_REQUIRED ON)
    set(CMAKE_OBJCXX_EXTENSIONS OFF)
endif()


option(TB_DIST "Enable distrubition build" OFF)
option(TB_PROFILE "Enable profiling" OFF)
option(TB_HEADLESS "Enable headless mode" OFF)
option(TB_NETWORKING "Enable networking" OFF)

add_library(tabby_flags INTERFACE)

if(TB_DIST)
    # set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build utility programs" FORCE)
    set(TB_PROFILE OFF CACHE BOOL "" FORCE)
endif(TB_DIST)

if(TB_PROFILE)
    target_compile_definitions(tabby_flags INTERFACE 
        TB_PROFILE=1 
        TRACY_ENABLE=1
        )
    add_compile_definitions(
        TB_PROFILE=1 
        TRACY_ENABLE=1
        )
    message(WARNING "Tabby: Extremely high memory usage when profiler is enabled")
endif(TB_PROFILE)

if(TB_HEADLESS)
    target_compile_definitions(tabby_flags INTERFACE TB_HEADLESS=1)
    add_compile_definitions(TB_HEADLESS=1)
else()
    target_compile_definitions(tabby_flags INTERFACE TB_HEADLESS=0)
    add_compile_definitions(TB_HEADLESS=0)
endif(TB_HEADLESS)

set(ALSOFT_UTILS OFF CACHE BOOL "Build utility programs" FORCE)
set(ALSOFT_NO_CONFIG_UTIL OFF CACHE BOOL "Disable building the alsoft-config utility" FORCE)
set(ALSOFT_EXAMPLES OFF CACHE BOOL "Build example programs" FORCE)
set(ALSOFT_TESTS OFF CACHE BOOL "Build test programs" FORCE)
set(ALSOFT_INSTALL  OFF CACHE BOOL "Install main library" FORCE)
set(ALSOFT_INSTALL_CONFIG CACHE BOOL "Install alsoft.conf sample configuration file" FORCE)
set(ALSOFT_INSTALL_HRTF_DATA CACHE BOOL "Install HRTF data files" FORCE)
set(ALSOFT_INSTALL_AMBDEC_PRESETS CACHE BOOL "Install AmbDec preset files" FORCE)
set(ALSOFT_INSTALL_EXAMPLES BOOL "Install example programs (alplay, alstream, ...)" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
set(SDL_SHARED OFF CACHE BOOL "Build a shared version of the library" FORCE)
set(SDL_STATIC ON CACHE BOOL "Build a static version of the library" FORCE)
set(SDL_TEST OFF CACHE BOOL "Build the SDL2_test library" FORCE)
set(ENKITS_BUILD_C_INTERFACE OFF CACHE BOOL "Build C interface" FORCE)
set(ENKITS_BUILD_EXAMPLES OFF CACHE BOOL "Build example applications" FORCE)
set(BOX2D_SAMPLES OFF CACHE BOOL "" FORCE)
set(BOX2D_DOCS OFF CACHE BOOL "" FORCE)
set(BOX2D_PROFILE OFF CACHE BOOL "" FORCE)
set(BOX2D_BUILD_UNIT_TESTS OFF CACHE BOOL "Build the Box2D unit tests" FORCE)
set(BOX2D_BUILD_TESTBED OFF CACHE BOOL "Build the Box2D testbed" FORCE)
set(MSDF_ATLAS_BUILD_STANDALONE OFF CACHE BOOL "Disable building MSDF Standalone executable" FORCE)
set(MSDF_ATLAS_USE_VCPKG OFF CACHE BOOL "Disable MSDF VCPKG Fetch" FORCE)
set(MSDF_ATLAS_USE_SKIA OFF CACHE BOOL "Disable Skia from MSDF" FORCE)
set(MSDFGEN_DISABLE_SVG ON CACHE BOOL "Disable MSDF VCPKG Fetch" FORCE)
set(MSDFGEN_DISABLE_PNG ON CACHE BOOL "Disable MSDF VCPKG Fetch" FORCE)
set(MSDF_ATLAS_NO_ARTERY_FONT OFF CACHE BOOL "Disable Artery Font export and do not require its submodule" FORCE)
set(MSDF_ATLAS_MSDFGEN_EXTERNAL OFF CACHE BOOL "Do not build the msdfgen submodule but find it as an external package" FORCE)
set(MSDF_ATLAS_INSTALL OFF CACHE BOOL "Generate installation target" FORCE)
set(MSDF_ATLAS_DYNAMIC_RUNTIME OFF CACHE BOOL "Link dynamic runtime library instead of static" FORCE)
set(MSDFGEN_CORE_ONLY OFF CACHE BOOL "Only build the core library with no dependencies" FORCE)
set(MSDFGEN_BUILD_STANDALONE OFF CACHE BOOL "Build the msdfgen standalone executable" FORCE)
set(MSDFGEN_USE_VCPKG OFF CACHE BOOL "Use vcpkg package manager to link project dependencies" FORCE)
set(MSDFGEN_USE_OPENMP OFF CACHE BOOL "Build with OpenMP support for multithreaded code" FORCE)
set(MSDFGEN_USE_CPP11 ON CACHE BOOL "Build with C++11 enabled" FORCE)
set(MSDFGEN_USE_SKIA ON CACHE BOOL "Build with the Skia library" FORCE)
set(MSDFGEN_INSTALL  OFF CACHE BOOL "Generate installation target" FORCE)
set(MSDFGEN_DYNAMIC_RUNTIME OFF CACHE BOOL "Link dynamic runtime library instead of static" FORCE)
set(SPDLOG_HEADER_ONLY OFF CACHE BOOL "" FORCE)
set(VMA_ENABLE_INSTALL OFF CACHE BOOL "Install VulkanMemoryAllocator" FORCE)
set(VK_BOOTSTRAP_TEST OFF CACHE BOOL "Test Vk-Bootstrap using Catch2 as well as build examples" FORCE)
set(VK_BOOTSTRAP_INSTALL OFF CACHE BOOL "Enable installing of vk-bootstrap" FORCE)

set(PROJECT_SOURCE_DIR "src")
set(PROJECT_VENDOR_DIR "vendor")

# ##############################################################################
# Add dependencies 
# ##############################################################################

set(GLM ${PROJECT_VENDOR_DIR}/glm)
set(STB ${PROJECT_VENDOR_DIR}/stb)
set(SDL2 ${PROJECT_VENDOR_DIR}/SDL2)
set(GLAD ${PROJECT_VENDOR_DIR}/glad)
set(ENTT ${PROJECT_VENDOR_DIR}/entt)
set(VOLK ${PROJECT_VENDOR_DIR}/volk)
set(BOX2D ${PROJECT_VENDOR_DIR}/box2d)
set(TRACY ${PROJECT_VENDOR_DIR}/tracy)
set(IMGUI ${PROJECT_VENDOR_DIR}/imgui)
set(ENKITS ${PROJECT_VENDOR_DIR}/enkiTS)
set(SPDLOG ${PROJECT_VENDOR_DIR}/spdlog)
set(FASTGLTF ${PROJECT_VENDOR_DIR}/fastgltf)
set(IMGUIZMO ${PROJECT_VENDOR_DIR}/imguizmo)
set(OPENAL ${PROJECT_VENDOR_DIR}/openal-soft)
set(VK-BOOTSTRAP ${PROJECT_VENDOR_DIR}/vk-bootstrap)
set(MSDF-ATLAS-GEN ${PROJECT_VENDOR_DIR}/msdf-atlas-gen)
set(VULKAN-MEMORY-ALLOCATOR ${PROJECT_VENDOR_DIR}/VulkanMemoryAllocator)
set(GAME-NETWORKING-SOCKETS ${PROJECT_VENDOR_DIR}/GameNetworkingSockets)

# ##############################################################################
# Project executable setup
# ##############################################################################

file(GLOB_RECURSE PROJECT_SOURCES 
        ${PROJECT_SOURCE_DIR}/*.h
        ${PROJECT_SOURCE_DIR}/*.cpp
        )

list(FILTER PROJECT_SOURCES EXCLUDE REGEX ".*/Platforms/.*")

# # Additional project sources (vendors, etc.)
list(APPEND PROJECT_SOURCES
        ${ENTT}/entt.hpp
        ${STB}/stb_image.h
        ${STB}/stb_image.cpp
        )



if (TB_HEADLESS)
    list(APPEND PROJECT_SOURCES
        ${PROJECT_SOURCE_DIR}/Platforms/Null/NullPlatformInput.cpp
        ${PROJECT_SOURCE_DIR}/Platforms/Null/NullPlatformUtils.cpp
        ${PROJECT_SOURCE_DIR}/Platforms/Null/NullPlatformWindow.cpp
        )
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    list(APPEND PROJECT_SOURCES
        ${PROJECT_SOURCE_DIR}/Platforms/Windows/WindowsInput.cpp
        ${PROJECT_SOURCE_DIR}/Platforms/Windows/WindowsPlatformUtils.cpp
        ${PROJECT_SOURCE_DIR}/Platforms/Windows/WindowsWindow.cpp
        )

elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    list(APPEND PROJECT_SOURCES
        ${PROJECT_SOURCE_DIR}/Platforms/Linux/LinuxInput.cpp
        ${PROJECT_SOURCE_DIR}/Platforms/Linux/LinuxPlatformUtils.cpp
        ${PROJECT_SOURCE_DIR}/Platforms/Linux/LinuxWindow.cpp
        )

elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    list(APPEND PROJECT_SOURCES
        ${PROJECT_SOURCE_DIR}/Platforms/MacOS/MacOSInput.cpp
        ${PROJECT_SOURCE_DIR}/Platforms/MacOS/MacOSPlatformUtils.cpp
        ${PROJECT_SOURCE_DIR}/Platforms/MacOS/MacOSPlatformUtils.mm
        ${PROJECT_SOURCE_DIR}/Platforms/MacOS/MacOSWindow.cpp
        )

elseif (CMAKE_SYSTEM_NAME STREQUAL "Android")
    list(APPEND PROJECT_SOURCES
        ${PROJECT_SOURCE_DIR}/Platforms/Android/AndroidInput.cpp
        ${PROJECT_SOURCE_DIR}/Platforms/Android/AndroidPlatformUtils.cpp
        ${PROJECT_SOURCE_DIR}/Platforms/Android/AndroidWindow.cpp
        )
endif()


add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES})

if (TB_NETWORKING)
target_include_directories(${PROJECT_NAME} PUBLIC ${GAME-NETWORKING-SOCKETS}/include) 
endif (TB_NETWORKING)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}) 
target_include_directories(${PROJECT_NAME} PUBLIC ${SDL2_INCLUDE_DIRS}) 
target_include_directories(${PROJECT_NAME} PUBLIC ${FASTGLTF}/include) 
target_include_directories(${PROJECT_NAME} PUBLIC ${SPDLOG}/include) 
target_include_directories(${PROJECT_NAME} PUBLIC ${OPENAL}/include) 
target_include_directories(${PROJECT_NAME} PUBLIC ${BOX2D}/include) 
target_include_directories(${PROJECT_NAME} PUBLIC ${ENKITS}/src) 
target_include_directories(${PROJECT_NAME} PUBLIC ${IMGUIZMO}) 
target_include_directories(${PROJECT_NAME} PUBLIC ${IMGUI}) 
target_include_directories(${PROJECT_NAME} PUBLIC ${TRACY}) 
target_include_directories(${PROJECT_NAME} PUBLIC ${ENTT}) 
target_include_directories(${PROJECT_NAME} PUBLIC ${STB}) 
target_include_directories(${PROJECT_NAME} PUBLIC ${GLM}) 
target_include_directories(${PROJECT_NAME} PRIVATE ${VULKAN-MEMORY-ALLOCATOR}/include) 
target_include_directories(${PROJECT_NAME} PRIVATE ${MSDF-ATLAS-GEN}/msdf-atlas-gen) 
target_include_directories(${PROJECT_NAME} PRIVATE ${VK-BOOTSTRAP}/src) 
target_include_directories(${PROJECT_NAME} PRIVATE ${GLAD}) 
target_include_directories(${PROJECT_NAME} PRIVATE ${VOLK}) 
target_include_directories(${PROJECT_NAME} PRIVATE ${STB}) 

# ####################
# Add Subdirectories
# ####################
if (NOT TARGET fastgltf)
    add_subdirectory(${GLM})
endif()
if (NOT TARGET SDL2-static)
    add_subdirectory(${SDL2})
endif()
if (NOT TARGET volk)
    add_subdirectory(${VOLK})
endif()
if (NOT TARGET imgui)
    add_subdirectory(${IMGUI})
endif()
if (NOT TARGET box2d)
    add_subdirectory(${BOX2D})
endif()
if (NOT TARGET tracy)
    add_subdirectory(${TRACY})
endif()
if (NOT TARGET enkiTS)
    add_subdirectory(${ENKITS})
endif()
if (NOT TARGET spdlog)
    add_subdirectory(${SPDLOG})
endif()
if (NOT TARGET imguizmo)
    add_subdirectory(${IMGUIZMO})
endif()
if (NOT TARGET fastgltf)
    add_subdirectory(${FASTGLTF})
endif()
if (NOT TARGET vk-bootstrap::vk-bootstrap)
    add_subdirectory(${VK-BOOTSTRAP})
endif()
if (NOT TARGET msdf-atlas-gen)
    add_subdirectory(${MSDF-ATLAS-GEN})
endif()
if (NOT TARGET GPUOpen::VulkanMemoryAllocator)
    add_subdirectory(${VULKAN-MEMORY-ALLOCATOR})
endif()
if (TB_NETWORKING AND NOT TARGET GameNetworkingSockets)
    add_subdirectory(${GAME-NETWORKING-SOCKETS})
    target_link_libraries(${PROJECT_NAME} PRIVATE GameNetworkingSockets::static)
endif ()


if(NOT EMSCRIPTEN AND NOT UNIX)
    add_subdirectory(${OPENAL})
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")

    if (MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE /experimental:c11atomics console)


        if(CMAKE_BUILD_TYPE MATCHES "Debug")
            set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")
            set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /SUBSYSTEM:CONSOLE")
            set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
        endif()
    endif()

    if (MINGW)
        set(CMAKE_SYSTEM_NAME Windows)
        set(ENABLE_STATIC_RUNTIME ON CACHE BOOL "Enable static runtime" FORCE)
        target_link_options(${PROJECT_NAME} PUBLIC -static)

        # does this even work?
        if(MAINECOON_DIST)
            target_link_options(${PROJECT_NAME} PUBLIC -mwindows)
        else()
            target_link_options(${PROJECT_NAME} PUBLIC -mconsole)
        endif()
    endif()

    message(STATUS "Generating scripts for Windows")
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        OpenAL::OpenAL
        SDL2::SDL2main
        SDL2::SDL2-static
        )
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")

    find_package(OpenAL REQUIRED STATIC)
    target_link_options(${PROJECT_NAME} PUBLIC -ludev)

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)

    message(STATUS "Generating scripts for Linux")
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        spdlog
        PkgConfig::GTK4
        ${OPENAL_LIBRARY}
        SDL2::SDL2-static
        )
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message(STATUS "Generating scripts for Mac")
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        spdlog
        OpenAL::OpenAL
        SDL2::SDL2-static
        )
elseif (CMAKE_SYSTEM_NAME STREQUAL "Android")
    message(STATUS "Generating scripts for Android")
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        log
        EGL
        spdlog
        GLESv2
        GLESv3
        android
        SDL2::SDL2
        SDL2::SDL2main
        )
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE 
    glm
    imgui
    box2d
    tracy 
    enkiTS
    imguizmo
    fastgltf
    volk::volk
    msdf-atlas-gen 
    vk-bootstrap::vk-bootstrap
    GPUOpen::VulkanMemoryAllocator
    )


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_NO_PROTOTYPES=1")

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -DDEBUG -DTB_DEBUG=1 -DTRACY_ENABLE=1")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# Ensure that hot-reload is enabled for VS
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
endif()

target_precompile_headers(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/tbpch.h")
