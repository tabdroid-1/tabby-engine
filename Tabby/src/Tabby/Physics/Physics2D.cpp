#include "Physics2D.h"
#include "Tabby/Scene/Entity.h"
#include "Tabby/Scene/Scene.h"
#include "Tabby/Scene/ScriptableEntity.h"
#include "box2d/b2_world.h"
#include "entt/entt.hpp"
#include <Tabby/Core/Assert.h>

namespace Tabby {

/*
 *
 *
 * Physisc 2D
 *
 *
 */

Physisc2D* Physisc2D::s_Instance = nullptr;

Physisc2D::Physisc2D()
    : m_PhysicsWorld(nullptr)
    , m_PhysicsContactListener(nullptr)
    , m_Scene(nullptr)
{
    if (!s_Instance)
        s_Instance = this;
}

void Physisc2D::InitWorld(Scene* scene, glm::vec2& gravity)
{
    if (!s_Instance)
        s_Instance = new Physisc2D();

    // Check if the scene is different or if the world needs reinitialization
    if (s_Instance->m_Scene != scene || !s_Instance->m_PhysicsWorld) {
        delete s_Instance->m_Scene;
        delete s_Instance->m_PhysicsWorld;
        delete s_Instance->m_PhysicsContactListener;

        s_Instance->m_Scene = scene;
        s_Instance->m_PhysicsWorld = new b2World({ gravity.x, gravity.y });
        s_Instance->m_PhysicsContactListener = new Physics2DContactListener(s_Instance->m_Scene, s_Instance->m_PhysicsWorld);
        s_Instance->m_PhysicsRaycastCallback = new Physics2DRaycastCallback(s_Instance->m_Scene);
    }
}

void Physisc2D::UpdateWorld(float ts, int32_t velocityIterations, int32_t positionIterations)
{
    TB_CORE_ASSERT(!s_Instance, "Physisc2D have to be initialized first!");
    s_Instance->m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);
}

b2World* Physisc2D::GetPhysicsWorld()
{
    if (s_Instance && s_Instance->m_PhysicsWorld)
        return s_Instance->m_PhysicsWorld;
    else {
        // Handle the case when the world is not initialized
        TB_CORE_ERROR("Physics world is not initialized!");
        return nullptr;
    }
}

RaycastHit2D Physisc2D::Raycast(const glm::vec2& origin, const glm::vec2& direction, float distance, int minDepth, int maxDepth)
{
    RaycastHit2D tempRayCastHit;

    b2Vec2 box2DOrigin(origin.x, origin.y);
    b2Vec2 box2DDirection(direction.x, direction.y);

    s_Instance->m_PhysicsRaycastCallback->RefreshCallbackInfo(origin, &tempRayCastHit);
    s_Instance->m_PhysicsWorld->RayCast(s_Instance->m_PhysicsRaycastCallback, box2DOrigin, box2DOrigin + distance * box2DDirection);

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

/*
 *
 *
 * RayCast callback 2D
 *
 *
 */

Physics2DRaycastCallback::Physics2DRaycastCallback(Scene* scene)
    : m_Scene(scene)
{
}

float Physics2DRaycastCallback::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction)
{
    m_Raycasthit->point = { point.x, point.y };
    m_Raycasthit->normal = { normal.x, normal.y };
    m_Raycasthit->fraction = fraction;

    b2BodyUserData bodyUserData = fixture->GetBody()->GetUserData();

    Entity e;

    e = { static_cast<entt::entity>(bodyUserData.pointer), m_Scene };

    m_Raycasthit->entity = e;
    m_Raycasthit->transform = &e.GetComponent<TransformComponent>();
    m_Raycasthit->rigidbody = &e.GetComponent<Rigidbody2DComponent>();
    m_Raycasthit->distance = glm::distance(m_RayOrigin, { point.x, point.y });

    return fraction;
}

void Physics2DRaycastCallback::RefreshCallbackInfo(const glm::vec2& rayOrigin, RaycastHit2D* raycastHit)
{
    m_Raycasthit = raycastHit;
    m_RayOrigin = rayOrigin;
}

/*
 *
 *
 * Contact listener 2D
 *
 *
 */

Physics2DContactListener::Physics2DContactListener(Scene* scene, b2World* world)
    : m_Scene(scene)
{
    world->SetContactListener(this);
}
void Physics2DContactListener::BeginContact(b2Contact* contact)
{

    Entity a;
    b2BodyUserData abodyUserData = contact->GetFixtureA()->GetBody()->GetUserData();
    a = { static_cast<entt::entity>(abodyUserData.pointer), m_Scene };

    Entity b;
    b2BodyUserData bbodyUserData = contact->GetFixtureB()->GetBody()->GetUserData();
    b = { static_cast<entt::entity>(bbodyUserData.pointer), m_Scene };

    ContactCallback callbackA;
    callbackA.entity = b;
    callbackA.transform = &b.GetComponent<TransformComponent>();
    callbackA.rigidbody = &b.GetComponent<Rigidbody2DComponent>();
    ContactCallback callbackB;
    callbackB.entity = a;
    callbackB.transform = &a.GetComponent<TransformComponent>();
    callbackB.rigidbody = &a.GetComponent<Rigidbody2DComponent>();

    if (a.HasComponent<NativeScriptComponent>()) {
        auto& nsc = a.GetComponent<NativeScriptComponent>();

        if (!nsc.Instance) {
            nsc.Instance = nsc.InstantiateScript();
            nsc.Instance->m_Entity = a;
            nsc.Instance->OnCreate();
        }

        nsc.Instance->OnCollisionEnter(callbackA);
    }

    if (b.HasComponent<NativeScriptComponent>()) {
        auto& nsc = b.GetComponent<NativeScriptComponent>();

        if (!nsc.Instance) {
            nsc.Instance = nsc.InstantiateScript();
            nsc.Instance->m_Entity = b;
            nsc.Instance->OnCreate();
        }

        nsc.Instance->OnCollisionEnter(callbackB);
    }
}

void Physics2DContactListener::EndContact(b2Contact* contact)
{
    Entity a;
    b2BodyUserData bodyUserData = contact->GetFixtureA()->GetBody()->GetUserData();
    a = { static_cast<entt::entity>(bodyUserData.pointer), m_Scene };

    Entity b;
    bodyUserData = contact->GetFixtureB()->GetBody()->GetUserData();
    b = { static_cast<entt::entity>(bodyUserData.pointer), m_Scene };

    ContactCallback callbackA;
    callbackA.entity = b;
    callbackA.transform = &b.GetComponent<TransformComponent>();
    callbackA.rigidbody = &b.GetComponent<Rigidbody2DComponent>();
    ContactCallback callbackB;
    callbackB.entity = a;
    callbackB.transform = &a.GetComponent<TransformComponent>();
    callbackB.rigidbody = &a.GetComponent<Rigidbody2DComponent>();

    if (a.HasComponent<NativeScriptComponent>()) {
        auto& nsc = a.GetComponent<NativeScriptComponent>();

        if (!nsc.Instance) {
            nsc.Instance = nsc.InstantiateScript();
            nsc.Instance->m_Entity = a;
            nsc.Instance->OnCreate();
        }

        nsc.Instance->OnCollisionExit(callbackA);
    }

    if (b.HasComponent<NativeScriptComponent>()) {
        auto& nsc = b.GetComponent<NativeScriptComponent>();

        if (!nsc.Instance) {
            nsc.Instance = nsc.InstantiateScript();
            nsc.Instance->m_Entity = b;
            nsc.Instance->OnCreate();
        }

        nsc.Instance->OnCollisionExit(callbackB);
    }
}

void Physics2DContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
    Entity a;
    b2BodyUserData bodyUserData = contact->GetFixtureA()->GetBody()->GetUserData();
    a = { static_cast<entt::entity>(bodyUserData.pointer), m_Scene };

    Entity b;
    bodyUserData = contact->GetFixtureB()->GetBody()->GetUserData();
    b = { static_cast<entt::entity>(bodyUserData.pointer), m_Scene };

    ContactCallback callbackA;
    callbackA.entity = b;
    callbackA.transform = &b.GetComponent<TransformComponent>();
    callbackA.rigidbody = &b.GetComponent<Rigidbody2DComponent>();
    ContactCallback callbackB;
    callbackB.entity = a;
    callbackB.transform = &a.GetComponent<TransformComponent>();
    callbackB.rigidbody = &a.GetComponent<Rigidbody2DComponent>();

    if (a.HasComponent<NativeScriptComponent>()) {
        auto& nsc = a.GetComponent<NativeScriptComponent>();

        if (!nsc.Instance) {
            nsc.Instance = nsc.InstantiateScript();
            nsc.Instance->m_Entity = a;
            nsc.Instance->OnCreate();
        }

        nsc.Instance->OnPostSolve(callbackA);
    }

    if (b.HasComponent<NativeScriptComponent>()) {
        auto& nsc = b.GetComponent<NativeScriptComponent>();

        if (!nsc.Instance) {
            nsc.Instance = nsc.InstantiateScript();
            nsc.Instance->m_Entity = b;
            nsc.Instance->OnCreate();
        }

        nsc.Instance->OnPostSolve(callbackB);
    }
}

void Physics2DContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
    Entity a;
    b2BodyUserData bodyUserData = contact->GetFixtureA()->GetBody()->GetUserData();
    a = { static_cast<entt::entity>(bodyUserData.pointer), m_Scene };

    Entity b;
    bodyUserData = contact->GetFixtureB()->GetBody()->GetUserData();
    b = { static_cast<entt::entity>(bodyUserData.pointer), m_Scene };

    ContactCallback callbackA;
    callbackA.entity = b;
    callbackA.transform = &b.GetComponent<TransformComponent>();
    callbackA.rigidbody = &b.GetComponent<Rigidbody2DComponent>();
    ContactCallback callbackB;
    callbackB.entity = a;
    callbackB.transform = &a.GetComponent<TransformComponent>();
    callbackB.rigidbody = &a.GetComponent<Rigidbody2DComponent>();

    if (a.HasComponent<NativeScriptComponent>()) {
        auto& nsc = a.GetComponent<NativeScriptComponent>();

        if (!nsc.Instance) {
            nsc.Instance = nsc.InstantiateScript();
            nsc.Instance->m_Entity = a;
            nsc.Instance->OnCreate();
        }

        nsc.Instance->OnPreSolve(callbackA);
    }

    if (b.HasComponent<NativeScriptComponent>()) {
        auto& nsc = b.GetComponent<NativeScriptComponent>();

        if (!nsc.Instance) {
            nsc.Instance = nsc.InstantiateScript();
            nsc.Instance->m_Entity = b;
            nsc.Instance->OnCreate();
        }

        nsc.Instance->OnPreSolve(callbackB);
    }
}
}
