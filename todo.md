Todo
----

- High priority
    - Physics stuff. (raycast triggers etc) 
        - <s>every 2d physics related stuff to Physics2D</s> Done
        - <s>2D Raycast</s> Done
        - <s>Return Entity, Collision etc info on body enter callback.</s> Done
        - Maybe combine colliders in to one component instead of multiple components.
        - Add collision filtering(layer/mask).
    - <s>Maybe use SDL2 for window creationg instead of glfw cus it has some problems.</s> Done
    - <s>Fix android.</s> Done
    - <s>Controller support. </s> Done
    - <s>Way to Select preffered FPS for application.</s> Done
    - <s>Fixed Update for Scripts </s> Done
    - <s>Move entities to scene that is being switched to that has "do not delete on load" enabled</s>. Done
        - <s>Update entity handle of childs and parents when switching scene.</s> No longer needed
        - <s>maybe engine should have only one entity registy and use that instead of creating new one for each scene </s> Done
        - <s>Create queue for rigidbody initialization. Else application crashes </s> Done
    - <s>Use OpenAL instead of miniaudio. </s> Done
        - <s>Play SFX and delete when not needed. </s> Done 
        - <s>Audio streaming for long audios. </s> Done after not noticing stupid mistake for 3 weeks.
    - <s>Asset manager. </s> Done
    - Use freetype instead of msdf-atlas-gen. Currently working on this
    - Clean up the code, add comments etc.
    - Multithreading
    - Create Application by reading a file. From a file named "tbconfig.(file format)" maybe.
    - Renderer is shit


- Low priority
    - Animation
    - Basic networking
    - Create pop-up showing error on release builds instead of just crashing without any info. 
    <!-- - tbpf (TabbyPrefabFormat) format and Prefab editor. (kinda like level editor that generates tbpf format)  -->
    <!--     - tbpf file will contain -->
    <!--         - entity prefabs(player, npc, level elemets etc.) -->
    <!--         - levels (basically compilation of prefabs) -->
    <!--     - individual prefabs and/or level can be spawned. preferably each app should hold 1 tbpf file which will hold everything. -->
    <!--     - encryption -->
    <!--     - this is probably should be in a different repo -->
    - Vulkan.
    - C++ reflection
    - Create a function that prints current output device name and a function to pick wich audio device you want to use.

- Misc.
    - Create Documentation
    - Create Build scripts

Bugs
----
- CurrentYFrame picks seamingly random coordinates from texture.
- GLES 3.0 text shader not working.
- Textures, Controllers, Sounds and Texts are not visible/working on web builds.
