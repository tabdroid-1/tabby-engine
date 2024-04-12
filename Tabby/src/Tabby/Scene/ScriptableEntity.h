
#pragma once

#include "Entity.h"
#include <Tabby/Log/Log.h>
#include <Tabby/Physics/2D/Physics2D.h>
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
    virtual void Update(Timestep ts) { }
    virtual void LateUpdate(Timestep ts) { }
    virtual void FixedUpdate(Timestep ts) { }

    // Physics Callback
    virtual void OnCollisionEnter(ContactCallback contact) { }
    virtual void OnCollisionExit(ContactCallback contact) { }
    virtual void OnPreSolve(ContactCallback contact) { }
    virtual void OnPostSolve(ContactCallback contact) { }

private:
    Entity m_Entity;
    friend class Scene;
    friend class Physics2DContactListener;
    friend class b2Body;
};

}
