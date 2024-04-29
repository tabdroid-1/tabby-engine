#pragma once

#include <Tabby/Physics/2D/Physics2DTypes.h>
#include <box2d/box2d.h>

namespace Tabby {

class Scene;

class Physisc2D {
public:
    Physisc2D();
    ~Physisc2D();

    static void InitWorld(glm::vec2& gravity);
    static void UpdateWorld(float ts, int32_t subStepCount);

    static RaycastHit2D Raycast(const glm::vec2& origin, const glm::vec2& direction, float distance, int minDepth, int maxDepth);
    static RaycastHit2D Raycast(const glm::vec2& origin, const glm::vec2& direction, float distance);
    static RaycastHit2D Raycast(const glm::vec2& origin, const glm::vec2& direction);

    // static RaycastHit2D Raycast(const glm::vec2& origin, const glm::vec2& direction, float distance, int minDepth, int maxDepth);
    // static RaycastHit2D Raycast(const glm::vec2& origin, const glm::vec2& direction, float distance);
    // static RaycastHit2D Raycast(const glm::vec2& origin, const glm::vec2& direction);

    static b2WorldId GetPhysicsWorld();

private:
    static void EnqueueBodyInit(BodyInfo2D bodyInfo);
    static void EnqueueShapeInit(ShapeInfo2D shapeInfo);
    static void EnqueueShapeUpdate(ShapeInfo2D shapeInfo);

    static void ProcessBodyInitQueue();
    static void ProcessShapeInitQueue();
    static void ProcessShapeUpdateQueue();
    static void ProcessEvents();

    static bool IsQueueEmpty() { return s_Instance->queueEmpty; }

private:
    static float Physics2DRaycastCallback(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void* context);

private:
    b2WorldId m_PhysicsWorld;
    std::queue<BodyInfo2D> bodyInitQueue;
    std::queue<ShapeInfo2D> shapeInitQueue;
    std::queue<ShapeInfo2D> shapeUpdateQueue;

    bool queueEmpty = true;

private:
    static Physisc2D* s_Instance;
    friend class Scene;
    friend BoxCollider2DComponent;
    friend CircleCollider2DComponent;
    friend CapsuleCollider2DComponent;
};

}

// #pragma once
//
// #include <Tabby/Physics/2D/Physics2DTypes.h>
// #include <box2d/box2d.h>
//
// namespace Tabby {
//
// class Scene;
//
// class Physisc2D {
// public:
//     Physisc2D();
//     ~Physisc2D();
//
//     static void InitWorld(glm::vec2& gravity);
//     static void UpdateWorld(float ts, int32_t subStepCount);
//
//     static RaycastHit2D Raycast(const glm::vec2& origin, const glm::vec2& direction, float distance, int minDepth, int maxDepth);
//     static RaycastHit2D Raycast(const glm::vec2& origin, const glm::vec2& direction, float distance);
//     static RaycastHit2D Raycast(const glm::vec2& origin, const glm::vec2& direction);
//
//     static b2WorldId GetPhysicsWorld();
//
// private:
//     static void EnqueueBody(BodyInfo2D bodyInfo);
//     static void EnqueueFixture(FixtureInfo2D fixtureInfo);
//     static void ProcessBodyQueue();
//     static void ProcessFixtureQueue();
//     static void ProcessEvents();
//     static bool IsQueueEmpty() { return s_Instance->queueEmpty; }
//
// private:
//     static float Physics2DRaycastCallback(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void* context);
//
// private:
//     b2WorldId m_PhysicsWorld;
//     std::queue<BodyInfo2D> bodyQueue;
//     std::queue<FixtureInfo2D> fixtureQueue;
//
//     bool queueEmpty = true;
//
// private:
//     static Physisc2D* s_Instance;
//     friend class Scene;
// };
//
// }
