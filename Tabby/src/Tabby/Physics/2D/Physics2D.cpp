#include "Physics2D.h"
#include "Physics2DContactListener.h"
#include "Physics2DRaycastCallback.h"
#include "Tabby/Scene/Entity.h"
#include "Tabby/Scene/Scene.h"
#include "box2d/b2_body.h"
#include "box2d/b2_world.h"
#include "entt/entt.hpp"
#include <Tabby/Core/Assert.h>
#include <utility>

namespace Tabby {

Physisc2D* Physisc2D::s_Instance = nullptr;

Physisc2D::Physisc2D()
    : m_PhysicsWorld(nullptr)
    , m_PhysicsContactListener(nullptr)
{
    if (!s_Instance)
        s_Instance = this;
}

Physisc2D::~Physisc2D()
{
    delete s_Instance->m_PhysicsWorld;
    delete s_Instance->m_PhysicsContactListener;
    delete s_Instance->m_PhysicsRaycastCallback;
    delete s_Instance;
}

void Physisc2D::InitWorld(glm::vec2& gravity)
{
    if (!s_Instance)
        s_Instance = new Physisc2D();

    // Check if the scene is different or if the world needs reinitialization
    if (!s_Instance->m_PhysicsWorld) {

        s_Instance->m_PhysicsWorld = new b2World({ gravity.x, gravity.y });

        s_Instance->m_PhysicsContactListener = new Physics2DContactListener(s_Instance->m_PhysicsWorld);
        s_Instance->m_PhysicsRaycastCallback = new Physics2DRaycastCallback();
    } else {
        s_Instance->m_PhysicsWorld->SetGravity({ gravity.x, gravity.y });
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

void Physisc2D::EnqueueBody(Entity entity, b2BodyDef* bodyDef)
{
    s_Instance->bodyQueue.push(std::make_pair(entity, bodyDef));
}

void Physisc2D::EnqueueFixture(Entity entity, b2FixtureDef* fixtureDef)
{
    s_Instance->fixtureQueue.push(std::make_pair(entity, fixtureDef));
}

void Physisc2D::ProcessBodyQueue()
{
    while (!s_Instance->bodyQueue.empty()) {

        s_Instance->queueEmpty = false;

        std::pair<Entity, b2BodyDef*> bodyDef = s_Instance->bodyQueue.front();
        auto& rb = bodyDef.first.GetComponent<Rigidbody2DComponent>();
        bodyDef.second->type = Utils::Rigidbody2DTypeToBox2DBody(rb.Type);

        rb.RuntimeBody = s_Instance->m_PhysicsWorld->CreateBody(bodyDef.second);
        static_cast<b2Body*>(rb.RuntimeBody)->SetFixedRotation(rb.FixedRotation);

        s_Instance->bodyQueue.pop();
        delete bodyDef.second;
    }

    if (s_Instance->bodyQueue.empty() && s_Instance->fixtureQueue.empty())
        s_Instance->queueEmpty = true;
}

void Physisc2D::ProcessFixtureQueue()
{
    while (!s_Instance->fixtureQueue.empty()) {
        s_Instance->queueEmpty = false;

        std::pair<Entity, b2FixtureDef*> fixtureDef = s_Instance->fixtureQueue.front();
        auto& rb = fixtureDef.first.GetComponent<Rigidbody2DComponent>();

        static_cast<b2Body*>(rb.RuntimeBody)->CreateFixture(fixtureDef.second);

        s_Instance->fixtureQueue.pop();

        delete fixtureDef.second->shape;
        delete fixtureDef.second;
    }

    if (s_Instance->bodyQueue.empty() && s_Instance->fixtureQueue.empty())
        s_Instance->queueEmpty = true;
}

}
