#include "Physics2D.h"
#include "Physics2DContactListener.h"
#include "Physics2DRaycastCallback.h"
#include "Tabby/Scene/Entity.h"
#include "Tabby/Scene/Scene.h"
#include "box2d/b2_body.h"
#include "box2d/b2_world.h"
#include "entt/entt.hpp"
#include <Tabby/Core/Assert.h>
#include <Tabby/Physics/2D/Physics2DTypes.h>
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
    delete m_PhysicsWorld;
    delete m_PhysicsContactListener;
    delete m_PhysicsRaycastCallback;
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
    ProcessBodyQueue();
    ProcessFixtureQueue();

    TB_CORE_ASSERT(s_Instance, "Physisc2D have to be initialized first!");
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

void Physisc2D::EnqueueBody(BodyInfo2D bodyInfo)
{
    s_Instance->bodyQueue.push(bodyInfo);
}

void Physisc2D::EnqueueFixture(FixtureInfo2D fixtureInfo)
{
    s_Instance->fixtureQueue.push(fixtureInfo);
}

void Physisc2D::ProcessBodyQueue()
{
    while (!s_Instance->bodyQueue.empty()) {

        s_Instance->queueEmpty = false;

        BodyInfo2D bodyInfo = s_Instance->bodyQueue.front();
        auto& transform = bodyInfo.entity.GetComponent<TransformComponent>();
        auto& rb2d = bodyInfo.entity.GetComponent<Rigidbody2DComponent>();

        // --------- Create Body def ---------
        b2BodyUserData bodyData;
        bodyData.pointer = static_cast<std::uintptr_t>(bodyInfo.entity.GetEntityHandle());

        b2BodyDef* bodyDef = new b2BodyDef;
        bodyDef->type = Utils::Rigidbody2DTypeToBox2DBody(rb2d.Type);
        bodyDef->position.Set(transform.Translation.x, transform.Translation.y);
        bodyDef->angle = transform.Rotation.z;
        bodyDef->userData = bodyData;

        // --------- Create Body ---------
        rb2d.RuntimeBody = s_Instance->m_PhysicsWorld->CreateBody(bodyDef);
        static_cast<b2Body*>(rb2d.RuntimeBody)->SetFixedRotation(rb2d.FixedRotation);

        s_Instance->bodyQueue.pop();
    }

    if (s_Instance->bodyQueue.empty() && s_Instance->fixtureQueue.empty())
        s_Instance->queueEmpty = true;
}

void Physisc2D::ProcessFixtureQueue()
{
    while (!s_Instance->fixtureQueue.empty()) {
        s_Instance->queueEmpty = false;

        FixtureInfo2D fixtureInfo = s_Instance->fixtureQueue.front();
        auto& transform = fixtureInfo.entity.GetComponent<TransformComponent>();

        // Check if entity it self has Rigidbody2DComponent. If there is, collider will be added to Rigidbody2DComponent of same entity.
        // If there is not, function will check its parent if it has Rigidbody2DComponent. And this will go on untill it finds an entity Rigidbody2DComponent;
        // This is is because an entity can have one component of same type. User can create More colliders by creating child entities.
        Rigidbody2DComponent* rb2d;
        if (fixtureInfo.entity.HasComponent<Rigidbody2DComponent>()) {
            rb2d = &fixtureInfo.entity.GetComponent<Rigidbody2DComponent>();
        } else {

            auto FindParentRigidbody = [](auto&& self, Rigidbody2DComponent* rb2d, entt::entity parentEntity) -> void {
                auto& hierarchyNode = Entity(parentEntity).GetComponent<HierarchyNodeComponent>();

                if (Entity(hierarchyNode.Parent.second).HasComponent<Rigidbody2DComponent>()) {
                    rb2d = &Entity(hierarchyNode.Parent.second).GetComponent<Rigidbody2DComponent>();
                } else {
                    self(self, rb2d, hierarchyNode.Parent.second);
                }
            };

            FindParentRigidbody(FindParentRigidbody, rb2d, fixtureInfo.entity);
        }

        if (!rb2d)
            continue;

        if (fixtureInfo.colliderType == ColliderType2D::Box) {
            auto& bc2d = fixtureInfo.entity.GetComponent<BoxCollider2DComponent>();

            // --------- Create box collider def ---------
            b2PolygonShape* boxShape = new b2PolygonShape;
            boxShape->SetAsBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y, b2Vec2(bc2d.Offset.x, bc2d.Offset.y), 0.0f);

            b2FixtureDef* fixtureDef = new b2FixtureDef;
            fixtureDef->shape = boxShape;
            fixtureDef->density = bc2d.Density;
            fixtureDef->friction = bc2d.Friction;
            fixtureDef->restitution = bc2d.Restitution;
            fixtureDef->restitutionThreshold = bc2d.RestitutionThreshold;

            // --------- Create box collider in body ---------
            bc2d.RuntimeFixture = static_cast<b2Body*>(rb2d->RuntimeBody)->CreateFixture(fixtureDef);
            bc2d.queuedForInitialization = false;

        } else if (fixtureInfo.colliderType == ColliderType2D::Circle) {
            auto& cc2d = fixtureInfo.entity.GetComponent<CircleCollider2DComponent>();

            // --------- Create circle collider def ---------
            b2CircleShape* circleShape = new b2CircleShape;
            circleShape->m_p.Set(cc2d.Offset.x, cc2d.Offset.y);
            circleShape->m_radius = transform.Scale.x * cc2d.Radius;

            b2FixtureDef* fixtureDef = new b2FixtureDef;
            fixtureDef->shape = circleShape;
            fixtureDef->density = cc2d.Density;
            fixtureDef->friction = cc2d.Friction;
            fixtureDef->restitution = cc2d.Restitution;
            fixtureDef->restitutionThreshold = cc2d.RestitutionThreshold;

            // --------- Create circle collider in body ---------
            cc2d.RuntimeFixture = static_cast<b2Body*>(rb2d->RuntimeBody)->CreateFixture(fixtureDef);
            cc2d.queuedForInitialization = false;
        }

        s_Instance->fixtureQueue.pop();
    }

    if (s_Instance->bodyQueue.empty() && s_Instance->fixtureQueue.empty())
        s_Instance->queueEmpty = true;
}

}
