#pragma once

#include "Components.h"
#include "Scene.h"
#include "Tabby/Core/UUID.h"

#include "entt/entt.hpp"
#include <utility>

namespace Tabby {

class Entity {
public:
    Entity() = default;
    Entity(entt::entity handle);
    Entity(const Entity& other) = default;

    template <typename T, typename... Args>
    T& AddComponent(Args&&... args)
    {
        TB_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
        T& component = SceneManager::GetRegistry().emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
        SceneManager::GetCurrentScene()->OnComponentAdded<T>(*this, component);

        return component;
    }

    template <typename T, typename... Args>
    T& AddOrReplaceComponent(Args&&... args)
    {
        T& component = SceneManager::GetRegistry().emplace_or_replace<T>(m_EntityHandle, std::forward<Args>(args)...);
        SceneManager::GetCurrentScene()->OnComponentAdded<T>(*this, component);
        return component;
    }

    template <typename T>
    T& GetComponent()
    {
        TB_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
        return SceneManager::GetRegistry().get<T>(m_EntityHandle);
    }

    template <typename T>
    bool HasComponent()
    {
        return SceneManager::GetRegistry().any_of<T>(m_EntityHandle);
    }

    template <typename T>
    void RemoveComponent()
    {
        TB_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
        SceneManager::GetRegistry().remove<T>(m_EntityHandle);
    }

    void AddChild(Entity& child)
    {
        auto& hierarchy_node_component = GetComponent<HierarchyNodeComponent>();
        child.GetComponent<HierarchyNodeComponent>().Parent = std::make_pair(GetUUID(), this->m_EntityHandle);
        hierarchy_node_component.Children.emplace_back(std::make_pair(child.GetUUID(), child.m_EntityHandle));

        if (GetComponent<IDComponent>().IsPersistent)
            UpdateIsPersistent(true);
    }

    operator bool() const { return m_EntityHandle != entt::null; }
    operator entt::entity() const { return m_EntityHandle; }
    operator uint32_t() const { return (uint32_t)m_EntityHandle; }

    UUID GetUUID() { return GetComponent<IDComponent>().ID; }
    const std::string& GetName() { return GetComponent<TagComponent>().Tag; }

    bool operator==(const Entity& other) const
    {
        return m_EntityHandle == other.m_EntityHandle;
    }

    bool operator!=(const Entity& other) const
    {
        return !(*this == other);
    }

private:
    void UpdateIsPersistent(bool enable)
    {
        auto& hierarchy_node_component = GetComponent<HierarchyNodeComponent>();
        GetComponent<IDComponent>().IsPersistent = enable;

        for (auto& child : hierarchy_node_component.Children) {
            Entity childEntity = { child.second };
            childEntity.UpdateIsPersistent(enable);
        }
    }

private:
    entt::entity GetEntityHandle() { return m_EntityHandle; }
    entt::entity m_EntityHandle { entt::null };

    friend class Scene;
};

}
