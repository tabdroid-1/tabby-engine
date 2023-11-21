Tabby Engine
=============================

A game engine made using Raylib in C++.
This engine is meant to be ran on older computers. (Android and Web versions planned).
Engine is mainly for 2D games but it also supports 3D.

Planned Features / To-do list
-----------------------------

- <s>ImGui implementation for easy component value editing without rebuilding and more.</s> Done
- <s>Parent-Child system or whatever.</s> Done
- <s>Out of screen texture culling.</s> Done
- <s>Make engine run using a function instead of having to create main function in executable.</s> Done
- <s>Remove float dt from every function. There is no need for it. There is a function to get deltatime. </s> Done
- <s>Basic UI implementation.</s> Done
- Octree. (Currently working on this)
- Script public variable reflection.
- Custom profiling implementation (tracy Profiler).
- Function to search and get GameObjects using name tag from active scene.
- Physics rework. (Hinges, make components more editable, callbacks like onTriggerEnter, onRaycastHit etc.)
- Asset Manager. 
- Audio and audio component.
- Serialization(aka saving).
- Scene manager rework. (switching scenes. scene pool etc.)
- Multi-threading
- More advanced animation system.
- Particle system.
- LDtk tile map.
- <s>Better 3D support</s> Priorities...
- <s>Steamworks API implementation</s> Priorities...
- <s>Web, Chrome OS, FreeBSD, Raspberry Pi and Android builds </s> Too far-fetched for now.
- <s>More advanced asset manager. Assets will be stored in the code and (some) will be encrypted using AES encryption to complicate data mining/revers engineering, but there will be option to use files instead of storing in code.(might be in a private branch though) </s> Too far-fetched for now.
- <s>Clean the f*kin "CMakeLists.txt" to be more understandable and useable. It looks hideous.</s> Extremely far-fetched for now.

Engine Dependencies
-------------------

- raylib
- box2d
- entt
- LDtkLoader
- raylib-tmx
- ImGui
- rlImGui

Compiling
---------

 1. ```shell
    git clone https://github.com/tabdroid-1/TabbyEngine.git && mkdir TabbyEngine/build && cd TabbyEngine/build
    ```

 2. ```shell
    cmake .. -DOPENGL_VERSION="ES 2.0" && make
    ```

    (u can also use opengl 2.1, 3.3, 4,3 or even 1.1 but that might cause errors and/or visual bugs)

Note From the author
 --------------------

- If you are going to use this game engine, pls support as much platform as possible (eh. You do not have to support Chrome OS or FreeBSD) and support more architectures like x86_32, x86_64, aarch64. You do not have to that but if you do it you would make me happi. :3

Special Thanks
 --------------

- JeffM for answering my stupid questions (Discord: jefferym)
