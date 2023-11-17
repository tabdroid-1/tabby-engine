
#pragma once

#include <Scene/GameObject.h>

namespace Tabby {

class Scene;

class ScriptableEntity {
public:
    virtual ~ScriptableEntity() { }

    template <typename T>
    T& GetComponent()
    {
        return m_GameObject.GetComponent<T>();
    }

    template <typename T>
    bool HasComponent()
    {
        return m_GameObject.HasComponent<T>();
    }

protected:
    virtual void OnCreate() { }
    virtual void OnDestroy() { }
    virtual void Update() { }
    virtual void LateUpdate() { }

private:
    GameObject m_GameObject;
    friend class Scene;
};
}
