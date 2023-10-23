Tabby Engine
=============================
A game engine made using Raylib in C++.
This engine is meant to be ran on older computers. (Android and Web versions planned).
Engine is mainly for 2D games but it also supports 3D.

Engine is build as a static library so it can be linked to executable statically and create standalone executable.


Planned Features / To-do list
-----------------------------
 - ImGui implementation for easy component value editing without rebuilding.(Currently working on this)
 - More advanced asset manager. Assets will be stored in the code and (some) will be encrypted using AES encryption to complicate data mining/revers engineering, but there will be option to use files instead of storing in code.(might be in a private branch though)
 - Audio component
 - More advanced animation system
 - Web, Chrome OS, FreeBSD, Raspberry Pi and Android builds
 - Better 3D support
 - Clean the f*kin "CMakeLists.txt" to be more understandable and useable. It looks hideous.
 - Steamworks API implementation

Engine Dependencies
-------------------
 - raylib
 - box2d
 - entt
 - LDtkLoader
 - raylib-tmx 

Compiling
---------
 1. ```shell
    git clone https://github.com/tabdroid-1/TabbyEngine.git && mkdir TabbyEngine/build && cd TabbyEngine/build
    ```
 2. ```shell
    cmake .. -DOPENGL_VERSION="ES 2.0" && make
    ```
    (u can also use opengl 2.1, 3.3, 4,3 or even 1.1 but that might cause error/visual bugs)

 Note From the author
 --------------------
 - If you are going to use this game engine, pls support as much platform as possible (eh. You do not have to support Chrome OS or FreeBSD) and support more architectures(like x86_32, x86_64, aarch64). You do not have to that but if you do it you would make me happi. :3
