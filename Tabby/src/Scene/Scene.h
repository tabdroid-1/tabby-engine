#pragma once

#include "Physics/Physics.h"
#include <Scene/GameObjectCollection.h>
#include <cstdio>

class Scene {
public:
    virtual void
    OnCreate()
        = 0; // Called when scene initially created. Called once per scene.
    virtual void
    OnDestroy()
        = 0; // Called when scene destroyed. Called once per scene.

    virtual void OnActivate() {}; // Called whenever a scene is activated.
    virtual void OnDeactivate() {}; // Called whenever a scene is deactivated.

    virtual void Update(float deltaTime) {};
    virtual void LateUpdate(float deltaTime) {};
    virtual void Draw() {};

    // float SCREEN_SCALE = 1;
    // float SCREEN_WIDTH = 2560;
    // float SCREEN_HEIGHT = 1600;
    //
    // float PIXELS_PER_METER = 32;
    //
    // float WORLD_WIDTH = 500;
    // float WORLD_HEIGHT = 500;
    // float WORLD_SCALE_X = WORLD_WIDTH / SCREEN_WIDTH; // divide world width by screen width to get world scale
    // float WORLD_SCALE_Y = WORLD_HEIGHT / SCREEN_HEIGHT; // divide world height by screen height to get world scale
    // const float PHYSICS_WORLD_SCALE = PIXELS_PER_METER * SCREEN_SCALE; // get the average of world scale x and y for unuiform scaling
    //
    // float METRE_WIDTH = WORLD_WIDTH / PIXELS_PER_METER;
    // float METRE_HEIGHT = WORLD_HEIGHT / PIXELS_PER_METER;

    Physics physics;
    GameObjectCollection gameObjects;
};
