Todo
---- 
- High priosity
    - Physics stuff. (raycast triggers etc) 
        - <s>every 2d physics related stuff to Physics2D</s> Done
        - <s>2D Raycast</s> Done
        - <s>Return Entity, Collision etc info on body enter callback.</s> Done
        - Maybe combine colliders in to one component instead of multiple components.
    - Maybe use SDL2 for window creationg instead of glfw cus it has some problems.
    - Use OpenAL instead of miniaudio.
    - More controll over audio.
    - Animation
    - Fixed Update for Scripts
    - Asset manager
 
- Low priority
    - Place TracyProfiler in to Vendor
    - Way to Select preffered FPS for application.
    - C++ reflection maybe?
    - Basic networking
    - Maybe instead of putting render order to spriterenderer put it in component all entities have.
    - tbpf (TabbyPrefabFormat) format and Prefab editor. (kinda like level editor that generates tbpf format) 
        - tbpf file will contain
            - entity prefabs(player, npc, level elemets etc.)
            - levels (basically compilation of prefabs)
        - individual prefabs and/or level can be spawned. preferably each app should hold 1 tbpf file which will hold everything.
    - <s>Fix android and web builds.</s> (maybe later)

Bugs
----
- CurrentYFrame picks seamingly random coordinates from texture.
- GLES 3.0 text shader not working.
