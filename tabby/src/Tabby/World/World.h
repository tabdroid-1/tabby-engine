#pragma once

#include <tbpch.h>
#include <Tabby/Core/Time/Timestep.h>

#include "entt/entt.hpp"

namespace Tabby {

class Entity;
class GLTF;

class World {

public:
    World();

    static void Init();

    static void LoadMap(const std::string& mapPath);

    static Entity CreateEntity(const std::string& name = std::string());
    static Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
    static void DestroyEntity(Entity entity);
    static void DestroyEntityWithChildren(Entity entity);

    static Entity DuplicateEntity(Entity entity);

    static Entity FindEntityByName(std::string_view name);
    static Entity GetEntityByUUID(UUID uuid);

    static Entity GetPrimaryCameraEntity();

    static void OnStart();
    static void OnStop();

    static void Update(Timestep ts);
    static void DrawImGui();

    static void OnViewportResize(uint32_t width, uint32_t height);

    static bool IsRunning() { return s_Instance->m_IsRunning; }
    static bool IsPaused() { return s_Instance->m_IsPaused; }

    static void SetPaused(bool paused) { s_Instance->m_IsPaused = paused; }

    static void Step(int frames = 1);

    template <typename... Components>
    static auto GetAllEntitiesWith()
    {
        return s_Instance->GetRegistry().view<Components...>();
    }

    static entt::registry& GetRegistry();

    // private:
    //     static void ProcessQueue(bool canSwitch);
    //     static void private_SwitchTo(std::string sceneName);

private:
    template <typename T>
    static void OnComponentAdded(Entity entity, T& component);

private:
    uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
    bool m_IsRunning = false;
    bool m_IsPaused = false;
    int m_StepFrames = 0;

    float m_FixedUpdateAccumulator = 0;

    static entt::registry m_EntityRegistry;
    std::unordered_map<UUID, entt::entity> m_EntityMap;

    static std::string m_CurrentMap;

private:
    static World* s_Instance;

    friend class Entity;
};

}
