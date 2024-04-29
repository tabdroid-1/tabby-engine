#include "Physics2D.h"
#include "box2d/box2d.h"
#include "entt/entt.hpp"
#include <Tabby/Physics/2D/Physics2DTypes.h>
#include <Tabby/Scene/ScriptableEntity.h>

namespace Tabby {

Physisc2D* Physisc2D::s_Instance = nullptr;

Physisc2D::Physisc2D()
{
    if (!s_Instance)
        s_Instance = this;
}

Physisc2D::~Physisc2D()
{
    b2DestroyWorld(m_PhysicsWorld);
    m_PhysicsWorld = b2_nullWorldId;
}

void Physisc2D::InitWorld(glm::vec2& gravity)
{
    if (!s_Instance)
        s_Instance = new Physisc2D();

    // Check if the scene is different or if the world needs reinitialization
    if (B2_IS_NON_NULL(s_Instance->m_PhysicsWorld)) {

        b2WorldDef worldDef = b2DefaultWorldDef();
        worldDef.gravity = { gravity.x, gravity.y };
        s_Instance->m_PhysicsWorld = b2CreateWorld(&worldDef);
    } else {
        b2World_SetGravity(s_Instance->m_PhysicsWorld, { gravity.x, gravity.y });
    }
}

void Physisc2D::UpdateWorld(float ts, int32_t subStepCount)
{
    ProcessBodyInitQueue();
    ProcessShapeInitQueue();
    ProcessShapeUpdateQueue();

    TB_CORE_ASSERT(s_Instance, "Physisc2D have to be initialized first!");
    b2World_Step(s_Instance->m_PhysicsWorld, ts, 4);

    ProcessEvents();
}

b2WorldId Physisc2D::GetPhysicsWorld()
{
    if (s_Instance && B2_IS_NON_NULL(s_Instance->m_PhysicsWorld))
        return s_Instance->m_PhysicsWorld;
    else {
        TB_CORE_ERROR("Physics world is not initialized!");
        return b2_nullWorldId;
    }
}

RaycastHit2D Physisc2D::Raycast(const glm::vec2& origin, const glm::vec2& direction, float distance, int minDepth, int maxDepth)
{
    RaycastHit2D tempRayCastHit;
    tempRayCastHit.origin = origin;

    // b2CastResultFcn* fcns[] = { physics2DRaycastCallback, RayCastClosestCallback, RayCastMultipleCallback,
    //     RayCastSortedCallback };
    // b2CastResultFcn* modeFcn = fcns[m_mode];
    b2CastResultFcn* fcn = Physics2DRaycastCallback;
    b2Vec2 box2DOrigin = { origin.x, origin.y };
    b2Vec2 box2DDirection = { direction.x, direction.y };
    b2Vec2 box2DDestination = { direction.x * distance, direction.y * distance };
    // s_Instance->m_PhysicsRaycastCallback->RefreshCallbackInfo(origin, &tempRayCastHit);
    // s_Instance->m_PhysicsWorld->RayCast(s_Instance->m_PhysicsRaycastCallback, box2DOrigin, box2DOrigin + distance * box2DDirection);
    // b2World_RayCast(b2WorldId worldId, b2Vec2 origin, b2Vec2 translation, b2QueryFilter filter, b2CastResultFcn *fcn, void *context)
    b2World_RayCast(s_Instance->m_PhysicsWorld, box2DOrigin, box2DDestination, b2DefaultQueryFilter(), fcn, &tempRayCastHit);

    return tempRayCastHit;
}

RaycastHit2D Physisc2D::Raycast(const glm::vec2& origin, const glm::vec2& direction, float distance)
{
    return Raycast(origin, direction, distance, 0, std::numeric_limits<int>::max());
}

RaycastHit2D Physisc2D::Raycast(const glm::vec2& origin, const glm::vec2& direction)
{
    return Raycast(origin, direction, std::numeric_limits<float>::infinity(), 0, std::numeric_limits<int>::max());
}

void Physisc2D::EnqueueBodyInit(BodyInfo2D bodyInfo)
{
    s_Instance->bodyInitQueue.push(bodyInfo);
}

void Physisc2D::EnqueueShapeInit(ShapeInfo2D shapeInfo)
{
    s_Instance->shapeInitQueue.push(shapeInfo);
}

void Physisc2D::EnqueueShapeUpdate(ShapeInfo2D shapeInfo)
{
    s_Instance->shapeUpdateQueue.push(shapeInfo);
}

void Physisc2D::ProcessBodyInitQueue()
{
    while (!s_Instance->bodyInitQueue.empty()) {

        s_Instance->queueEmpty = false;

        BodyInfo2D bodyInfo = s_Instance->bodyInitQueue.front();
        auto& transform = bodyInfo.entity.GetComponent<TransformComponent>();
        auto& rb2d = bodyInfo.entity.GetComponent<Rigidbody2DComponent>();

        // --------- Create Body def ---------
        BodyUserData2D* bodyUserData = new BodyUserData2D { bodyInfo.entity };

        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = Utils::Rigidbody2DTypeToBox2DBody(rb2d.Type);
        bodyDef.position = { transform.Translation.x, transform.Translation.y };
        bodyDef.angle = transform.Rotation.z;
        bodyDef.userData = static_cast<void*>(bodyUserData);

        // --------- Create Body ---------
        rb2d.RuntimeBodyId = b2CreateBody(s_Instance->m_PhysicsWorld, &bodyDef);
        b2Body_SetFixedRotation(rb2d.RuntimeBodyId, rb2d.FixedRotation);

        s_Instance->bodyInitQueue.pop();
    }

    if (s_Instance->bodyInitQueue.empty() && s_Instance->shapeInitQueue.empty() && s_Instance->shapeUpdateQueue.empty())
        s_Instance->queueEmpty = true;
}

void Physisc2D::ProcessShapeInitQueue()
{
    while (!s_Instance->shapeInitQueue.empty()) {
        s_Instance->queueEmpty = false;

        ShapeInfo2D shapeInfo = s_Instance->shapeInitQueue.front();
        auto& transform = shapeInfo.entity.GetComponent<TransformComponent>();

        // Check if entity it self has Rigidbody2DComponent. If there is, collider will be added to Rigidbody2DComponent of same entity.
        // If there is not, function will check its parent if it has Rigidbody2DComponent. And this will go on untill it finds an entity Rigidbody2DComponent;
        // This is is because an entity can have one component of same type. User can create More colliders by creating child entities.
        entt::entity rb2dEntity { entt::null };
        if (shapeInfo.entity.HasComponent<Rigidbody2DComponent>()) {
            rb2dEntity = shapeInfo.entity;
        } else {

            auto FindParentRigidbody = [](auto&& self, entt::entity parentEntity) -> entt::entity {
                auto& hierarchyNode = Entity(parentEntity).GetComponent<HierarchyNodeComponent>();

                if (Entity(hierarchyNode.Parent.second).HasComponent<Rigidbody2DComponent>()) {
                    return hierarchyNode.Parent.second;
                } else {
                    if (hierarchyNode.Parent.first.Valid())
                        return self(self, hierarchyNode.Parent.second);
                }
            };

            rb2dEntity = FindParentRigidbody(FindParentRigidbody, shapeInfo.entity);
        }

        if (rb2dEntity == entt::null)
            s_Instance->shapeInitQueue.pop();

        if (shapeInfo.colliderType == ColliderType2D::Box) {
            auto& bc2d = shapeInfo.entity.GetComponent<BoxCollider2DComponent>();
            ShapeUserData2D* userData = new ShapeUserData2D { shapeInfo.entity, rb2dEntity };

            // --------- Create box collider def ---------
            b2Polygon box = b2MakeOffsetBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y, { transform.LocalTranslation.x + bc2d.Offset.x, transform.LocalTranslation.y + bc2d.Offset.y }, bc2d.angle);

            b2ShapeDef shapeDef = b2DefaultShapeDef();
            shapeDef.density = bc2d.Density;
            shapeDef.friction = bc2d.Friction;
            shapeDef.restitution = bc2d.Restitution;
            shapeDef.isSensor = bc2d.isSensor;
            shapeDef.enableSensorEvents = bc2d.enableSensorEvents;
            shapeDef.enableContactEvents = bc2d.enableContactEvents;
            shapeDef.enablePreSolveEvents = bc2d.enablePreSolveEvents;
            shapeDef.userData = static_cast<void*>(userData);

            // --------- Create box collider in body ---------
            bc2d.RuntimeShapeId = b2CreatePolygonShape(Entity(rb2dEntity).GetComponent<Rigidbody2DComponent>().RuntimeBodyId, &shapeDef, &box);
            bc2d.queuedForInitialization = false;

        } else if (shapeInfo.colliderType == ColliderType2D::Circle) {
            auto& cc2d = shapeInfo.entity.GetComponent<CircleCollider2DComponent>();
            ShapeUserData2D* userData = new ShapeUserData2D { shapeInfo.entity, rb2dEntity };

            // --------- Create circle collider def ---------
            b2Circle circleShape = { { transform.LocalTranslation.x + cc2d.Offset.x, transform.LocalTranslation.y + cc2d.Offset.y }, cc2d.Radius };

            b2ShapeDef shapeDef = b2DefaultShapeDef();
            shapeDef.density = cc2d.Density;
            shapeDef.friction = cc2d.Friction;
            shapeDef.restitution = cc2d.Restitution;
            shapeDef.isSensor = cc2d.isSensor;
            shapeDef.enableSensorEvents = cc2d.enableSensorEvents;
            shapeDef.enableContactEvents = cc2d.enableContactEvents;
            shapeDef.enablePreSolveEvents = cc2d.enablePreSolveEvents;
            shapeDef.userData = static_cast<void*>(userData);

            // --------- Create circle collider in body ---------
            cc2d.RuntimeShapeId = b2CreateCircleShape(Entity(rb2dEntity).GetComponent<Rigidbody2DComponent>().RuntimeBodyId, &shapeDef, &circleShape);
            cc2d.queuedForInitialization = false;
        } else if (shapeInfo.colliderType == ColliderType2D::Capsule) {
            auto& cc2d = shapeInfo.entity.GetComponent<CapsuleCollider2DComponent>();
            ShapeUserData2D* userData = new ShapeUserData2D { shapeInfo.entity, rb2dEntity };

            // --------- Create circle collider def ---------

            b2Vec2 center1 = { cc2d.center1.x + transform.LocalTranslation.x, cc2d.center1.y + transform.LocalTranslation.y };
            b2Vec2 center2 = { cc2d.center2.x + transform.LocalTranslation.x, cc2d.center2.y + transform.LocalTranslation.y };
            b2Capsule capsuleShape = { center1, center2, cc2d.Radius };

            b2ShapeDef shapeDef = b2DefaultShapeDef();
            shapeDef.density = cc2d.Density;
            shapeDef.friction = cc2d.Friction;
            shapeDef.restitution = cc2d.Restitution;
            shapeDef.isSensor = cc2d.isSensor;
            shapeDef.enableSensorEvents = cc2d.enableSensorEvents;
            shapeDef.enableContactEvents = cc2d.enableContactEvents;
            shapeDef.enablePreSolveEvents = cc2d.enablePreSolveEvents;
            shapeDef.userData = static_cast<void*>(userData);

            // --------- Create circle collider in body ---------
            cc2d.RuntimeShapeId = b2CreateCapsuleShape(Entity(rb2dEntity).GetComponent<Rigidbody2DComponent>().RuntimeBodyId, &shapeDef, &capsuleShape);
            cc2d.queuedForInitialization = false;
        }

        s_Instance->shapeInitQueue.pop();
    }

    if (s_Instance->bodyInitQueue.empty() && s_Instance->shapeInitQueue.empty() && s_Instance->shapeUpdateQueue.empty())
        s_Instance->queueEmpty = true;
}

void Physisc2D::ProcessShapeUpdateQueue()
{
    while (!s_Instance->shapeUpdateQueue.empty()) {
        s_Instance->queueEmpty = false;

        ShapeInfo2D shapeInfo = s_Instance->shapeUpdateQueue.front();
        auto& transform = shapeInfo.entity.GetComponent<TransformComponent>();

        if (shapeInfo.colliderType == ColliderType2D::Box) {
            auto& bc2d = shapeInfo.entity.GetComponent<BoxCollider2DComponent>();

            // --------- Create box collider def ---------
            b2Polygon box = b2MakeOffsetBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y, { transform.LocalTranslation.x + bc2d.Offset.x, transform.LocalTranslation.y + bc2d.Offset.y }, bc2d.angle);

            b2Shape_SetPolygon(bc2d.RuntimeShapeId, &box);
            bc2d.queuedForInitialization = false;

        } else if (shapeInfo.colliderType == ColliderType2D::Circle) {
            auto& cc2d = shapeInfo.entity.GetComponent<CircleCollider2DComponent>();
            b2Circle circleShape = { { transform.LocalTranslation.x + cc2d.Offset.x, transform.LocalTranslation.y + cc2d.Offset.y }, cc2d.Radius };

            b2Shape_SetCircle(cc2d.RuntimeShapeId, &circleShape);
            cc2d.queuedForInitialization = false;

        } else if (shapeInfo.colliderType == ColliderType2D::Capsule) {
            auto& cc2d = shapeInfo.entity.GetComponent<CapsuleCollider2DComponent>();

            b2Vec2 center1 = { cc2d.center1.x + transform.LocalTranslation.x, cc2d.center1.y + transform.LocalTranslation.y };
            b2Vec2 center2 = { cc2d.center2.x + transform.LocalTranslation.x, cc2d.center2.y + transform.LocalTranslation.y };
            b2Capsule capsuleShape = { center1, center2, cc2d.Radius };

            // --------- Create circle collider in body ---------
            b2Shape_SetCapsule(cc2d.RuntimeShapeId, &capsuleShape);
            cc2d.queuedForInitialization = false;
        }

        s_Instance->shapeUpdateQueue.pop();
    }

    if (s_Instance->bodyInitQueue.empty() && s_Instance->shapeInitQueue.empty() && s_Instance->shapeUpdateQueue.empty())
        s_Instance->queueEmpty = true;
}

float Physisc2D::Physics2DRaycastCallback(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void* context)
{

    static_cast<RaycastHit2D*>(context)->point = { point.x, point.y };
    static_cast<RaycastHit2D*>(context)->normal = { normal.x, normal.y };
    static_cast<RaycastHit2D*>(context)->fraction = fraction;

    ShapeUserData2D* userData = static_cast<ShapeUserData2D*>(b2Shape_GetUserData(shapeId));

    static_cast<RaycastHit2D*>(context)->entity = userData->bodyEntity;
    static_cast<RaycastHit2D*>(context)->transform = &userData->bodyEntity.GetComponent<TransformComponent>();
    static_cast<RaycastHit2D*>(context)->rigidbody = &userData->bodyEntity.GetComponent<Rigidbody2DComponent>();
    static_cast<RaycastHit2D*>(context)->distance = glm::distance(static_cast<RaycastHit2D*>(context)->origin, { point.x, point.y });

    return fraction;
}

void Physisc2D::ProcessEvents()
{
    b2ContactEvents contactEvents = b2World_GetContactEvents(s_Instance->m_PhysicsWorld);
    for (int i = 0; i < contactEvents.beginCount; ++i) {
        b2ContactBeginTouchEvent event = contactEvents.beginEvents[i];

        // --- Process Begin Touch Event -----------------
        b2BodyId bodyIdA = b2Shape_GetBody(event.shapeIdA);
        b2BodyId bodyIdB = b2Shape_GetBody(event.shapeIdB);

        BodyUserData2D* userDataA = static_cast<BodyUserData2D*>(b2Body_GetUserData(bodyIdA));
        BodyUserData2D* userDataB = static_cast<BodyUserData2D*>(b2Body_GetUserData(bodyIdB));

        if (userDataA->bodyEntity.HasComponent<NativeScriptComponent>()) {
            auto& nsc = userDataA->bodyEntity.GetComponent<NativeScriptComponent>();

            // If entity has NativeScriptComponent it will call its OnCollisionEnter
            ContactCallback callbackA;
            callbackA.entity = userDataB->bodyEntity;
            callbackA.transform = &userDataB->bodyEntity.GetComponent<TransformComponent>();
            callbackA.rigidbody = &userDataB->bodyEntity.GetComponent<Rigidbody2DComponent>();

            if (!nsc.Instance) {
                nsc.Instance = nsc.InstantiateScript();
                nsc.Instance->m_Entity = userDataA->bodyEntity;
                nsc.Instance->OnCreate();
            }
            nsc.Instance->OnCollisionEnter(callbackA);
        }

        if (userDataB->bodyEntity.HasComponent<NativeScriptComponent>()) {
            auto& nsc = userDataB->bodyEntity.GetComponent<NativeScriptComponent>();

            // If entity has NativeScriptComponent it will call its OnCollisionEnter
            ContactCallback callbackB;
            callbackB.entity = userDataA->bodyEntity;
            callbackB.transform = &userDataA->bodyEntity.GetComponent<TransformComponent>();
            callbackB.rigidbody = &userDataA->bodyEntity.GetComponent<Rigidbody2DComponent>();

            if (!nsc.Instance) {
                nsc.Instance = nsc.InstantiateScript();
                nsc.Instance->m_Entity = userDataB->bodyEntity;
                nsc.Instance->OnCreate();
            }
            nsc.Instance->OnCollisionEnter(callbackB);
        }

        // ----------------------------------------------
    }

    for (int i = 0; i < contactEvents.endCount; ++i) {
        b2ContactEndTouchEvent event = contactEvents.endEvents[i];

        // --- Process End Touch Event -----------------
        b2BodyId bodyIdA = b2Shape_GetBody(event.shapeIdA);
        b2BodyId bodyIdB = b2Shape_GetBody(event.shapeIdB);

        BodyUserData2D* userDataA = static_cast<BodyUserData2D*>(b2Body_GetUserData(bodyIdA));
        BodyUserData2D* userDataB = static_cast<BodyUserData2D*>(b2Body_GetUserData(bodyIdB));

        if (userDataA->bodyEntity.HasComponent<NativeScriptComponent>()) {
            auto& nsc = userDataA->bodyEntity.GetComponent<NativeScriptComponent>();

            // If entity has NativeScriptComponent it will call its OnCollisionExit
            ContactCallback callbackA;
            callbackA.entity = userDataB->bodyEntity;
            callbackA.transform = &userDataB->bodyEntity.GetComponent<TransformComponent>();
            callbackA.rigidbody = &userDataB->bodyEntity.GetComponent<Rigidbody2DComponent>();

            if (!nsc.Instance) {
                nsc.Instance = nsc.InstantiateScript();
                nsc.Instance->m_Entity = userDataA->bodyEntity;
                nsc.Instance->OnCreate();
            }
            nsc.Instance->OnCollisionExit(callbackA);
        }

        if (userDataB->bodyEntity.HasComponent<NativeScriptComponent>()) {
            auto& nsc = userDataB->bodyEntity.GetComponent<NativeScriptComponent>();

            // If entity has NativeScriptComponent it will call its OnCollisionExit
            ContactCallback callbackB;
            callbackB.entity = userDataA->bodyEntity;
            callbackB.transform = &userDataA->bodyEntity.GetComponent<TransformComponent>();
            callbackB.rigidbody = &userDataA->bodyEntity.GetComponent<Rigidbody2DComponent>();

            if (!nsc.Instance) {
                nsc.Instance = nsc.InstantiateScript();
                nsc.Instance->m_Entity = userDataB->bodyEntity;
                nsc.Instance->OnCreate();
            }
            nsc.Instance->OnCollisionExit(callbackB);
        }

        // ----------------------------------------------
    }
}

}
