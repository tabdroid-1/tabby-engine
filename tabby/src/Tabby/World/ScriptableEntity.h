#pragma once

#include "Entity.h"
#include <Tabby/Physics/2D/Physics2DTypes.h>

namespace Tabby {

class Physics2D;

class ScriptableEntity {
public:
    virtual ~ScriptableEntity() { }

    template <typename T>
    T& GetComponent()
    {
        return m_Entity.GetComponent<T>();
    }

    template <typename T>
    bool HasComponent()
    {
        return m_Entity.HasComponent<T>();
    }

protected:
    virtual void OnCreate() { }
    virtual void OnDestroy() { }
    virtual void Update(Timestep ts) { }
    virtual void LateUpdate(Timestep ts) { }
    virtual void FixedUpdate(Timestep ts) { }
    virtual void Draw() { }

    // Physics Callback
    virtual void OnCollisionEnter(ContactCallback contact) { }
    virtual void OnCollisionExit(ContactCallback contact) { }
    virtual void OnPreSolve(ContactCallback contact) { }
    virtual void OnPostSolve(ContactCallback contact) { }

private:
    Entity m_Entity;
    friend class World;
    friend class Physisc2D;
};

}
