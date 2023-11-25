#include "Graphics/Graphics.h"
#include "raylib.h"
#include "raymath.h"
#include "rcamera.h"
#include "rlgl.h"
#include <Math/Math.h>
#include <Scene/Octree/Octree.h>
#include <Scene/Scene.h>

#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_world.h"
#include "box2d/box2d.h"
#include <Scene/Components.h>
#include <Scene/GameObject.h>
#include <Scene/ScriptableGameObject.h>
#include <cstddef>
#include <cwchar>

namespace Tabby {

GameObject Scene::CreateEntity(const std::string& name)
{
    return CreateEntityWithUUID(UUID(), name);
}

GameObject Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
{
    Tabby::GameObject entity = { m_Registry.create(), this };
    entity.AddComponent<TransformComponent>();
    entity.AddComponent<IDComponent>(uuid);
    auto& tag = entity.AddComponent<TagComponent>();
    tag.Name = name.empty() ? "Entity" : name;
    return entity;
}

void Scene::DestroyEntity(GameObject entity)
{
    m_Registry.destroy(entity);
}

void Scene::InitScene()
{

    // Init physics
    // FIXME: Physics of entities created in runtime will not be initialized and will crash the application.
    {
        physics->Init({ 0.0, -9.81 });

        auto view = m_Registry.view<RigidBodyComponent>();
        for (auto e : view) {
            GameObject gameObject = { e, this };
            auto& transform = gameObject.GetComponent<TransformComponent>();
            auto& rb = gameObject.GetComponent<RigidBodyComponent>();

            b2BodyDef bodyDef;
            // bodyDef.type = (b2BodyType)rb.Type;
            bodyDef.type = b2_staticBody;
            bodyDef.position.Set(transform.Position.x / physics->GetPhysicsWorldScale(), transform.Position.y / physics->GetPhysicsWorldScale());
            bodyDef.angle = transform.Rotation.z;

            b2Body* body = physics->GetPhysicsWorld().CreateBody(&bodyDef);
            body->SetFixedRotation(rb.FixedRotation);

            if (gameObject.HasComponent<BoxCollider2DComponent>()) {
                auto& bc2D = gameObject.GetComponent<BoxCollider2DComponent>();

                b2PolygonShape shape;

                // shape.SetAsBox(bc2D.Size.x * transform.Scale.x, bc2D.Size.y * transform.Scale.y);
                shape.SetAsBox(bc2D.Size.x * transform.Scale.x, bc2D.Size.y * transform.Scale.y, b2Vec2(bc2D.Offset.x, bc2D.Offset.y), 0.0f);

                b2FixtureDef fixtureDef;
                fixtureDef.shape = &shape;
                fixtureDef.density = bc2D.Density;
                fixtureDef.friction = bc2D.Friction;
                fixtureDef.restitution = bc2D.Bounce;
                fixtureDef.restitutionThreshold = bc2D.BounceThreshold;
                body->CreateFixture(&fixtureDef);
            }

            // if (gameObject.HasComponent<CapsuleCollider2DComponent>()) {
            //     auto& cc2D = gameObject.GetComponent<CapsuleCollider2DComponent>();
            //
            //     b2FixtureDef fixtureDef;
            //     fixtureDef.density = cc2D.Density;
            //     fixtureDef.friction = cc2D.Friction;
            //     fixtureDef.restitution = cc2D.Bounce;
            //     fixtureDef.restitutionThreshold = cc2D.BounceThreshold;
            //
            //     // Create a box shape for the main body of the capsule
            //     b2PolygonShape boxShape;
            //     boxShape.SetAsBox(cc2D.Size.x / 2.0f, (cc2D.Size.y - 2.0f * (cc2D.Size.x / 2)) / 2.0f, b2Vec2(0.0f, 0.0f), 0.0f);
            //     fixtureDef.shape = &boxShape;
            //     body->CreateFixture(&fixtureDef);
            //
            //     // Create a circle shape for the top circle of the capsule
            //     b2CircleShape topCircleShape;
            //     topCircleShape.m_radius = cc2D.Size.x / 2;
            //     fixtureDef.shape = &topCircleShape;
            //
            //     // Position the top circle shape at the top of the capsule
            //     b2Vec2 topPosition(0.0f, cc2D.Size.y / 2.0f - (cc2D.Size.x / 2));
            //     topCircleShape.m_p = topPosition;
            //     body->CreateFixture(&fixtureDef);
            //
            //     // Create a circle shape for the bottom circle of the capsule
            //     b2CircleShape bottomCircleShape;
            //     bottomCircleShape.m_radius = cc2D.Size.x / 2;
            //     fixtureDef.shape = &bottomCircleShape;
            //
            //     // Position the bottom circle shape at the bottom of the capsule
            //     b2Vec2 bottomPosition(0.0f, -cc2D.Size.y / 2.0f + (cc2D.Size.x / 2));
            //     bottomCircleShape.m_p = bottomPosition;
            //     body->CreateFixture(&fixtureDef);
            //
            //     // auto& cc2D = gameObject.GetComponent<CapsuleCollider2DComponent>();
            //     //
            //     // b2FixtureDef fixtureDef;
            //     // fixtureDef.density = cc2D.Density;
            //     // fixtureDef.friction = cc2D.Friction;
            //     // fixtureDef.restitution = cc2D.Bounce;
            //     // fixtureDef.restitutionThreshold = cc2D.BounceThreshold;
            //     //
            //     // // Create a box shape for the main body of the capsule
            //     // b2PolygonShape boxShape;
            //     //
            //     // // Adjust size based on transform scale
            //     // float scaleX = transform.Scale.x;
            //     // float scaleY = transform.Scale.y;
            //     // boxShape.SetAsBox((cc2D.Size.x / 2.0f) * scaleX, ((cc2D.Size.y - 2.0f * (cc2D.Size.x / 2)) / 2.0f) * scaleY, b2Vec2(0.0f, 0.0f), 0.0f);
            //     //
            //     // // Adjust position based on offset
            //     // b2Vec2 boxPosition(cc2D.Offset.x, cc2D.Offset.y);
            //     // boxPosition *= scaleY; // Consider the Y-axis scaling for the offset
            //     // boxShape.m_centroid += boxPosition;
            //     //
            //     // fixtureDef.shape = &boxShape;
            //     // body->CreateFixture(&fixtureDef);
            //     //
            //     // // Create a circle shape for the top circle of the capsule
            //     // b2CircleShape topCircleShape;
            //     // topCircleShape.m_radius = (cc2D.Size.x / 2) * scaleX;
            //     //
            //     // // Adjust position based on offset
            //     // b2Vec2 topPosition(cc2D.Offset.x, (cc2D.Size.y / 2.0f - (cc2D.Size.x / 2)) * scaleY);
            //     // topPosition *= scaleY;
            //     // topCircleShape.m_p = topPosition;
            //     // body->CreateFixture(&fixtureDef);
            //     //
            //     // // Create a circle shape for the bottom circle of the capsule
            //     // b2CircleShape bottomCircleShape;
            //     // bottomCircleShape.m_radius = (cc2D.Size.x / 2) * scaleX;
            //     //
            //     // // Adjust position based on offset
            //     // b2Vec2 bottomPosition(cc2D.Offset.x, -((cc2D.Size.y / 2.0f - (cc2D.Size.x / 2))) * scaleY);
            //     // bottomPosition *= scaleY;
            //     // bottomCircleShape.m_p = bottomPosition;
            //     // body->CreateFixture(&fixtureDef);
            // }

            // bodyDef.type = (b2BodyType)rb.Type;
            body->SetType((b2BodyType)rb.Type);
            rb.RuntimeBody = body;
        }
    }

    // Init Octree
    {
        m_Registry.view<OctreeComponent>().each([=](auto entity, auto& octree) {
            octree.Tree = new Octree(octree.Entities, octree.MinNodeSize, this);
        });
    }
}

void Scene::Update()
{
    {
        m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc) {
            // Scene On ScenePlay
            if (!nsc.Instance) {

                nsc.Instance = nsc.InstantiateScript();
                nsc.Instance->m_GameObject = { entity, this };
                nsc.Instance->OnCreate();
            }
            nsc.Instance->Update();
            nsc.Instance->LateUpdate();
        });
    }

    {
        for (auto entity : m_Registry.view<TransformComponent>()) {

            auto& transform = m_Registry.get<TransformComponent>(entity);

            if (transform.Rotation.x >= 360) {
                float rot = transform.Rotation.x - 360.0f;
                transform.Rotation.x = rot;
            }

            if (transform.Rotation.y >= 360) {
                float rot = transform.Rotation.y - 360.0f;
                transform.Rotation.y = rot;
            }

            if (transform.Rotation.z >= 360) {
                float rot = transform.Rotation.z - 360.0f;
                transform.Rotation.z = rot;
            }

            if (transform.Parent != entt::null) {

                GameObject parent = { transform.Parent, this };

                Vector3 position = {
                    parent.GetComponent<TransformComponent>().Position.x + transform.LocalPosition.x,
                    parent.GetComponent<TransformComponent>().Position.y + transform.LocalPosition.y,
                    parent.GetComponent<TransformComponent>().Position.z + transform.LocalPosition.z,
                };

                Vector3 rotation = {
                    parent.GetComponent<TransformComponent>().Rotation.x + transform.LocalRotation.x,
                    parent.GetComponent<TransformComponent>().Rotation.y + transform.LocalRotation.y,
                    parent.GetComponent<TransformComponent>().Rotation.z + transform.LocalRotation.z,
                };

                Vector3 scale = {
                    parent.GetComponent<TransformComponent>().Scale.x * transform.LocalScale.x,
                    parent.GetComponent<TransformComponent>().Scale.y * transform.LocalScale.y,
                    parent.GetComponent<TransformComponent>().Scale.z * transform.LocalScale.z,
                };

                transform.Position = position;
                transform.Rotation = rotation;
                transform.Scale = scale;
            }
        }
    }

    {
        auto group = m_Registry.group<TransformComponent>(entt::get<CameraComponent>);
        for (auto entity : group) {

            auto [transform, camera] = group.get<TransformComponent, CameraComponent>(entity);

            if (camera.IsMainCamera) {

                SetActiveCamera(entity);
            }

            if (camera.DebugCameraMovement) {
                UpdateCamera(&camera.Camera, camera.CameraMode);
            } else {
                camera.Camera.position = transform.Position;
            }
        }
    }

    {
        auto group = m_Registry.group<AnimationComponent>(entt::get<SpriteRendererComponent>);
        for (auto entity : group) {

            auto [animator, sprite] = group.get<AnimationComponent, SpriteRendererComponent>(entity);

            if (animator.CurrentAnimation.first != "null") {
                bool newFrame = animator.CurrentAnimation.second->UpdateFrame(GetFrameTime());

                if (newFrame) {
                    const FrameData* data = animator.CurrentAnimation.second->GetCurrentFrame();
                    /* sprite->Load(data->texture); */

                    sprite.srcRec = { (float)data->x, (float)data->y, (float)data->width, (float)data->height };
                }
            }

            sprite.Texture = animator.CurrentTexture;
        }
    }

    {
        const int32_t velocityIteration = 6;
        const int32_t positionIterations = 2;

        physics->Update(GetFrameTime());

        auto view = m_Registry.view<RigidBodyComponent>();
        for (auto e : view) {
            Tabby::GameObject gameObject = { e, this };
            auto& transform = gameObject.GetComponent<TransformComponent>();
            auto& rb = gameObject.GetComponent<RigidBodyComponent>();

            b2Body* body = (b2Body*)rb.RuntimeBody;

            const auto& position = body->GetPosition();
            transform.Position.x = position.x;
            transform.Position.y = position.y;
            transform.Rotation.z = rb.GetAngle();

            if (gameObject.HasComponent<BoxCollider2DComponent>()) {
                auto& bc2D = gameObject.GetComponent<BoxCollider2DComponent>();

                b2Fixture* fixture = body->GetFixtureList();
                if (fixture) {
                    fixture->SetDensity(bc2D.Density);
                    fixture->SetFriction(bc2D.Friction);
                    fixture->SetRestitution(bc2D.Bounce);
                    fixture->SetRestitutionThreshold(bc2D.BounceThreshold);
                    body->ResetMassData(); // Recalculate mass based on new density
                }
            }

            // if (gameObject.HasComponent<CapsuleCollider2DComponent>()) {
            //     auto& bc2D = gameObject.GetComponent<CapsuleCollider2DComponent>();
            //
            //     b2Fixture* fixture = body->GetFixtureList();
            //     if (fixture) {
            //         fixture->SetDensity(bc2D.Density);
            //         fixture->SetFriction(bc2D.Friction);
            //         fixture->SetRestitution(bc2D.Bounce);
            //         fixture->SetRestitutionThreshold(bc2D.BounceThreshold);
            //         body->ResetMassData(); // Recalculate mass based on new density
            //     }
            // }
        }
    }

    {
        BeginMode3D(m_Registry.get<CameraComponent>(m_ActiveCamera).Camera);
        rlEnableDepthTest();
        // Update Camera Frustum
        {
            auto view = m_Registry.view<CameraComponent>();
            for (auto& entity : view) {

                auto& camera = view.get<CameraComponent>(entity);

                if (camera.IsMainCamera) {
                    camera.Frustum = CameraTools::ExtractFrustum();
                }
            }
        }

        // Draw Octree
        {
            auto view = m_Registry.view<OctreeComponent>();
            for (auto& entity : view) {

                auto& octree = view.get<OctreeComponent>(entity);

                if (octree.DebugDraw) {
                    octree.Tree->Draw(octree.DrawColor);
                }
            }
        }

        physics->Draw();

        rlDisableBackfaceCulling();

        // #ifdef DEBUG
        //     DrawGrid(100, 1.0f);
        // #endif // DEBUG

        // Draw Sprite
        // NOTE: have sort entities to fix transparent part of sprite now showing whats behind
        {
            auto view = m_Registry.view<SpriteRendererComponent, TransformComponent>();

            // std::vector<entt::entity> sortedEntities;

            // for (entt::entity entity : view) {
            //     if (m_Registry.get<SpriteRendererComponent>(entity).Active == true)
            //         sortedEntities.push_back(entity);
            // }
            //
            // std::sort(sortedEntities.begin(), sortedEntities.end(), [this](const entt::entity& a, const entt::entity& b) {
            //     auto& transformA = m_Registry.get<TransformComponent>(a);
            //     auto& transformB = m_Registry.get<TransformComponent>(b);
            //     return transformA.Position.z < transformB.Position.z;
            // });

            for (const entt::entity entity : view) {
                auto transform = m_Registry.get<TransformComponent>(entity);
                auto sprite = m_Registry.get<SpriteRendererComponent>(entity);

                Graphics::DrawSprite(transform.GetTransform(), sprite.Texture, sprite.srcRec, transform.Position, transform.Rotation, { 0.0f, 0.0f }, { transform.Scale.x, transform.Scale.y }, sprite.Tint);
            }
        }

        EndMode3D();
    }
}

void Scene::OnDestroy()
{
    {
        m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc) {
            // Scene On ScenePlay
            if (nsc.Instance) {
                nsc.Instance->OnDestroy();
            }
        });
    }
}
}
