#pragma once

#include "Graphics/Camera/CameraTools.h"
#include "raylib.h"
#include <Core/UUID.h>
#include <Physics/Physics.h>
#include <Scene/Octree/Octree.h>
#include <box2d/b2_world.h>
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

    void InitScene();
    void Update();
    void LateUpdate();
    void Draw();
    virtual void DrawHud() {}; // Draws ingame ui. These will be always on top
    virtual void DrawImGui() {}; // Draws mostly debugging related ui using Dear ImGui.

    void SetActiveCamera(const entt::entity camera) { m_ActiveCamera = camera; }

    GameObject CreateEntity(const std::string& name = std::string());
    GameObject CreateEntityWithUUID(UUID uuid, const std::string& name);
    void DestroyEntity(GameObject gameObject);

    entt::registry m_Registry;

    bool creationDone = false;
    bool activationDone = false;

private:
    Physics* physics = new Physics();
    Octree* octree;
    entt::entity m_ActiveCamera;

    friend class GameObject;
};

}
