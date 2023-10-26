#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Scene/Scene.h>

#include <entt/entt.hpp>

namespace Tabby {

class GameObject {

public:
    GameObject() = default;
    GameObject(entt::entity handle, Scene* scene);
    GameObject(const GameObject& other) = default;

    template <typename T, typename... Args>
    T& AddComponent(Args&&... args)
    {
        T& component = m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
        // m_Scene->OnComponentAdded<T>(*this, component);
        return component;
    }

    template <typename T>
    T& GetComponent()
    {
        return m_Scene->m_Registry.get<T>(m_EntityHandle);
    }

    template <typename T>
    bool HasComponent()
    {
        return m_Scene->m_Registry.any_of<T>(m_EntityHandle);
    }

    template <typename T>
    void RemoveComponent()
    {
        m_Scene->m_Registry.remove<T>(m_EntityHandle);
    }

    void AddChild(GameObject& child);

    operator bool() const { return m_EntityHandle != entt::null; }
    operator entt::entity() const { return m_EntityHandle; }
    operator uint32_t() const { return (uint32_t)m_EntityHandle; }

    UUID GetUUID();
    const std::string& GetName();
    bool operator==(const GameObject& other) const
    {
        return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
    }

    bool operator!=(const GameObject& other) const
    {
        return !(*this == other);
    }

private:
    entt::entity m_EntityHandle { entt::null };
    Scene* m_Scene = nullptr;
};

}
