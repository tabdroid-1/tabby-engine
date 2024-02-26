#include "Physics2DContactListener.h"
#include "Physics2D.h"
#include <box2d/b2_contact.h>

#include "Tabby/Scene/ScriptableEntity.h"

namespace Tabby {

Physics2DContactListener::Physics2DContactListener(b2World* world)
{
    world->SetContactListener(this);
}
void Physics2DContactListener::BeginContact(b2Contact* contact)
{

    Entity a;
    b2BodyUserData abodyUserData = contact->GetFixtureA()->GetBody()->GetUserData();
    a = { static_cast<entt::entity>(abodyUserData.pointer) };

    Entity b;
    b2BodyUserData bbodyUserData = contact->GetFixtureB()->GetBody()->GetUserData();
    b = { static_cast<entt::entity>(bbodyUserData.pointer) };

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
    a = { static_cast<entt::entity>(bodyUserData.pointer) };

    Entity b;
    bodyUserData = contact->GetFixtureB()->GetBody()->GetUserData();
    b = { static_cast<entt::entity>(bodyUserData.pointer) };

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
    a = { static_cast<entt::entity>(bodyUserData.pointer) };

    Entity b;
    bodyUserData = contact->GetFixtureB()->GetBody()->GetUserData();
    b = { static_cast<entt::entity>(bodyUserData.pointer) };

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
    a = { static_cast<entt::entity>(bodyUserData.pointer) };

    Entity b;
    bodyUserData = contact->GetFixtureB()->GetBody()->GetUserData();
    b = { static_cast<entt::entity>(bodyUserData.pointer) };

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
