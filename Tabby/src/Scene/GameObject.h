#pragma once

#include <memory>
#include <string>
#include <vector>

#include <ComponentSystem/Component.h>
#include <ComponentSystem/Components/C_Drawable.h>
#include <ComponentSystem/Components/C_Transform.h>
#include <Core/S_Drawable.h>

class GameObject {
public:
    GameObject(std::string name);
    GameObject(std::string name, bool destroyOnLoad);

    void Awake(); // Called when object created. Use to ensure required components
                  // are present.
    void Start(); // Called after Awake method. Use to initialise variables.

    void Update(float deltaTime);
    void LateUpdate(float deltaTime);
    void Draw();
    void Delete();

    template <typename T>
    std::shared_ptr<T> AddComponent()
    {
        static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

        // TODO: allow us to add more than one component, implement getcomponents
        //  Check that we don't already have a component of this type.
        for (auto& exisitingComponent : components) {
            if (std::dynamic_pointer_cast<T>(exisitingComponent)) {
                return std::dynamic_pointer_cast<T>(exisitingComponent);
            }
        }

        std::shared_ptr<T> newComponent = std::make_shared<T>(this);

        components.push_back(newComponent);

        if (std::dynamic_pointer_cast<C_Drawable>(newComponent)) {
            drawable = std::dynamic_pointer_cast<C_Drawable>(newComponent);
        }

        return newComponent;
    };

    template <typename T>
    std::shared_ptr<T> GetComponent()
    {
        static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

        // Check that we don't already have a component of this type.
        for (auto& exisitingComponent : components) {
            if (std::dynamic_pointer_cast<T>(exisitingComponent)) {
                return std::dynamic_pointer_cast<T>(exisitingComponent);
            }
        }

        return nullptr;
    };

    std::shared_ptr<GameObject> AddChild(std::shared_ptr<GameObject> child)
    {
        child->SetIsChild(true);
        children.push_back(child);

        drawables.Add(children);

        return child;
    }

    std::shared_ptr<C_Drawable> GetDrawable();
    std::string GetName() { return name; }
    bool IsPersistent() { return isPersistent; }
    bool IsChild() { return isChild; }
    void SetIsChild(bool IsChild) { isChild = IsChild; }

    bool IsQueuedForRemoval();
    void QueueForRemoval();

    std::shared_ptr<C_Transform> transform;

private:
    std::string name;
    bool isPersistent;
    bool isChild;
    S_Drawable drawables;

    std::vector<std::shared_ptr<Component>> components;
    std::vector<std::shared_ptr<GameObject>> children;
    std::shared_ptr<C_Drawable> drawable;
    bool queuedForRemoval;
};
