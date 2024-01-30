#include "Physics2D.h"
#include "Tabby/Scene/Entity.h"
#include "Tabby/Scene/Scene.h"
#include "Tabby/Scene/ScriptableEntity.h"
#include "box2d/b2_world.h"
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
    }
}

void Physisc2D::UpdateWorld(float ts, int32_t velocityIterations, int32_t positionIterations)
{
    TB_CORE_ASSERT(!s_Instance, "Physisc2D have to be initialized first!");
    s_Instance->m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);
}

b2World* Physisc2D::GetPhysicsWorld()
{
    // return s_Instance->m_PhysicsWorld;
    if (s_Instance && s_Instance->m_PhysicsWorld)
        return s_Instance->m_PhysicsWorld;
    else {
        // Handle the case when the world is not initialized
        TB_CORE_ERROR("Physics world is not initialized!");
        return nullptr;
    }
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
    b2BodyUserData bodyUserData = contact->GetFixtureA()->GetBody()->GetUserData();
    if (bodyUserData.pointer)
        a = { static_cast<entt::entity>(bodyUserData.pointer), m_Scene };

    Entity b;
    bodyUserData = contact->GetFixtureB()->GetBody()->GetUserData();
    if (bodyUserData.pointer)
        b = { static_cast<entt::entity>(bodyUserData.pointer), m_Scene };

    if (a.HasComponent<NativeScriptComponent>()) {
        auto& nsc = a.GetComponent<NativeScriptComponent>();

        if (!nsc.Instance) {
            nsc.Instance = nsc.InstantiateScript();
            nsc.Instance->m_Entity = a;
            nsc.Instance->OnCreate();
        }

        nsc.Instance->OnCollisionEnter();
    }

    if (b.HasComponent<NativeScriptComponent>()) {
        auto& nsc = b.GetComponent<NativeScriptComponent>();

        if (!nsc.Instance) {
            nsc.Instance = nsc.InstantiateScript();
            nsc.Instance->m_Entity = b;
            nsc.Instance->OnCreate();
        }

        nsc.Instance->OnCollisionEnter();
    }
}

void Physics2DContactListener::EndContact(b2Contact* contact)
{
    Entity a;
    b2BodyUserData bodyUserData = contact->GetFixtureA()->GetBody()->GetUserData();
    if (bodyUserData.pointer)
        a = { static_cast<entt::entity>(bodyUserData.pointer), m_Scene };

    Entity b;
    bodyUserData = contact->GetFixtureB()->GetBody()->GetUserData();
    if (bodyUserData.pointer)
        b = { static_cast<entt::entity>(bodyUserData.pointer), m_Scene };

    if (a.HasComponent<NativeScriptComponent>()) {
        auto& nsc = a.GetComponent<NativeScriptComponent>();

        if (!nsc.Instance) {
            nsc.Instance = nsc.InstantiateScript();
            nsc.Instance->m_Entity = a;
            nsc.Instance->OnCreate();
        }

        nsc.Instance->OnCollisionExit();
    }

    if (b.HasComponent<NativeScriptComponent>()) {
        auto& nsc = b.GetComponent<NativeScriptComponent>();

        if (!nsc.Instance) {
            nsc.Instance = nsc.InstantiateScript();
            nsc.Instance->m_Entity = b;
            nsc.Instance->OnCreate();
        }

        nsc.Instance->OnCollisionExit();
    }
}

void Physics2DContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
    Entity a;
    b2BodyUserData bodyUserData = contact->GetFixtureA()->GetBody()->GetUserData();
    if (bodyUserData.pointer)
        a = { static_cast<entt::entity>(bodyUserData.pointer), m_Scene };

    Entity b;
    bodyUserData = contact->GetFixtureB()->GetBody()->GetUserData();
    if (bodyUserData.pointer)
        b = { static_cast<entt::entity>(bodyUserData.pointer), m_Scene };

    if (a.HasComponent<NativeScriptComponent>()) {
        auto& nsc = a.GetComponent<NativeScriptComponent>();

        if (!nsc.Instance) {
            nsc.Instance = nsc.InstantiateScript();
            nsc.Instance->m_Entity = a;
            nsc.Instance->OnCreate();
        }

        nsc.Instance->OnPostSolve();
    }

    if (b.HasComponent<NativeScriptComponent>()) {
        auto& nsc = b.GetComponent<NativeScriptComponent>();

        if (!nsc.Instance) {
            nsc.Instance = nsc.InstantiateScript();
            nsc.Instance->m_Entity = b;
            nsc.Instance->OnCreate();
        }

        nsc.Instance->OnPostSolve();
    }
}

void Physics2DContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
    Entity a;
    b2BodyUserData bodyUserData = contact->GetFixtureA()->GetBody()->GetUserData();
    if (bodyUserData.pointer)
        a = { static_cast<entt::entity>(bodyUserData.pointer), m_Scene };

    Entity b;
    bodyUserData = contact->GetFixtureB()->GetBody()->GetUserData();
    if (bodyUserData.pointer)
        b = { static_cast<entt::entity>(bodyUserData.pointer), m_Scene };

    if (a.HasComponent<NativeScriptComponent>()) {
        auto& nsc = a.GetComponent<NativeScriptComponent>();

        if (!nsc.Instance) {
            nsc.Instance = nsc.InstantiateScript();
            nsc.Instance->m_Entity = a;
            nsc.Instance->OnCreate();
        }

        nsc.Instance->OnPreSolve();
    }

    if (b.HasComponent<NativeScriptComponent>()) {
        auto& nsc = b.GetComponent<NativeScriptComponent>();

        if (!nsc.Instance) {
            nsc.Instance = nsc.InstantiateScript();
            nsc.Instance->m_Entity = b;
            nsc.Instance->OnCreate();
        }

        nsc.Instance->OnPreSolve();
    }
}
}
