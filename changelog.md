Version: 26-02-2024_1 Alpha
Changes:
- Changed SceneStateMachine's name to SceneManager because of changes.
- engine uses 1 registry for all scenes instead of 1 registry per scene (same for physics2d).
- Move Physics2D Callback classes in to different files.
- Physics2D objects are created in OnComponentAdded<T>() functions instead of OnPhysics2DStart cus there is no need .
- Switching scenes now works. Physics2D does not cause crash when switching scenes.
- Queue system for Physics2D and scene for preventing crashes.
    - Switching Scene before all Physics2D objects initialized. caused crash.
    - Creating rigidbody object and Colliders caused crash when physics2d world was in locked state. Thats why queue system was implemented. 
