#include <Tabby/Physics/2D/Physics2DTypes.h>
#include <Tabby/Physics/2D/Physics2DUtil.h>
#include <Tabby/Threading/TaskScheduler.h>
#include <Tabby/Physics/2D/Physics2D.h>
#include <Tabby/Core/Time/Time.h>
#include <Tabby/Math/Math.h>

#include <box2d/box2d.h>
#include <entt.hpp>

namespace Tabby {

static bool Physics2DPreSolve(b2ShapeId shapeIdA, b2ShapeId shapeIdB, b2Manifold* manifold, void* context);
void* Physics2DEnqueueTask(b2TaskCallback* task, int32_t itemCount, int32_t minRange, void* taskContext, void* userContext);
void Physics2DFinishTask(void* taskPtr, void* userContext);

Physics2D* Physics2D::s_Instance = nullptr;

Physics2D::Physics2D()
{
    TB_PROFILE_SCOPE_NAME("Tabby::Physics2D::Constructor");

    if (!s_Instance)
        s_Instance = this;

    m_PhysicsWorld = b2_nullWorldId;
}

Physics2D::~Physics2D()
{
    TB_PROFILE_SCOPE_NAME("Tabby::Physics2D::Destructor");

    b2DestroyWorld(m_PhysicsWorld);
    m_PhysicsWorld = b2_nullWorldId;
}

void Physics2D::Init()
{
    TB_PROFILE_SCOPE_NAME("Tabby::Physics2D::Init");

    if (!s_Instance)
        s_Instance = new Physics2D();

    if (B2_IS_NULL(s_Instance->m_PhysicsWorld)) {
        s_Instance->m_TaskScheduler.Initialize((int)(enki::GetNumHardwareThreads() / 2));
        b2WorldDef worldDef = b2DefaultWorldDef();
        worldDef.gravity = { 0.0f, -9.81f };
        worldDef.workerCount = (int)(enki::GetNumHardwareThreads() / 2);
        worldDef.enqueueTask = Physics2DEnqueueTask;
        worldDef.finishTask = Physics2DFinishTask;
        worldDef.userTaskContext = s_Instance;
        worldDef.enableSleep = true;
        s_Instance->m_PhysicsWorld = b2CreateWorld(&worldDef);
        b2World_SetPreSolveCallback(s_Instance->m_PhysicsWorld, Physics2DPreSolve, nullptr);
    }
}

void Physics2D::Init(const Vector2& gravity)
{
    TB_PROFILE_SCOPE_NAME("Tabby::Physics2D::Init");

    if (!s_Instance)
        s_Instance = new Physics2D();

    if (B2_IS_NULL(s_Instance->m_PhysicsWorld)) {

        b2WorldDef worldDef = b2DefaultWorldDef();
        worldDef.gravity = { gravity.x, gravity.y };
        s_Instance->m_PhysicsWorld = b2CreateWorld(&worldDef);
    }
}

void Physics2D::UpdateWorld()
{
    TB_PROFILE_SCOPE_NAME("Tabby::Physics2D::UpdateWorld");

    ProcessBodyInitQueue();
    ProcessShapeInitQueue();
    ProcessShapeUpdateQueue();

    TB_CORE_ASSERT_TAGGED(s_Instance, "Physics2D: Instance have to be initialized first!");
    b2World_Step(s_Instance->m_PhysicsWorld, Time::GetDeltaTime(), s_Instance->m_SubstepCount);

    ProcessEvents();
}

uint8_t Physics2D::GetSubstepCount()
{
    TB_PROFILE_SCOPE_NAME("Tabby::Physics2D::GetSubstepCount");

    return s_Instance->m_SubstepCount;
}

void Physics2D::SetSubstepCount(uint8_t substepCount)
{
    TB_PROFILE_SCOPE_NAME("Tabby::Physics2D::SetSubstepCount");

    if (substepCount < 1) {
        TB_CORE_ERROR("Physics2D: SubstepCount can not be lower than 1!");
        return;
    }
    s_Instance->m_SubstepCount = substepCount;
}

Vector2 Physics2D::GetGravity()
{
    TB_PROFILE_SCOPE_NAME("Tabby::Physics2D::GetGravity");

    Vector2 gravity;
    if (B2_IS_NULL(s_Instance->m_PhysicsWorld)) {

        const auto& b2grav = b2World_GetGravity(s_Instance->m_PhysicsWorld);
        gravity = { b2grav.x, b2grav.y };
    }
    return gravity;
}

void Physics2D::SetGravity(Vector2 gravity)
{
    TB_PROFILE_SCOPE_NAME("Tabby::Physics2D::SetGravity");

    if (B2_IS_NULL(s_Instance->m_PhysicsWorld))
        b2World_SetGravity(s_Instance->m_PhysicsWorld, { gravity.x, gravity.y });
    else
        TB_CORE_WARN("Physics2D: Physics world is null. Gravity is not set!");
}

b2WorldId Physics2D::GetPhysicsWorld()
{
    TB_PROFILE_SCOPE_NAME("Tabby::Physics2D::GetPhysicsWorld");

    if (s_Instance && B2_IS_NON_NULL(s_Instance->m_PhysicsWorld))
        return s_Instance->m_PhysicsWorld;
    else {
        TB_CORE_ERROR("Physics2D: Physics world is not initialized!");
        return b2_nullWorldId;
    }
}

RaycastHit2D Physics2D::RayCast(const Vector2& origin, const Vector2& direction, float distance, RaycastFilter2D raycastFilter, int minDepth, int maxDepth)
{
    TB_PROFILE_SCOPE_NAME("Tabby::Physics2D::RayCast");

    RaycastHit2D tempRayCastHit;
    tempRayCastHit.RaycastFilter = raycastFilter;
    tempRayCastHit.Origin = origin;

    b2CastResultFcn* fcn = Physics2DRaycastCallback;
    b2Vec2 box2DOrigin = { origin.x, origin.y };
    // b2Vec2 box2DDirection = { direction.x, direction.y };
    b2Vec2 box2DDestination = { direction.x * distance, direction.y * distance };
    b2World_CastRay(s_Instance->m_PhysicsWorld, box2DOrigin, box2DDestination, { raycastFilter.GetCollisionLayer(), raycastFilter.GetCollisionMask() }, fcn, &tempRayCastHit);

    return tempRayCastHit;
}

RaycastHit2D Physics2D::RayCast(const Vector2& origin, const Vector2& direction, float distance, RaycastFilter2D raycastFilter)
{
    return RayCast(origin, direction, distance, raycastFilter, 0, std::numeric_limits<int>::max());
}

RaycastHit2D Physics2D::RayCast(const Vector2& origin, const Vector2& direction, float distance)
{
    return RayCast(origin, direction, distance, RaycastFilter2D(), 0, std::numeric_limits<int>::max());
}

RaycastHit2D Physics2D::RayCast(const Vector2& origin, const Vector2& direction)
{
    return RayCast(origin, direction, 2000.0f, RaycastFilter2D(), 0, std::numeric_limits<int>::max());
}

RaycastHit2D Physics2D::BoxCast(const Vector2& boxSize, const Vector2& origin, const Vector2& direction, float distance, RaycastFilter2D raycastFilter, int minDepth, int maxDepth)
{
    TB_PROFILE_SCOPE_NAME("Tabby::Physics2D::BoxCast");

    RaycastHit2D tempRayCastHit;

    tempRayCastHit.Origin = origin;

    b2Polygon box = b2MakeBox(boxSize.x, boxSize.y);

    b2CastResultFcn* fcn = Physics2DRaycastCallback;
    b2Transform box2DOrigin = { { origin.x, origin.y }, b2MakeRot(0.0f) };
    // b2Vec2 box2DDirection = { direction.x, direction.y };
    b2Vec2 box2DDestination = { direction.x * distance, direction.y * distance };

    b2World_CastPolygon(s_Instance->m_PhysicsWorld, &box, box2DOrigin, box2DDestination, { raycastFilter.GetCollisionLayer(), raycastFilter.GetCollisionMask() }, fcn, &tempRayCastHit);

    return tempRayCastHit;
}

RaycastHit2D Physics2D::BoxCast(const Vector2& boxSize, const Vector2& origin, const Vector2& direction, float distance, RaycastFilter2D raycastFilter)
{
    return BoxCast(boxSize, origin, direction, distance, RaycastFilter2D(), 0, std::numeric_limits<int>::max());
}

RaycastHit2D Physics2D::BoxCast(const Vector2& boxSize, const Vector2& origin, const Vector2& direction, float distance)
{
    return BoxCast(boxSize, origin, direction, distance, RaycastFilter2D(), 0, std::numeric_limits<int>::max());
}

RaycastHit2D Physics2D::BoxCast(const Vector2& boxSize, const Vector2& origin, const Vector2& direction)
{
    return BoxCast(boxSize, origin, direction, 2000.0f, RaycastFilter2D(), 0, std::numeric_limits<int>::max());
}

RaycastHit2D Physics2D::CapsuleCast(const Vector2& point1, const Vector2& point2, float radius, const Vector2& origin, const Vector2& direction, float distance, RaycastFilter2D raycastFilter, int minDepth, int maxDepth)
{
    TB_PROFILE_SCOPE_NAME("Tabby::Physics2D::CapsuleCast");

    RaycastHit2D tempRayCastHit;

    tempRayCastHit.Origin = origin;

    b2Capsule capsule = { { point1.x, point1.y }, { point2.x, point2.y }, radius };

    b2CastResultFcn* fcn = Physics2DRaycastCallback;
    b2Transform box2DOrigin = { { origin.x, origin.y }, b2MakeRot(0.0f) };
    // b2Vec2 box2DDirection = { direction.x, direction.y };
    b2Vec2 box2DDestination = { direction.x * distance, direction.y * distance };

    b2World_CastCapsule(s_Instance->m_PhysicsWorld, &capsule, box2DOrigin, box2DDestination, { raycastFilter.GetCollisionLayer(), raycastFilter.GetCollisionMask() }, fcn, &tempRayCastHit);

    return tempRayCastHit;
}

RaycastHit2D Physics2D::CapsuleCast(const Vector2& point1, const Vector2& point2, float radius, const Vector2& origin, const Vector2& direction, float distance, RaycastFilter2D raycastFilter)
{

    return CapsuleCast(point1, point2, radius, origin, direction, distance, RaycastFilter2D(), 0, std::numeric_limits<int>::max());
}

RaycastHit2D Physics2D::CapsuleCast(const Vector2& point1, const Vector2& point2, float radius, const Vector2& origin, const Vector2& direction, float distance)
{
    return CapsuleCast(point1, point2, radius, origin, direction, distance, RaycastFilter2D(), 0, std::numeric_limits<int>::max());
}

RaycastHit2D Physics2D::CapsuleCast(const Vector2& point1, const Vector2& point2, float radius, const Vector2& origin, const Vector2& direction)
{
    return CapsuleCast(point1, point2, radius, origin, direction, 2000.0f, RaycastFilter2D(), 0, std::numeric_limits<int>::max());
}

RaycastHit2D Physics2D::CircleCast(float radius, const Vector2& origin, const Vector2& direction, float distance, RaycastFilter2D raycastFilter, int minDepth, int maxDepth)
{
    TB_PROFILE_SCOPE_NAME("Tabby::Physics2D::CircleCast");

    RaycastHit2D tempRayCastHit;

    tempRayCastHit.Origin = origin;

    b2Circle circle = { { 0.0f, 0.0f }, radius };

    b2CastResultFcn* fcn = Physics2DRaycastCallback;
    b2Transform box2DOrigin = { { origin.x, origin.y }, b2MakeRot(0.0f) };
    // b2Vec2 box2DDirection = { direction.x, direction.y };
    b2Vec2 box2DDestination = { direction.x * distance, direction.y * distance };

    b2World_CastCircle(s_Instance->m_PhysicsWorld, &circle, box2DOrigin, box2DDestination, { raycastFilter.GetCollisionLayer(), raycastFilter.GetCollisionMask() }, fcn, &tempRayCastHit);

    return tempRayCastHit;
}

RaycastHit2D Physics2D::CircleCast(float radius, const Vector2& origin, const Vector2& direction, float distance, RaycastFilter2D raycastFilter)
{
    return CircleCast(radius, origin, direction, distance, RaycastFilter2D(), 0, std::numeric_limits<int>::max());
}

RaycastHit2D Physics2D::CircleCast(float radius, const Vector2& origin, const Vector2& direction, float distance)
{
    return CircleCast(radius, origin, direction, distance, RaycastFilter2D(), 0, std::numeric_limits<int>::max());
}

RaycastHit2D Physics2D::CircleCast(float radius, const Vector2& origin, const Vector2& direction)
{
    return CircleCast(radius, origin, direction, 2000.0f, RaycastFilter2D(), 0, std::numeric_limits<int>::max());
}

void Physics2D::EnqueueBodyInit(BodyInfo2D bodyInfo)
{
    TB_PROFILE_SCOPE_NAME("Tabby::Physics2D::EnqueueBodyInit");

    s_Instance->bodyInitQueue.push(bodyInfo);
}

void Physics2D::EnqueueShapeInit(ShapeInfo2D shapeInfo)
{
    TB_PROFILE_SCOPE_NAME("Tabby::Physics2D::EnqueueShapeInit");

    s_Instance->shapeInitQueue.push(shapeInfo);
}

void Physics2D::EnqueueShapeUpdate(ShapeInfo2D shapeInfo)
{
    TB_PROFILE_SCOPE_NAME("Tabby::Physics2D::EnqueueShapeUpdate");

    s_Instance->shapeUpdateQueue.push(shapeInfo);
}

void Physics2D::ProcessBodyInitQueue()
{
    TB_PROFILE_SCOPE_NAME("Tabby::Physics2D::ProcessBodyInitQueue");

    while (!s_Instance->bodyInitQueue.empty()) {

        s_Instance->queueEmpty = false;

        BodyInfo2D bodyInfo = s_Instance->bodyInitQueue.front();
        auto& transform = bodyInfo.BodyEntity.GetComponent<TransformComponent>();
        auto& rb2d = bodyInfo.BodyEntity.GetComponent<Rigidbody2DComponent>();

        // --------- Create Body def ---------
        BodyUserData2D* bodyUserData = new BodyUserData2D { bodyInfo.BodyEntity };

        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = Utils::Rigidbody2DTypeToBox2DBody(rb2d.Type);
        bodyDef.position = { transform.GetWorldPosition().x, transform.GetWorldPosition().y };
        bodyDef.rotation = b2MakeRot(transform.GetWorldRotation().z * Math::DEG2RAD);
        bodyDef.userData = static_cast<void*>(bodyUserData);

        // --------- Create Body ---------
        rb2d.RuntimeBodyId = b2CreateBody(s_Instance->m_PhysicsWorld, &bodyDef);
        b2Body_SetFixedRotation(rb2d.RuntimeBodyId, rb2d.FixedRotation);

        s_Instance->bodyInitQueue.pop();
    }

    if (s_Instance->bodyInitQueue.empty() && s_Instance->shapeInitQueue.empty() && s_Instance->shapeUpdateQueue.empty())
        s_Instance->queueEmpty = true;
}

void Physics2D::ProcessShapeInitQueue()
{
    TB_PROFILE_SCOPE_NAME("Tabby::Physics2D::ProcessShapeInitQueue");

    while (!s_Instance->shapeInitQueue.empty()) {
        s_Instance->queueEmpty = false;

        ShapeInfo2D shapeInfo = s_Instance->shapeInitQueue.front();
        auto& transform = shapeInfo.ShapeEntity.GetComponent<TransformComponent>();

        // Check if entity it self has Rigidbody2DComponent. If there is, collider will be added to Rigidbody2DComponent of same entity.
        // If there is not, function will check its parent if it has Rigidbody2DComponent. And this will go on untill it finds an entity Rigidbody2DComponent;
        // This is is because an entity can have one component of same type. User can create More colliders by creating child entities.
        entt::entity rb2dEntity { entt::null };
        if (shapeInfo.ShapeEntity.HasComponent<Rigidbody2DComponent>()) {
            rb2dEntity = shapeInfo.ShapeEntity;
        } else {

            auto FindParentRigidbody = [](auto&& self, entt::entity parentEntity) -> entt::entity {
                auto& hierarchyNode = Entity(parentEntity).GetComponent<HierarchyNodeComponent>();

                if (Entity(hierarchyNode.Parent.second).HasComponent<Rigidbody2DComponent>()) {
                    return hierarchyNode.Parent.second;
                } else {
                    if (hierarchyNode.Parent.first.Valid())
                        return self(self, hierarchyNode.Parent.second);
                    else
                        return entt::null;
                }
            };

            rb2dEntity = FindParentRigidbody(FindParentRigidbody, shapeInfo.ShapeEntity);
        }

        if (rb2dEntity == entt::null) {
            s_Instance->shapeInitQueue.pop();
            continue;
        }

        if (shapeInfo.ColliderType == ColliderType2D::Box) {
            auto& bc2d = shapeInfo.ShapeEntity.GetComponent<BoxCollider2DComponent>();
            ShapeUserData2D* userData = new ShapeUserData2D { shapeInfo.ShapeEntity, rb2dEntity, ColliderType2D::Box };

            // --------- Create box collider def ---------
            b2Polygon box = b2MakeOffsetBox(bc2d.Size.x * transform.scale.x, bc2d.Size.y * transform.scale.y, { bc2d.Offset.x, bc2d.Offset.y }, bc2d.Angle);

            b2ShapeDef shapeDef = b2DefaultShapeDef();
            shapeDef.density = bc2d.Density;
            shapeDef.friction = bc2d.Friction;
            shapeDef.restitution = bc2d.Restitution;
            shapeDef.isSensor = bc2d.IsSensor;
            shapeDef.enableSensorEvents = bc2d.EnableSensorEvents;
            shapeDef.enableContactEvents = bc2d.EnableContactEvents;
            shapeDef.enablePreSolveEvents = bc2d.EnablePreSolveEvents;
            shapeDef.filter.categoryBits = bc2d.CollisionLayer;
            shapeDef.filter.maskBits = bc2d.CollisionMask;
            shapeDef.userData = static_cast<void*>(userData);

            // --------- Create box collider in body ---------
            bc2d.RuntimeShapeId = b2CreatePolygonShape(Entity(rb2dEntity).GetComponent<Rigidbody2DComponent>().RuntimeBodyId, &shapeDef, &box);
            bc2d.QueuedForInitialization = false;

        } else if (shapeInfo.ColliderType == ColliderType2D::Circle) {
            auto& cc2d = shapeInfo.ShapeEntity.GetComponent<CircleCollider2DComponent>();
            ShapeUserData2D* userData = new ShapeUserData2D { shapeInfo.ShapeEntity, rb2dEntity, ColliderType2D::Circle };

            // --------- Create circle collider def ---------
            b2Circle circleShape = { { cc2d.Offset.x, cc2d.Offset.y }, cc2d.Radius };

            b2ShapeDef shapeDef = b2DefaultShapeDef();
            shapeDef.density = cc2d.Density;
            shapeDef.friction = cc2d.Friction;
            shapeDef.restitution = cc2d.Restitution;
            shapeDef.isSensor = cc2d.IsSensor;
            shapeDef.enableSensorEvents = cc2d.EnableSensorEvents;
            shapeDef.enableContactEvents = cc2d.EnableContactEvents;
            shapeDef.enablePreSolveEvents = cc2d.EnablePreSolveEvents;
            shapeDef.filter.categoryBits = cc2d.CollisionLayer;
            shapeDef.filter.maskBits = cc2d.CollisionMask;
            shapeDef.userData = static_cast<void*>(userData);

            // --------- Create circle collider in body ---------
            cc2d.RuntimeShapeId = b2CreateCircleShape(Entity(rb2dEntity).GetComponent<Rigidbody2DComponent>().RuntimeBodyId, &shapeDef, &circleShape);
            cc2d.QueuedForInitialization = false;
        } else if (shapeInfo.ColliderType == ColliderType2D::Capsule) {
            auto& cc2d = shapeInfo.ShapeEntity.GetComponent<CapsuleCollider2DComponent>();
            ShapeUserData2D* userData = new ShapeUserData2D { shapeInfo.ShapeEntity, rb2dEntity, ColliderType2D::Capsule };

            // --------- Create circle collider def ---------

            b2Vec2 center1 = { cc2d.center1.x, cc2d.center1.y };
            b2Vec2 center2 = { cc2d.center2.x, cc2d.center2.y };
            b2Capsule capsuleShape = { center1, center2, cc2d.Radius };

            b2ShapeDef shapeDef = b2DefaultShapeDef();
            shapeDef.density = cc2d.Density;
            shapeDef.friction = cc2d.Friction;
            shapeDef.restitution = cc2d.Restitution;
            shapeDef.isSensor = cc2d.IsSensor;
            shapeDef.enableSensorEvents = cc2d.EnableSensorEvents;
            shapeDef.enableContactEvents = cc2d.EnableContactEvents;
            shapeDef.enablePreSolveEvents = cc2d.EnablePreSolveEvents;
            shapeDef.filter.categoryBits = cc2d.CollisionLayer;
            shapeDef.filter.maskBits = cc2d.CollisionMask;
            shapeDef.userData = static_cast<void*>(userData);

            // --------- Create circle collider in body ---------
            cc2d.RuntimeShapeId = b2CreateCapsuleShape(Entity(rb2dEntity).GetComponent<Rigidbody2DComponent>().RuntimeBodyId, &shapeDef, &capsuleShape);
            cc2d.QueuedForInitialization = false;
        } else if (shapeInfo.ColliderType == ColliderType2D::Polygon) {
            auto& pc2d = shapeInfo.ShapeEntity.GetComponent<PolygonCollider2DComponent>();
            ShapeUserData2D* userData = new ShapeUserData2D { shapeInfo.ShapeEntity, rb2dEntity, ColliderType2D::Polygon };

            // --------- Create circle collider def ---------

            std::vector<b2Vec2> points;
            for (const auto& point : pc2d.Points) {
                points.push_back({ point.x, point.y });
            }
            b2Hull hull = b2ComputeHull(points.data(), points.size());
            TB_CORE_VERIFY_TAGGED(hull.count != 0, "Hull computation failed!");
            b2Polygon polygon = b2MakePolygon(&hull, Math::RAD2DEG * pc2d.Radius);

            b2ShapeDef shapeDef = b2DefaultShapeDef();
            shapeDef.density = pc2d.Density;
            shapeDef.friction = pc2d.Friction;
            shapeDef.restitution = pc2d.Restitution;
            shapeDef.isSensor = pc2d.IsSensor;
            shapeDef.enableSensorEvents = pc2d.EnableSensorEvents;
            shapeDef.enableContactEvents = pc2d.EnableContactEvents;
            shapeDef.enablePreSolveEvents = pc2d.EnablePreSolveEvents;
            shapeDef.filter.categoryBits = pc2d.CollisionLayer;
            shapeDef.filter.maskBits = pc2d.CollisionMask;
            shapeDef.userData = static_cast<void*>(userData);

            // --------- Create circle collider in body ---------
            pc2d.RuntimeShapeId = b2CreatePolygonShape(Entity(rb2dEntity).GetComponent<Rigidbody2DComponent>().RuntimeBodyId, &shapeDef, &polygon);
            pc2d.QueuedForInitialization = false;
        } else if (shapeInfo.ColliderType == ColliderType2D::Segment) {
            auto& sc2d = shapeInfo.ShapeEntity.GetComponent<SegmentCollider2DComponent>();
            ShapeUserData2D* userData = new ShapeUserData2D { shapeInfo.ShapeEntity, rb2dEntity, ColliderType2D::Segment };

            // --------- Create circle collider def ---------

            b2Vec2 point1 = { sc2d.point1.x, sc2d.point1.y };
            b2Vec2 point2 = { sc2d.point2.x, sc2d.point2.y };
            b2Segment segmentShape = { point1, point2 };

            b2ShapeDef shapeDef = b2DefaultShapeDef();
            shapeDef.density = sc2d.Density;
            shapeDef.friction = sc2d.Friction;
            shapeDef.restitution = sc2d.Restitution;
            shapeDef.isSensor = sc2d.IsSensor;
            shapeDef.enableSensorEvents = sc2d.EnableSensorEvents;
            shapeDef.enableContactEvents = sc2d.EnableContactEvents;
            shapeDef.enablePreSolveEvents = sc2d.EnablePreSolveEvents;
            shapeDef.filter.categoryBits = sc2d.CollisionLayer;
            shapeDef.filter.maskBits = sc2d.CollisionMask;
            shapeDef.userData = static_cast<void*>(userData);

            // --------- Create circle collider in body ---------
            sc2d.RuntimeShapeId = b2CreateSegmentShape(Entity(rb2dEntity).GetComponent<Rigidbody2DComponent>().RuntimeBodyId, &shapeDef, &segmentShape);
            sc2d.QueuedForInitialization = false;
        }

        s_Instance->shapeInitQueue.pop();
    }

    if (s_Instance->bodyInitQueue.empty() && s_Instance->shapeInitQueue.empty() && s_Instance->shapeUpdateQueue.empty())
        s_Instance->queueEmpty = true;
}

void Physics2D::ProcessShapeUpdateQueue()
{
    TB_PROFILE_SCOPE_NAME("Tabby::Physics2D::ProcessShapeUpdateQueue");

    while (!s_Instance->shapeUpdateQueue.empty()) {
        s_Instance->queueEmpty = false;

        ShapeInfo2D shapeInfo = s_Instance->shapeUpdateQueue.front();
        auto& transform = shapeInfo.ShapeEntity.GetComponent<TransformComponent>();

        if (shapeInfo.ColliderType == ColliderType2D::Box) {
            auto& bc2d = shapeInfo.ShapeEntity.GetComponent<BoxCollider2DComponent>();

            // --------- Create box collider def ---------
            b2Polygon box = b2MakeOffsetBox(bc2d.Size.x * transform.scale.x, bc2d.Size.y * transform.scale.y, { bc2d.Offset.x, bc2d.Offset.y }, bc2d.Angle);

            b2Shape_SetPolygon(bc2d.RuntimeShapeId, &box);

            b2Filter filter = b2Shape_GetFilter(bc2d.RuntimeShapeId);
            filter.categoryBits = bc2d.CollisionLayer;
            filter.maskBits = bc2d.CollisionMask;
            b2Shape_SetFilter(bc2d.RuntimeShapeId, filter);

            bc2d.QueuedForInitialization = false;

        } else if (shapeInfo.ColliderType == ColliderType2D::Circle) {
            auto& cc2d = shapeInfo.ShapeEntity.GetComponent<CircleCollider2DComponent>();
            b2Circle circleShape = { { cc2d.Offset.x, cc2d.Offset.y }, cc2d.Radius };

            b2Shape_SetCircle(cc2d.RuntimeShapeId, &circleShape);

            b2Filter filter = b2Shape_GetFilter(cc2d.RuntimeShapeId);
            filter.categoryBits = cc2d.CollisionLayer;
            filter.maskBits = cc2d.CollisionMask;
            b2Shape_SetFilter(cc2d.RuntimeShapeId, filter);

            cc2d.QueuedForInitialization = false;

        } else if (shapeInfo.ColliderType == ColliderType2D::Capsule) {
            auto& cc2d = shapeInfo.ShapeEntity.GetComponent<CapsuleCollider2DComponent>();

            b2Vec2 center1 = { cc2d.center1.x, cc2d.center1.y };
            b2Vec2 center2 = { cc2d.center2.x, cc2d.center2.y };
            b2Capsule capsuleShape = { center1, center2, cc2d.Radius };

            // --------- Create circle collider in body ---------
            b2Shape_SetCapsule(cc2d.RuntimeShapeId, &capsuleShape);

            b2Filter filter = b2Shape_GetFilter(cc2d.RuntimeShapeId);
            filter.categoryBits = cc2d.CollisionLayer;
            filter.maskBits = cc2d.CollisionMask;
            b2Shape_SetFilter(cc2d.RuntimeShapeId, filter);

            cc2d.QueuedForInitialization = false;

        } else if (shapeInfo.ColliderType == ColliderType2D::Polygon) {
            auto& pc2d = shapeInfo.ShapeEntity.GetComponent<PolygonCollider2DComponent>();

            // --------- Create box collider def ---------

            std::vector<b2Vec2> points;
            for (const auto& point : pc2d.Points) {
                points.push_back({ point.x, point.y });
            }
            b2Hull hull = b2ComputeHull(points.data(), points.size());
            TB_CORE_VERIFY_TAGGED(hull.count != 0, "Hull computation failed!");
            b2Polygon polygon = b2MakePolygon(&hull, Math::RAD2DEG * pc2d.Radius);

            b2Shape_SetPolygon(pc2d.RuntimeShapeId, &polygon);

            b2Filter filter = b2Shape_GetFilter(pc2d.RuntimeShapeId);
            filter.categoryBits = pc2d.CollisionLayer;
            filter.maskBits = pc2d.CollisionMask;
            b2Shape_SetFilter(pc2d.RuntimeShapeId, filter);

            pc2d.QueuedForInitialization = false;

        } else if (shapeInfo.ColliderType == ColliderType2D::Segment) {
            auto& sc2d = shapeInfo.ShapeEntity.GetComponent<SegmentCollider2DComponent>();

            b2Vec2 point1 = { sc2d.point1.x, sc2d.point1.y };
            b2Vec2 point2 = { sc2d.point2.x, sc2d.point2.y };
            b2Segment segmentShape = { point1, point2 };

            // --------- Create circle collider in body ---------
            b2Shape_SetSegment(sc2d.RuntimeShapeId, &segmentShape);

            b2Filter filter = b2Shape_GetFilter(sc2d.RuntimeShapeId);
            filter.categoryBits = sc2d.CollisionLayer;
            filter.maskBits = sc2d.CollisionMask;
            b2Shape_SetFilter(sc2d.RuntimeShapeId, filter);

            sc2d.QueuedForInitialization = false;
        }

        s_Instance->shapeUpdateQueue.pop();
    }

    if (s_Instance->bodyInitQueue.empty() && s_Instance->shapeInitQueue.empty() && s_Instance->shapeUpdateQueue.empty())
        s_Instance->queueEmpty = true;
}

float Physics2D::Physics2DRaycastCallback(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void* context)
{
    TB_PROFILE_SCOPE_NAME("Tabby::Physics2D::Physics2DRaycastCallback");

    ShapeUserData2D* userData = static_cast<ShapeUserData2D*>(b2Shape_GetUserData(shapeId));

    static_cast<RaycastHit2D*>(context)->Point = { point.x, point.y };
    static_cast<RaycastHit2D*>(context)->Normal = { normal.x, normal.y };
    static_cast<RaycastHit2D*>(context)->Fraction = fraction;

    static_cast<RaycastHit2D*>(context)->HitEntity = userData->BodyEntity;
    static_cast<RaycastHit2D*>(context)->Transform = &userData->BodyEntity.GetComponent<TransformComponent>();
    static_cast<RaycastHit2D*>(context)->Rigidbody = &userData->BodyEntity.GetComponent<Rigidbody2DComponent>();
    static_cast<RaycastHit2D*>(context)->Distance = glm::distance(static_cast<RaycastHit2D*>(context)->Origin, { point.x, point.y });

    return fraction;
}

void Physics2D::ProcessEvents()
{
    TB_PROFILE_SCOPE_NAME("Tabby::Physics2D::ProcessEvents");

    b2ContactEvents contactEvents = b2World_GetContactEvents(s_Instance->m_PhysicsWorld);
    for (int i = 0; i < contactEvents.beginCount; ++i) {
        b2ContactBeginTouchEvent event = contactEvents.beginEvents[i];

        // --- Process Begin Touch Event -----------------

        b2BodyId bodyIdA = b2Shape_GetBody(event.shapeIdA);
        b2BodyId bodyIdB = b2Shape_GetBody(event.shapeIdB);

        ShapeUserData2D* shapeDataA = static_cast<ShapeUserData2D*>(b2Shape_GetUserData(event.shapeIdA));
        ShapeUserData2D* shapeDataB = static_cast<ShapeUserData2D*>(b2Shape_GetUserData(event.shapeIdB));

        BodyUserData2D* bodyDataA = static_cast<BodyUserData2D*>(b2Body_GetUserData(bodyIdA));
        BodyUserData2D* bodyDataB = static_cast<BodyUserData2D*>(b2Body_GetUserData(bodyIdB));

        if (bodyDataA->BodyEntity.HasComponent<Rigidbody2DComponent>()) {
            auto& rb = bodyDataA->BodyEntity.GetComponent<Rigidbody2DComponent>();

            Collision callbackA;
            callbackA.CollidedEntity = bodyDataB->BodyEntity;
            callbackA.CollidedEntityTransform = &bodyDataB->BodyEntity.GetComponent<TransformComponent>();
            callbackA.CollidedEntityRigidbody = &bodyDataB->BodyEntity.GetComponent<Rigidbody2DComponent>();
            callbackA.HostColliderData = *shapeDataA;

            if (rb.OnCollisionEnterCallback) {
                rb.OnCollisionEnterCallback(callbackA);
            }
        }

        if (bodyDataB->BodyEntity.HasComponent<Rigidbody2DComponent>()) {
            auto& rb = bodyDataB->BodyEntity.GetComponent<Rigidbody2DComponent>();

            Collision callbackB;
            callbackB.CollidedEntity = bodyDataA->BodyEntity;
            callbackB.CollidedEntityTransform = &bodyDataA->BodyEntity.GetComponent<TransformComponent>();
            callbackB.CollidedEntityRigidbody = &bodyDataA->BodyEntity.GetComponent<Rigidbody2DComponent>();
            callbackB.HostColliderData = *shapeDataB;

            if (rb.OnCollisionEnterCallback) {
                rb.OnCollisionEnterCallback(callbackB);
            }
        }
    }

    for (int i = 0; i < contactEvents.endCount; ++i) {
        b2ContactEndTouchEvent event = contactEvents.endEvents[i];

        // --- Process End Touch Event -----------------
        b2BodyId bodyIdA = b2Shape_GetBody(event.shapeIdA);
        b2BodyId bodyIdB = b2Shape_GetBody(event.shapeIdB);

        ShapeUserData2D* shapeDataA = static_cast<ShapeUserData2D*>(b2Shape_GetUserData(event.shapeIdA));
        ShapeUserData2D* shapeDataB = static_cast<ShapeUserData2D*>(b2Shape_GetUserData(event.shapeIdB));

        BodyUserData2D* bodyDataA = static_cast<BodyUserData2D*>(b2Body_GetUserData(bodyIdA));
        BodyUserData2D* bodyDataB = static_cast<BodyUserData2D*>(b2Body_GetUserData(bodyIdB));

        if (bodyDataA->BodyEntity.HasComponent<Rigidbody2DComponent>()) {
            auto& rb = bodyDataA->BodyEntity.GetComponent<Rigidbody2DComponent>();

            Collision callbackA;
            callbackA.CollidedEntity = bodyDataB->BodyEntity;
            callbackA.CollidedEntityTransform = &bodyDataB->BodyEntity.GetComponent<TransformComponent>();
            callbackA.CollidedEntityRigidbody = &bodyDataB->BodyEntity.GetComponent<Rigidbody2DComponent>();
            callbackA.HostColliderData = *shapeDataA;
            callbackA.CollidedEntitysColliderData = *shapeDataB;

            if (rb.OnCollisionExitCallback) {
                rb.OnCollisionExitCallback(callbackA);
            }
        }

        if (bodyDataB->BodyEntity.HasComponent<Rigidbody2DComponent>()) {
            auto& rb = bodyDataB->BodyEntity.GetComponent<Rigidbody2DComponent>();

            Collision callbackB;
            callbackB.CollidedEntity = bodyDataA->BodyEntity;
            callbackB.CollidedEntityTransform = &bodyDataA->BodyEntity.GetComponent<TransformComponent>();
            callbackB.CollidedEntityRigidbody = &bodyDataA->BodyEntity.GetComponent<Rigidbody2DComponent>();
            callbackB.HostColliderData = *shapeDataB;
            callbackB.CollidedEntitysColliderData = *shapeDataA;

            if (rb.OnCollisionExitCallback) {
                rb.OnCollisionExitCallback(callbackB);
            }
        }
        // ----------------------------------------------
    }
}
bool Physics2DPreSolve(b2ShapeId shapeIdA, b2ShapeId shapeIdB, b2Manifold* manifold, void* context)
{
    TB_PROFILE_SCOPE_NAME("Tabby::Physics2DPreSolve");

    TB_CORE_ASSERT_TAGGED(b2Shape_IsValid(shapeIdA), "Shape is null!");
    TB_CORE_ASSERT_TAGGED(b2Shape_IsValid(shapeIdB), "Shape is null!");

    b2BodyId bodyIdA = b2Shape_GetBody(shapeIdA);
    b2BodyId bodyIdB = b2Shape_GetBody(shapeIdB);

    ShapeUserData2D* shapeDataA = static_cast<ShapeUserData2D*>(b2Shape_GetUserData(shapeIdA));
    ShapeUserData2D* shapeDataB = static_cast<ShapeUserData2D*>(b2Shape_GetUserData(shapeIdB));

    BodyUserData2D* bodyDataA = static_cast<BodyUserData2D*>(b2Body_GetUserData(bodyIdA));
    BodyUserData2D* bodyDataB = static_cast<BodyUserData2D*>(b2Body_GetUserData(bodyIdB));

    if (shapeDataA->BodyEntity.HasComponent<Rigidbody2DComponent>()) {

        Collision callbackA;

        if (shapeDataA->ColliderType == ColliderType2D::Box) {
            callbackA.CollidedEntity = bodyDataB->BodyEntity;
            callbackA.CollidedEntityTransform = &bodyDataB->BodyEntity.GetComponent<TransformComponent>();
            callbackA.CollidedEntityRigidbody = &bodyDataB->BodyEntity.GetComponent<Rigidbody2DComponent>();
            callbackA.HostColliderData = *shapeDataA;
            callbackA.CollidedEntitysColliderData = *shapeDataB;

            auto& bc = shapeDataA->ShapeEntity.GetComponent<BoxCollider2DComponent>();

            if (bc.OnPreSolve) {
                return bc.OnPreSolve(callbackA);
            }
        } else if (shapeDataA->ColliderType == ColliderType2D::Circle) {
            callbackA.CollidedEntity = bodyDataB->BodyEntity;
            callbackA.CollidedEntityTransform = &bodyDataB->BodyEntity.GetComponent<TransformComponent>();
            callbackA.CollidedEntityRigidbody = &bodyDataB->BodyEntity.GetComponent<Rigidbody2DComponent>();
            callbackA.HostColliderData = *shapeDataA;
            callbackA.CollidedEntitysColliderData = *shapeDataB;

            auto& cc = shapeDataA->ShapeEntity.GetComponent<CircleCollider2DComponent>();

            if (cc.OnPreSolve) {
                return cc.OnPreSolve(callbackA);
            }
        } else if (shapeDataA->ColliderType == ColliderType2D::Capsule) {
            callbackA.CollidedEntity = bodyDataB->BodyEntity;
            callbackA.CollidedEntityTransform = &bodyDataB->BodyEntity.GetComponent<TransformComponent>();
            callbackA.CollidedEntityRigidbody = &bodyDataB->BodyEntity.GetComponent<Rigidbody2DComponent>();
            callbackA.HostColliderData = *shapeDataA;
            callbackA.CollidedEntitysColliderData = *shapeDataB;

            auto& cc = shapeDataA->ShapeEntity.GetComponent<CapsuleCollider2DComponent>();

            if (cc.OnPreSolve) {
                return cc.OnPreSolve(callbackA);
            }
        } else if (shapeDataA->ColliderType == ColliderType2D::Segment) {
            callbackA.CollidedEntity = bodyDataB->BodyEntity;
            callbackA.CollidedEntityTransform = &bodyDataB->BodyEntity.GetComponent<TransformComponent>();
            callbackA.CollidedEntityRigidbody = &bodyDataB->BodyEntity.GetComponent<Rigidbody2DComponent>();
            callbackA.HostColliderData = *shapeDataA;
            callbackA.CollidedEntitysColliderData = *shapeDataB;

            auto& sc = shapeDataA->ShapeEntity.GetComponent<SegmentCollider2DComponent>();

            if (sc.OnPreSolve) {
                return sc.OnPreSolve(callbackA);
            }
        }
    }

    if (bodyDataB->BodyEntity.HasComponent<Rigidbody2DComponent>()) {
        Collision callbackB;

        if (shapeDataA->ColliderType == ColliderType2D::Box) {
            callbackB.CollidedEntity = bodyDataA->BodyEntity;
            callbackB.CollidedEntityTransform = &bodyDataA->BodyEntity.GetComponent<TransformComponent>();
            callbackB.CollidedEntityRigidbody = &bodyDataA->BodyEntity.GetComponent<Rigidbody2DComponent>();
            callbackB.HostColliderData = *shapeDataB;
            callbackB.CollidedEntitysColliderData = *shapeDataA;

            auto& bc = shapeDataB->ShapeEntity.GetComponent<BoxCollider2DComponent>();

            if (bc.OnPreSolve) {
                return bc.OnPreSolve(callbackB);
            }
        } else if (shapeDataA->ColliderType == ColliderType2D::Circle) {
            callbackB.CollidedEntity = bodyDataA->BodyEntity;
            callbackB.CollidedEntityTransform = &bodyDataA->BodyEntity.GetComponent<TransformComponent>();
            callbackB.CollidedEntityRigidbody = &bodyDataA->BodyEntity.GetComponent<Rigidbody2DComponent>();
            callbackB.HostColliderData = *shapeDataB;
            callbackB.CollidedEntitysColliderData = *shapeDataA;

            auto& cc = shapeDataB->ShapeEntity.GetComponent<CircleCollider2DComponent>();

            if (cc.OnPreSolve) {
                return cc.OnPreSolve(callbackB);
            }
        } else if (shapeDataA->ColliderType == ColliderType2D::Capsule) {
            callbackB.CollidedEntity = bodyDataA->BodyEntity;
            callbackB.CollidedEntityTransform = &bodyDataA->BodyEntity.GetComponent<TransformComponent>();
            callbackB.CollidedEntityRigidbody = &bodyDataA->BodyEntity.GetComponent<Rigidbody2DComponent>();
            callbackB.HostColliderData = *shapeDataB;
            callbackB.CollidedEntitysColliderData = *shapeDataA;

            auto& cc = shapeDataB->ShapeEntity.GetComponent<CapsuleCollider2DComponent>();

            if (cc.OnPreSolve) {
                return cc.OnPreSolve(callbackB);
            }
        } else if (shapeDataA->ColliderType == ColliderType2D::Segment) {
            callbackB.CollidedEntity = bodyDataA->BodyEntity;
            callbackB.CollidedEntityTransform = &bodyDataA->BodyEntity.GetComponent<TransformComponent>();
            callbackB.CollidedEntityRigidbody = &bodyDataA->BodyEntity.GetComponent<Rigidbody2DComponent>();
            callbackB.HostColliderData = *shapeDataB;
            callbackB.CollidedEntitysColliderData = *shapeDataA;

            auto& sc = shapeDataB->ShapeEntity.GetComponent<SegmentCollider2DComponent>();

            if (sc.OnPreSolve) {
                return sc.OnPreSolve(callbackB);
            }
        }
    }

    return true;
}

void* Physics2D::Physics2DEnqueueTask(b2TaskCallback* task, int32_t itemCount, int32_t minRange, void* taskContext, void* userContext)
{
    TB_PROFILE_SCOPE_NAME("Tabby::Physics2D::Physics2DEnqueueTask");

    Physics2DTask* physiscTask = new Physics2DTask();
    physiscTask->m_SetSize = itemCount;
    physiscTask->m_MinRange = minRange;
    physiscTask->m_task = task;
    physiscTask->m_taskContext = taskContext;

    Physics2D::s_Instance->m_TaskScheduler.AddTaskSetToPipe(physiscTask);

    return physiscTask;
}

void Physics2D::Physics2DFinishTask(void* taskPtr, void* userContext)
{
    TB_PROFILE_SCOPE_NAME("Tabby::Physics2D::Physics2DFinishTask");
    if (taskPtr != nullptr) {
        Physics2DTask* physiscTask = static_cast<Physics2DTask*>(taskPtr);
        Physics2D::s_Instance->m_TaskScheduler.WaitforTask(physiscTask);
        delete physiscTask;
    }
}

}
