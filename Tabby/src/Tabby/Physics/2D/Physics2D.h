#pragma once

#include "box2d/b2_world.h"
#include "glm/fwd.hpp"
#include <Tabby/Core/Assert.h>
#include <Tabby/Physics/2D/Physics2DTypes.h>
#include <Tabby/Scene/Entity.h>

namespace Tabby {

class Scene;
class Entity;
class Physics2DContactListener;
class Physics2DRaycastCallback;

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
    static void EnqueueBody(BodyInfo2D bodyInfo);
    static void EnqueueFixture(FixtureInfo2D fixtureInfo);
    static void ProcessBodyQueue();
    static void ProcessFixtureQueue();
    static bool IsQueueEmpty() { return s_Instance->queueEmpty; }

private:
    b2World* m_PhysicsWorld = nullptr;
    Physics2DContactListener* m_PhysicsContactListener = nullptr;
    Physics2DRaycastCallback* m_PhysicsRaycastCallback = nullptr;
    std::queue<BodyInfo2D> bodyQueue;
    std::queue<FixtureInfo2D> fixtureQueue;

    bool queueEmpty = true;

private:
    static Physisc2D* s_Instance;
    friend class Scene;
};

}
