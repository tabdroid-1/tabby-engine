Todo
---- 
- High priority
    - Physics stuff. (raycast triggers etc) 
        - <s>every 2d physics related stuff to Physics2D</s> Done
        - <s>2D Raycast</s> Done
        - <s>Return Entity, Collision etc info on body enter callback.</s> Done
        - Maybe combine colliders in to one component instead of multiple components.
    - <s>Maybe use SDL2 for window creationg instead of glfw cus it has some problems.</s> Done
    - <s>Fix android.</s> Done
    - Controller input support.
    - Fixed Update for Scripts
    - Use OpenAL instead of miniaudio.
    - Asset manager
    - Animation
    - Basic networking
    - Use freetype instead of msdf-atlas-gen
 
- Low priority
    - Way to Select preffered FPS for application.
    - C++ reflection maybe?
    - Maybe instead of putting render order to spriterenderer put it in component all entities have.
    - tbpf (TabbyPrefabFormat) format and Prefab editor. (kinda like level editor that generates tbpf format) 
        - tbpf file will contain
            - entity prefabs(player, npc, level elemets etc.)
            - levels (basically compilation of prefabs)
        - individual prefabs and/or level can be spawned. preferably each app should hold 1 tbpf file which will hold everything.
        - encryption

Bugs
----
- CurrentYFrame picks seamingly random coordinates from texture.
- GLES 3.0 text shader not working.
- Textures, Sounds and Texts are not visible/working on web builds.
