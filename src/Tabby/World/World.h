#pragma once
#include <tbpch.h>
#include <Tabby/Foundation/Types.h>

#include <TaskScheduler.h>
#include <entt.hpp>

namespace Tabby {

class Camera;
class Entity;

enum Schedule : uint8_t {
    PreStartup = 0,
    Startup,
    PostStartup,

    // First,
    PreUpdate,
    Update,
    PostUpdate,
    // Last,

    // FixedFirst,
    FixedPreUpdate,
    FixedUpdate,
    FixedPostUpdate,
    // FixedLast,

    Draw,
};

// this need to be defined by user.
void vWorldHandleEntityDeletion(Entity entity);

class World {

public:
    World();

    static void Init();
    static void OnStart();
    static void OnStop();

    static void AddSystem(Schedule schedule, const std::function<void()>& function);

    template <typename T, typename... Args>
    static T& AddResource(Args&&... args)
    {
        T resource { std::forward<Args>(args)... };
        s_Instance->m_Resources[typeid(T)] = resource;
        return std::any_cast<T&>(s_Instance->m_Resources[typeid(T)]);
    }

    template <typename T>
    static T& GetResource()
    {
        TB_CORE_VERIFY_TAGGED(s_Instance->m_Resources.find(typeid(T)) != s_Instance->m_Resources.end(), "Resource not found!");

        return std::any_cast<T&>(s_Instance->m_Resources.at(typeid(T)));
    }

    static Entity CreateEntity(const std::string& name = std::string());
    static Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
    static void DestroyEntity(Entity entity);
    static void DestroyEntityWithChildren(Entity entity);

    static Entity DuplicateEntity(Entity entity);

    static Entity FindEntityByName(std::string_view name);
    static Entity GetEntityByUUID(UUID uuid);

    static void Update();
    void RendererThread();

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

private:
    static void UpdateEntityTransforms();

    template <typename T>
    static void OnComponentAdded(Entity entity, T& component);

private:
    uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
    bool m_IsRunning = false;
    bool m_IsPaused = false;
    int m_StepFrames = 0;

    float m_FixedUpdateAccumulator = 0;

    enki::TaskScheduler m_TaskScheduler;
    entt::registry m_EntityRegistry;
    std::unordered_map<UUID, entt::entity> m_EntityMap;

    std::unordered_map<std::type_index, std::any> m_Resources;

    // Called onece on application startup
    std::vector<std::function<void()>> m_PreStartupSystems;
    std::vector<std::function<void()>> m_StartupSystems;
    std::vector<std::function<void()>> m_PostStartupSystems;

    // Called every frame
    std::vector<std::function<void()>> m_PreUpdateSystems;
    std::vector<std::function<void()>> m_UpdateSystems;
    std::vector<std::function<void()>> m_PostUpdateSystems;

    // Called fixed update
    std::vector<std::function<void()>> m_FixedPreUpdateSystems;
    std::vector<std::function<void()>> m_FixedUpdateSystems;
    std::vector<std::function<void()>> m_FixedPostUpdateSystems;

    // Called everyupdate
    std::vector<std::function<void()>> m_DrawSystems;

private:
    inline static World* s_Instance;

    friend class Entity;
};

}
