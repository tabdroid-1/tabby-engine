#include "Tabby/Physics/PhysicsWorld.h"
#include "Tabby/Scene/Entity.h"
#include "Tabby/Scene/Scene.h"
#include "Tabby/Scene/ScriptableEntity.h"
#include <Tabby/Core/Log.h>
#include <Tabby/Scene/Components.h>

#include "box2d/b2_body.h"
#include "box2d/b2_settings.h"

namespace Tabby {

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

PhysicsWorldHandle2D::PhysicsWorldHandle2D(Scene* scene, const glm::vec2& gravity)
    : m_Scene(scene)
{
    m_PhysicsWorld = new b2World({ gravity.x, gravity.y });
    m_PhysicsContactListener = new Physics2DContactListener(m_Scene, m_PhysicsWorld);
}

}
