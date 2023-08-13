#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Scene/Scene.h>

#include <entt.hpp>

namespace Tabby {

class GameObject {

public:
    GameObject() = default;
    GameObject(entt::entity handle, Scene* scene);
    GameObject(const GameObject& other) = default;

    // void Awake(); // Called when object created. Use to ensure required components
    //               // are present.
    // void Start(); // Called after Awake method. Use to initialise variables.
    //
    // void Update(float deltaTime);
    // void LateUpdate(float deltaTime);
    // void Draw();
    // void Delete();

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

    bool IsPersistent() { return isPersistent; }
    bool IsChild() { return isChild; }
    void SetIsChild(bool IsChild) { isChild = IsChild; }

private:
    bool isPersistent;
    bool isChild;

    entt::entity m_EntityHandle { entt::null };
    Scene* m_Scene = nullptr;
};

}
