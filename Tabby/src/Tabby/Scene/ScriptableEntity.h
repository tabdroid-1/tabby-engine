
#pragma once

#include "Entity.h"
#include <Tabby/Core/Log.h>
#include <Tabby/Scene/Components.h>

namespace Tabby {

class ScriptableEntity {
public:
    virtual ~ScriptableEntity() { }

    template <typename T>
    T& GetComponent()
    {
        return m_Entity.GetComponent<T>();
    }

protected:
    virtual void OnCreate() { }
    virtual void OnDestroy() { }
    virtual void OnUpdate(Timestep ts) { }

    // Physics Callback
    virtual void OnCollisionEnter() { }
    virtual void OnCollisionExit() { }
    virtual void OnPreSolve() { }
    virtual void OnPostSolve() { }

private:
    Entity m_Entity;
    friend class Scene;
    friend class Physics2DContactListener;
    friend class b2Body;
};

}
