#pragma once

#include "Tabby/Scene/Components.h"

#include "box2d/b2_world.h"
#include "box2d/box2d.h"
#include "glm/fwd.hpp"
#include <Tabby/Core/Assert.h>

namespace Tabby {

namespace Utils {

    inline b2BodyType Rigidbody2DTypeToBox2DBody(Rigidbody2DComponent::BodyType bodyType)
    {
        switch (bodyType) {
        case Rigidbody2DComponent::BodyType::Static:
            return b2_staticBody;
        case Rigidbody2DComponent::BodyType::Dynamic:
            return b2_dynamicBody;
        case Rigidbody2DComponent::BodyType::Kinematic:
            return b2_kinematicBody;
        }

        TB_CORE_ASSERT(false, "Unknown body type");
        return b2_staticBody;
    }

    inline Rigidbody2DComponent::BodyType Rigidbody2DTypeFromBox2DBody(b2BodyType bodyType)
    {
        switch (bodyType) {
        case b2_staticBody:
            return Rigidbody2DComponent::BodyType::Static;
        case b2_dynamicBody:
            return Rigidbody2DComponent::BodyType::Dynamic;
        case b2_kinematicBody:
            return Rigidbody2DComponent::BodyType::Kinematic;
        }

        TB_CORE_ASSERT(false, "Unknown body type");
        return Rigidbody2DComponent::BodyType::Static;
    }
}

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

class Physisc2D {
public:
    Physisc2D();
    ~Physisc2D();

    static void InitWorld(Scene* scene, glm::vec2& gravity);
    static void UpdateWorld(float ts, int32_t velocityIterations, int32_t positionIterations);

    static b2World* GetPhysicsWorld();

private:
    b2World* m_PhysicsWorld = nullptr;
    Physics2DContactListener* m_PhysicsContactListener = nullptr;
    Scene* m_Scene = nullptr;

private:
    static Physisc2D* s_Instance;
};

}
