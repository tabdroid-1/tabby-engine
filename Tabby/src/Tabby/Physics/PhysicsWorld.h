#pragma once

#include "box2d/b2_world.h"
#include "box2d/box2d.h"
#include "glm/fwd.hpp"

namespace Tabby {

class Scene;

class Physics2DContactListener : public b2ContactListener {
public:
    Physics2DContactListener(Scene* scene, b2World* world);
    void BeginContact(b2Contact* contact);
    void EndContact(b2Contact* contact);
    void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
    void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);

private:
    Scene* m_Scene = nullptr;
};

class PhysicsWorldHandle2D {
public:
    PhysicsWorldHandle2D(Scene* scene, const glm::vec2& gravity);
    ~PhysicsWorldHandle2D() { delete m_PhysicsWorld; }

    void Step(float ts, int32_t velocityIterations, int32_t positionIterations)
    {
        m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);
    }

    b2World* GetPhysicsWorld() { return m_PhysicsWorld; }

private:
    b2World* m_PhysicsWorld = nullptr;
    Physics2DContactListener* m_PhysicsContactListener;
    Scene* m_Scene = nullptr;
};

}
