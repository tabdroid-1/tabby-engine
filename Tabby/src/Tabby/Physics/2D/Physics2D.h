#pragma once

#include "Tabby/Scene/Components.h"
#include <tbpch.h>

#include "box2d/b2_world.h"
#include "glm/fwd.hpp"
#include <Tabby/Core/Assert.h>
#include <Tabby/Scene/Entity.h>

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
class Entity;
class Physics2DContactListener;
class Physics2DRaycastCallback;

// Callback Data Structs
struct ContactCallback {
    Entity entity;
    Tabby::TransformComponent* transform;
    Tabby::Rigidbody2DComponent* rigidbody;
};

struct RaycastHit2D {
    Entity entity;
    Tabby::TransformComponent* transform;
    Tabby::Rigidbody2DComponent* rigidbody;
    glm::vec2 point = { 0.0f, 0.0f };
    glm::vec2 normal = { 0.0f, 0.0f };
    float distance;
    float fraction;
};
//----------------------

class Physisc2D {
public:
    Physisc2D();
    ~Physisc2D();

    static void InitWorld(glm::vec2& gravity);
    static void UpdateWorld(float ts, int32_t velocityIterations, int32_t positionIterations);

    static RaycastHit2D Raycast(const glm::vec2& origin, const glm::vec2& direction, float distance, int minDepth, int maxDepth);
    static RaycastHit2D Raycast(const glm::vec2& origin, const glm::vec2& direction, float distance);
    static RaycastHit2D Raycast(const glm::vec2& origin, const glm::vec2& direction);

    static b2World* GetPhysicsWorld();

private:
    static void EnqueueBody(Entity entity, b2BodyDef* bodyDef);
    static void EnqueueFixture(Entity entity, b2FixtureDef* fixtureDef);
    static void ProcessBodyQueue();
    static void ProcessFixtureQueue();
    static bool IsQueueEmpty() { return s_Instance->queueEmpty; }

private:
    b2World* m_PhysicsWorld = nullptr;
    Physics2DContactListener* m_PhysicsContactListener = nullptr;
    Physics2DRaycastCallback* m_PhysicsRaycastCallback = nullptr;
    std::queue<std::pair<Entity, b2BodyDef*>> bodyQueue;
    std::queue<std::pair<Entity, b2FixtureDef*>> fixtureQueue;

    bool queueEmpty = true;

private:
    static Physisc2D* s_Instance;
    friend class Scene;
};

}
