cmake_minimum_required(VERSION 3.19)

project(imgui)


find_package(OpenGL REQUIRED)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_library(imgui STATIC
        imgui.cpp
        imgui_demo.cpp
        imgui_draw.cpp
        imgui_tables.cpp
        imgui_widgets.cpp
        # backends/imgui_impl_glfw.cpp
        # backends/imgui_impl_opengl3.cpp

        imgui.h
        imgui_internal.h
        imstb_rectpack.h
        imstb_textedit.h
        imstb_truetype.h
        )



target_link_libraries(imgui
        OpenGL::GL
        )
