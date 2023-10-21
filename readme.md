Tabby Engine
=============================
A game engine made using Raylib in C++.
This engine is meant to be ran on older computers. (Android and Web versions planned).
Engine is mainly for 2D games but it also supports 3D.

Engine is build as a static library so it can be linked to executable statically and create standalone executable.


Planned Features / To-do list
----------------
 - More advanced asset manager. Assets will be stored in the code and (some) will be encrypted using AES encryption to complicate data mining/revers engineering(might be in a private branch though)
 - Audio component
 - More advanced animation system
 - Web and Android builds
 - Better 3D support


Compiling
----------------
 1. ```shell
    git clone -b 3d https://github.com/tabdroid-1/TabbyENG.git && mkdir TabbyENG/build && cd TabbyENG/build
    ```
 2. ```shell
    cmake .. -DOPENGL_VERSION="ES 2.0" && make
    ```
    (u can also use opengl 2.1, 3.3, 4,3 or even 1.1 but that might cause error/visual bugs)
