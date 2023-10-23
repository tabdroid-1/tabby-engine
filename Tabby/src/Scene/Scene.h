#pragma once

#include "raylib.h"
#include <Core/UUID.h>
#include <Physics/Physics.h>
#include <box2d/b2_world.h>
#include <cstdio>
#include <entt/entt.hpp>

namespace Tabby {

class GameObject;
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

    void InitPhysics();
    void InitScene();
    void Update(float deltaTime);
    void LateUpdate(float deltaTime);
    void Draw();
    virtual void DrawImGui() {}; // Called whenever a scene is deactivated.

    void SetActiveCamera(const Camera& camera) { ActiveCamera = camera; }

    GameObject CreateEntity(const std::string& name = std::string());
    GameObject CreateEntityWithUUID(UUID uuid, const std::string& name);
    void DestroyEntity(GameObject gameObject);

    entt::registry m_Registry;

    bool creationDone = false;
    bool activationDone = false;

private:
    Physics* physics = new Physics();
    Camera ActiveCamera;

    friend class GameObject;
};

}
