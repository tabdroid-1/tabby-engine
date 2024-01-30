Todo
---- 
 - Physics stuff. (raycast triggers etc) (Currently working on this)
  - currently pushing every physics related stuff to Physics2D.
 - C++ reflection maybe?
 - More controll over audio.
 - Animation
 - Asset manager
 - Way to Sellect preffered FPS for application.
 - Fixed Update for Scripts
 - Maybe instead of putting render order to spriterenderer put it in component all entities have.
 - Maybe use SDL2 for window creationg instead of glfw.
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
