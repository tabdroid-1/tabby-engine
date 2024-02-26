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
    - <s>Move entities to scene that is being switched to that has "do not delete on load" enabled</s>.
        - <s>Update entity handle of childs and parents when switching scene.</s> No longer needed
        - <s>maybe engine should have only one entity registy and use that instead of creating new one for each scene </s> Done
        - <s>Create queue for rigidbody initialization. Else application crashes </s>
    - Use OpenAL instead of miniaudio.
    - Asset manager
    - Animation
    - Basic networking
    - Use freetype instead of msdf-atlas-gen
    - Clean up the code, add comments etc.
    - Create Application by reading a file. From a file named "tbconfig.(file format)" maybe.


- Low priority
    - Vulkan.
    - C++ reflection maybe?
    - Maybe instead of putting render order to spriterenderer put it in component all entities have.
    - tbpf (TabbyPrefabFormat) format and Prefab editor. (kinda like level editor that generates tbpf format) 
        - tbpf file will contain
            - entity prefabs(player, npc, level elemets etc.)
            - levels (basically compilation of prefabs)
        - individual prefabs and/or level can be spawned. preferably each app should hold 1 tbpf file which will hold everything.
        - encryption
        - this is probably should be in a different repo

Bugs
----
- CurrentYFrame picks seamingly random coordinates from texture.
- GLES 3.0 text shader not working.
- Textures, Controllers, Sounds and Texts are not visible/working on web builds.
