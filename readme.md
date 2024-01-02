Tabby Engine
=============================

Simple game engine written in C++.
This engine is meant to be ran on older computers. (Android and Web versions planned).
Engine is mainly for 2D games.

In this engine you mostly do everything using code.
There is Prefab/Level editor planned since its hard to make levels using just code.

Engine is based on Hazel engine.

Planned Features / To-do list
-----------------------------

- Audio and audio component.
- Asset Manager. 
- Profiling implementation (tracy Profiler).
- Serialization(aka saving).
- Multi-threading
- <s>Web, Raspberry Pi and Android builds </s> Too far-fetched for now.

Engine Dependencies
-------------------

- box2d
- entt
- ImGui
- glad
- glfw
- glm
- imguizmo
- spdlog
- stb_image
- yaml-cpp
- tracy

Compiling
---------

 1. ```shell
    git clone -b customRenderer https://github.com/tabdroid-1/TabbyEngine.git && mkdir TabbyEngine/build && cd TabbyEngine/build
    ```

 2. ```shell
    cmake .. && make

Note From the author
 --------------------

- If you are going to use this game engine, pls support as much platform as possible (eh. You do not have to support Chrome OS or FreeBSD) and support more architectures like x86_32, x86_64, aarch64. You do not have to that but if you do it you would make me happi. :3

Special Thanks
 --------------

- Jonny Hotbody
- Ronald McDonald
