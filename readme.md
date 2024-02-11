Tabby Engine
=============================

Simple game engine written in C++.
This engines main focus is compatibility for most New and old platforms.
Engine is mainly for 2D games but has 3d elements. Further 3D support planned.

In this engine you mostly do everything using code.
There is Prefab/Level editor planned since its hard to make levels using just code.

Engine architecture is based on Hazel engine.

External Dependencies
-------------------
- Bundled with engine
  - box2d
  - entt
  - ImGui
  - glad2
  - sdl2 
  - glm
  - imguizmo
  - spdlog
  - stb_image
  - yaml-cpp
  - msdf-atlas-gen
  - tracy profiler
  - miniaudio

- Manual installition might be required
  - Freetype
  - tinyxml2
  - PNG
  - Threads

Supported Graphics Apis
-----------------------
- OpenGL 3.3
- OpenGL 3.0 es

(Kinda)Supported Platforms
-------------------
- Linux
- Windows (probably. not tested)
- MacOS
- Web
- Android

Compiling
---------

You might have to install some libraries for building.
For building web install emscriptem from their git repo. Installing from package manager may give compilation errors.

- Native
  - For Unix:  

    1. ```shell
        git clone -b customRenderer https://github.com/tabdroid-1/TabbyEngine.git && mkdir TabbyEngine/build && cd TabbyEngine/build
        ```

    2. ```shell
        cmake .. && make
        ```
    
  - For Windows:  
    1. ```shell
        Todo
        ```

- For web 

  1. ```shell
     git clone -b customRenderer https://github.com/tabdroid-1/TabbyEngine.git && mkdir TabbyEngine/build && cd TabbyEngine/build
     ```

  2. ```shell
     emcmake cmake .. && make
     ```

  3. ```shell
     python3 -m http.server 8080
     ```

Note From the author
 --------------------
- If you are going to use this game engine, pls support as much platform as possible (eh. You do not have to support Chrome OS or FreeBSD) and support more architectures like x86_32, x86_64, aarch64. You do not have to that but if you do it you would make me happi. :3

Special Thanks
 --------------
- Jonny Hotbody
- Peppino Spaghetti
- Soilder from TF2
