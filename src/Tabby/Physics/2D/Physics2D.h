#pragma once
#include <Tabby/Physics/2D/Physics2DTypes.h>

#include <box2d/box2d.h>
#include <TaskScheduler.h>

namespace Tabby {

class Scene;
class Physics2DTask;

class Physics2D {
public:
    Physics2D();
    ~Physics2D();

    static void Init();
    static void Init(const Vector2& gravity);
    static void UpdateWorld();

    static uint8_t GetSubstepCount();
    static void SetSubstepCount(uint8_t substepCount);

    static Vector2 GetGravity();
    static void SetGravity(Vector2 gravity);

    static RaycastHit2D RayCast(const Vector2& origin, const Vector2& direction, float distance, RaycastFilter2D raycastFilter, int minDepth, int maxDepth);
    static RaycastHit2D RayCast(const Vector2& origin, const Vector2& direction, float distance, RaycastFilter2D raycastFilter);
    static RaycastHit2D RayCast(const Vector2& origin, const Vector2& direction, float distance);
    static RaycastHit2D RayCast(const Vector2& origin, const Vector2& direction);

    static RaycastHit2D BoxCast(const Vector2& boxSize, const Vector2& origin, const Vector2& direction, float distance, RaycastFilter2D raycastFilter, int minDepth, int maxDepth);
    static RaycastHit2D BoxCast(const Vector2& boxSize, const Vector2& origin, const Vector2& direction, float distance, RaycastFilter2D raycastFilter);
    static RaycastHit2D BoxCast(const Vector2& boxSize, const Vector2& origin, const Vector2& direction, float distance);
    static RaycastHit2D BoxCast(const Vector2& boxSize, const Vector2& origin, const Vector2& direction);

    static RaycastHit2D CapsuleCast(const Vector2& point1, const Vector2& point2, float radius, const Vector2& origin, const Vector2& direction, float distance, RaycastFilter2D raycastFilter, int minDepth, int maxDepth);
    static RaycastHit2D CapsuleCast(const Vector2& point1, const Vector2& point2, float radius, const Vector2& origin, const Vector2& direction, float distance, RaycastFilter2D raycastFilter);
    static RaycastHit2D CapsuleCast(const Vector2& point1, const Vector2& point2, float radius, const Vector2& origin, const Vector2& direction, float distance);
    static RaycastHit2D CapsuleCast(const Vector2& point1, const Vector2& point2, float radius, const Vector2& origin, const Vector2& direction);

    static RaycastHit2D CircleCast(float radius, const Vector2& origin, const Vector2& direction, float distance, RaycastFilter2D raycastFilter, int minDepth, int maxDepth);
    static RaycastHit2D CircleCast(float radius, const Vector2& origin, const Vector2& direction, float distance, RaycastFilter2D raycastFilter);
    static RaycastHit2D CircleCast(float radius, const Vector2& origin, const Vector2& direction, float distance);
    static RaycastHit2D CircleCast(float radius, const Vector2& origin, const Vector2& direction);

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
    static void* Physics2DEnqueueTask(b2TaskCallback* task, int32_t itemCount, int32_t minRange, void* taskContext, void* userContext);
    static void Physics2DFinishTask(void* taskPtr, void* userContext);

private:
    b2WorldId m_PhysicsWorld;
    uint8_t m_SubstepCount = 6;
    std::queue<BodyInfo2D> bodyInitQueue;
    std::queue<ShapeInfo2D> shapeInitQueue;
    std::queue<ShapeInfo2D> shapeUpdateQueue;

    enki::TaskScheduler m_TaskScheduler;
    int32_t m_TaskCount;
    int m_ThreadCount;

    bool queueEmpty = true;

private:
    static Physics2D* s_Instance;
    friend class World;
    friend BoxCollider2DComponent;
    friend CircleCollider2DComponent;
    friend CapsuleCollider2DComponent;
    friend SegmentCollider2DComponent;
    // friend void Physics2DFinishTask(void* taskPtr, void* userContext);
    // friend void* Physics2DEnqueueTask(b2TaskCallback* task, int32_t itemCount, int32_t minRange, void* taskContext, void* userContext);
};

class Physics2DTask : public enki::ITaskSet {
public:
    Physics2DTask() = default;

    void ExecuteRange(enki::TaskSetPartition range, uint32_t threadIndex) override
    {
        m_task(range.start, range.end, threadIndex, m_taskContext);
    }

    b2TaskCallback* m_task = nullptr;
    void* m_taskContext = nullptr;
};

}
