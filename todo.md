Todo
---- 
 - Physics stuff. (raycast triggers etc)
 - Animation
 - Asset manager
 - Audio player
 - tbpf (TabbyPrefabFormat) format and Prefab editor. (kinda like level editor that generates tbpf format) 
   - tbpf file will contain
     - entity prefabs(player, npc, level elemets etc.)
     - levels (basically compilation of prefabs)
   - individual prefabs and/or level can be spawned. preferably each app should hold 1 tbpf file which will hold everything.
 - Maybe instead of putting render order to spriterenderer put it in component all entities have.
 - <s>Fix android and web builds.</s> (maybe later)

Bugs
----
 - CurrentYFrame picks seamingly picks random coords from texture.
