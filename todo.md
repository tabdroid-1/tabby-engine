Todo
----

- High priority
    - Physics stuff. (raycast triggers etc) 
        - <s>every 2d physics related stuff to Physics2D</s> Done
        - <s>2D Raycast</s> Done
        - <s>Return Entity, Collision etc info on body enter callback.</s> Done
        - <s>Attach child colliders to parent with rigidbody. 
          This will be iterated through all entities until child ends or child has rigidbody component. </s> Done Since 21-04-2024_1 Alpha
        - <s>Box2D 3.0 migration </s> Done Since 23-04-2024_1 Alpha.
        - <s>Add circle, capsule and polygon casting. </s> Done since 01-05-2024_1 Alpha
        - <s>Add collision filtering(layer/mask). </s> Done since 03-05-2024_1 Alpha
        - <s>Collision filtering for raycasting. </s> Done since 05-05-2024_1 Alpha
    - <s>Maybe use SDL2 for window creationg instead of glfw cus it has some problems.</s> Done
    - <s>Android Support.</s> Done
    - <s>Controller support. </s> Done
    - <s>Way to Select preffered FPS for application.</s> Done
    - <s>Fixed Update for Scripts </s> Done
    - <s>Move entities to scene that is being switched to that has "do not delete on load" enabled</s>. Done
        - <s>Update entity handle of childs and parents when switching scene.</s> No longer needed
        - <s>maybe engine should have only one entity registy and use that instead of creating new one for each scene </s> Done
        - <s>Create queue for rigidbody initialization. Else application crashes </s> Done
    - <s>Use OpenAL instead of miniaudio. </s> Done
        - <s>Play SFX and delete when not needed. </s> Done 
        - <s>Audio streaming for long audios. </s> Done 
    - <s>Asset manager. </s> Done
    - <s>Go balls deep in to ECS (ECS like in Bevy Engine)</s> idk. 
    - <s>Initial Prefab class which serializes in to Tabby Prefab Format. </s> Done Since 25-05-2024_1 Alpha
    - Try vulkan
    - Initial Level Editor work.
    - Way for Prefabs to hold asset paths that will be required by components.
    - Implement File Dialog to linux(maybe using gtk)
    - Add font support to asset manager.
    - Fix OpenGL ES Text Shader and fix framebuffer. 
    - Clean up the code, add comments etc.
    - Multithreading
    - Create Application by reading a file. From a file named "tbconfig.(file format)" maybe.
    - Renderer is shit


- Low priority
    - Plugins
    - Angel Script
    - Animation
    - Basic networking
    - Create pop-up showing error on release builds instead of just crashing without any info. 
    - Vulkan.
    - C++ reflection

- Misc.
    - Create Documentation
    - Create Build scripts

Bugs
----
- CurrentYFrame picks seamingly random coordinates from texture.
- GLES 3.0 text shader not working.
- Textures, Controllers, Sounds and Texts are not visible/working on web builds.
