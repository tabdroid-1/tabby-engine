#pragma once

#include <tbpch.h>

#include "Tabby/Core/Timestep.h"
#include "Tabby/Core/UUID.h"

#include "entt/entt.hpp"

class b2World;

namespace Tabby {

class Entity;

class Scene {
public:
    Scene();
    ~Scene();

    virtual void OnCreate() {}; // Called when scene initially created. Called once per scene.
    virtual void OnActivate() {}; // Called once when scene is activated.
    virtual void OnDeactivate() {}; // Called when switched to other scene.
    virtual void DrawImGui() {}; // Draws mostly debugging related ui using Dear ImGui.

    static Ref<Scene> Copy(Ref<Scene> other);

    Entity CreateEntity(const std::string& name = std::string());
    Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
    void DestroyEntity(Entity entity);

    void OnStart();
    void OnStop();

    void OnUpdate(Timestep ts);

    void OnViewportResize(uint32_t width, uint32_t height);

    Entity DuplicateEntity(Entity entity);

    Entity FindEntityByName(std::string_view name);
    Entity GetEntityByUUID(UUID uuid);

    Entity GetPrimaryCameraEntity();

    bool IsRunning() const { return m_IsRunning; }
    bool IsPaused() const { return m_IsPaused; }

    void SetPaused(bool paused) { m_IsPaused = paused; }

    void Step(int frames = 1);

    template <typename... Components>
    auto GetAllEntitiesWith()
    {
        return m_Registry.view<Components...>();
    }

private:
    template <typename T>
    void OnComponentAdded(Entity entity, T& component);

    void OnPhysics2DStart();
    void OnPhysics2DStop();

private:
    entt::registry m_Registry;
    uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
    bool m_IsRunning = false;
    bool m_IsPaused = false;
    int m_StepFrames = 0;

    std::unordered_map<UUID, entt::entity> m_EntityMap;

    friend class Entity;
    friend class SceneSerializer;
    friend class SceneHierarchyPanel;
};

}
