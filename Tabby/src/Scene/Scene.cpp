#include "Graphics/Graphics.h"
#include "raylib.h"
#include "raymath.h"
#include "rcamera.h"
#include "rlgl.h"
#include <Math/Math.h>
#include <Scene/Scene.h>

#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_world.h"
#include "box2d/box2d.h"
#include <Scene/Components.h>
#include <Scene/GameObject.h>
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
    tag.Tag = name.empty() ? "Entity" : name;
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
            bodyDef.position.Set(transform.position.x / physics->GetPhysicsWorldScale(), transform.position.y / physics->GetPhysicsWorldScale());
            bodyDef.angle = transform.rotation.z;

            b2Body* body = physics->GetPhysicsWorld().CreateBody(&bodyDef);
            body->SetFixedRotation(rb.FixedRotation);

            if (gameObject.HasComponent<BoxCollider2DComponent>()) {
                auto& bc2D = gameObject.GetComponent<BoxCollider2DComponent>();

                b2PolygonShape shape;

                shape.SetAsBox(bc2D.Size.x * transform.scale.x, bc2D.Size.y * transform.scale.y);

                b2FixtureDef fixtureDef;
                fixtureDef.shape = &shape;
                fixtureDef.density = bc2D.Density;
                fixtureDef.friction = bc2D.Friction;
                fixtureDef.restitution = bc2D.Bounce;
                fixtureDef.restitutionThreshold = bc2D.BounceThreshold;
                body->CreateFixture(&fixtureDef);
            }

            if (gameObject.HasComponent<CapsuleCollider2DComponent>()) {
                auto& cc2D = gameObject.GetComponent<CapsuleCollider2DComponent>();

                b2FixtureDef fixtureDef;
                fixtureDef.density = cc2D.Density;
                fixtureDef.friction = cc2D.Friction;
                fixtureDef.restitution = cc2D.Bounce;
                fixtureDef.restitutionThreshold = cc2D.BounceThreshold;

                // Create a box shape for the main body of the capsule
                b2PolygonShape boxShape;
                boxShape.SetAsBox(cc2D.Size.x / 2.0f, (cc2D.Size.y - 2.0f * (cc2D.Size.x / 2)) / 2.0f, b2Vec2(0.0f, 0.0f), 0.0f);
                fixtureDef.shape = &boxShape;
                body->CreateFixture(&fixtureDef);

                // Create a circle shape for the top circle of the capsule
                b2CircleShape topCircleShape;
                topCircleShape.m_radius = cc2D.Size.x / 2;
                fixtureDef.shape = &topCircleShape;

                // Position the top circle shape at the top of the capsule
                b2Vec2 topPosition(0.0f, cc2D.Size.y / 2.0f - (cc2D.Size.x / 2));
                topCircleShape.m_p = topPosition;
                body->CreateFixture(&fixtureDef);

                // Create a circle shape for the bottom circle of the capsule
                b2CircleShape bottomCircleShape;
                bottomCircleShape.m_radius = cc2D.Size.x / 2;
                fixtureDef.shape = &bottomCircleShape;

                // Position the bottom circle shape at the bottom of the capsule
                b2Vec2 bottomPosition(0.0f, -cc2D.Size.y / 2.0f + (cc2D.Size.x / 2));
                bottomCircleShape.m_p = bottomPosition;
                body->CreateFixture(&fixtureDef);
            }

            // bodyDef.type = (b2BodyType)rb.Type;
            body->SetType((b2BodyType)rb.Type);
            rb.RuntimeBody = body;
        }
    }

    // Octree
    {

        auto view = m_Registry.view<TransformComponent>();
        std::vector<entt::entity> gameObjects;

        for (entt::entity entity : view) {
            gameObjects.push_back(entity);
        }

        octree = new Octree(gameObjects, 0.25f, this);
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
        });
    }

    {
        for (auto entity : m_Registry.view<TransformComponent>()) {

            auto& transform = m_Registry.get<TransformComponent>(entity);

            if (transform.rotation.x >= 360) {
                float rot = transform.rotation.x - 360.0f;
                transform.rotation.x = rot;
            }

            if (transform.rotation.y >= 360) {
                float rot = transform.rotation.y - 360.0f;
                transform.rotation.y = rot;
            }

            if (transform.rotation.z >= 360) {
                float rot = transform.rotation.z - 360.0f;
                transform.rotation.z = rot;
            }

            if (transform.parent != entt::null) {

                GameObject parent = { transform.parent, this };

                Vector3 position = {
                    parent.GetComponent<TransformComponent>().position.x + transform.localPosition.x,
                    parent.GetComponent<TransformComponent>().position.y + transform.localPosition.y,
                    parent.GetComponent<TransformComponent>().position.z + transform.localPosition.z,
                };

                Vector3 rotation = {
                    parent.GetComponent<TransformComponent>().rotation.x + transform.localRotation.x,
                    parent.GetComponent<TransformComponent>().rotation.y + transform.localRotation.y,
                    parent.GetComponent<TransformComponent>().rotation.z + transform.localRotation.z,
                };

                Vector3 scale = {
                    parent.GetComponent<TransformComponent>().scale.x * transform.localScale.x,
                    parent.GetComponent<TransformComponent>().scale.y * transform.localScale.y,
                    parent.GetComponent<TransformComponent>().scale.z * transform.localScale.z,
                };

                transform.position = position;
                transform.rotation = rotation;
                transform.scale = scale;
            }
        }
    }

    {
        auto group = m_Registry.group<TransformComponent>(entt::get<CameraComponent>);
        for (auto entity : group) {

            auto [transform, camera] = group.get<TransformComponent, CameraComponent>(entity);

            if (camera.isMainCamera) {

                SetActiveCamera(entity);
            }

            if (camera.debugCameraMovement) {
                UpdateCamera(&camera.camera, camera.cameraMode);
            } else {
                camera.camera.position = transform.position;
            }
        }
    }

    {
        auto group = m_Registry.group<AnimationComponent>(entt::get<SpriteRendererComponent>);
        for (auto entity : group) {

            auto [animator, sprite] = group.get<AnimationComponent, SpriteRendererComponent>(entity);

            if (animator.currentAnimation.first != "null") {
                bool newFrame = animator.currentAnimation.second->UpdateFrame(GetFrameTime());

                if (newFrame) {
                    const FrameData* data = animator.currentAnimation.second->GetCurrentFrame();
                    /* sprite->Load(data->texture); */

                    sprite.srcRec = { (float)data->x, (float)data->y, (float)data->width, (float)data->height };
                }
            }

            sprite.Texture = animator.currentTexture;
        }
    }
}

void Scene::LateUpdate()
{

    {
        m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc) {
            // Scene On ScenePlay
            if (!nsc.Instance) {

                nsc.Instance = nsc.InstantiateScript();
                nsc.Instance->m_GameObject = { entity, this };
                nsc.Instance->OnCreate();
            }
            nsc.Instance->LateUpdate();
        });
    }

    {
        const int32_t velocityIteration = 6;
        const int32_t positionIterations = 2;

        // m_PhysiscWorld->Step(dt, velocityIteration, positionIterations);
        physics->Update(GetFrameTime());
        auto view = m_Registry.view<RigidBodyComponent>();
        for (auto e : view) {
            Tabby::GameObject gameObject = { e, this };
            auto& transform = gameObject.GetComponent<TransformComponent>();
            auto& rb = gameObject.GetComponent<RigidBodyComponent>();

            b2Body* body = (b2Body*)rb.RuntimeBody;

            const auto& position = body->GetPosition();
            transform.position.x = position.x;
            transform.position.y = position.y;
            transform.rotation.z = rb.GetAngle();

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

            if (gameObject.HasComponent<CapsuleCollider2DComponent>()) {
                auto& bc2D = gameObject.GetComponent<CapsuleCollider2DComponent>();

                b2Fixture* fixture = body->GetFixtureList();
                if (fixture) {
                    fixture->SetDensity(bc2D.Density);
                    fixture->SetFriction(bc2D.Friction);
                    fixture->SetRestitution(bc2D.Bounce);
                    fixture->SetRestitutionThreshold(bc2D.BounceThreshold);
                    body->ResetMassData(); // Recalculate mass based on new density
                }
            }
        }
    }
}

void Scene::Draw()
{

    bool isTrue = false;

    BeginMode3D(m_Registry.get<CameraComponent>(m_ActiveCamera).camera);

    {
        auto view = m_Registry.view<CameraComponent>();
        for (auto& entity : view) {

            auto& camera = view.get<CameraComponent>(entity);

            if (camera.isMainCamera) {
                camera.frustum = CameraTools::ExtractFrustum();
            }
        }
    }

    physics->Draw();

    rlDisableBackfaceCulling();

#ifdef DEBUG
    DrawGrid(100, 1.0f);
#endif // DEBUG

    // Draw Sprite
    {

        auto view = m_Registry.view<SpriteRendererComponent>();

        std::vector<entt::entity> sortedEntities;
        sortedEntities.reserve(view.size());

        for (entt::entity entity : view) {

            Tabby::GameObject gameObject = { entity, this };
            auto& transform = gameObject.GetComponent<TransformComponent>();

            if (Tabby::CameraTools::IsSphereInFrustum(m_Registry.get<CameraComponent>(m_ActiveCamera).frustum, transform.position, transform.scale.x > transform.scale.y ? transform.scale.x : transform.scale.y)) {
                sortedEntities.push_back(entity);
            }
        }

        std::sort(sortedEntities.begin(), sortedEntities.end(), [this](const entt::entity& a, const entt::entity& b) {
            auto& transformA = m_Registry.get<TransformComponent>(a);
            auto& transformB = m_Registry.get<TransformComponent>(b);
            return transformA.position.z < transformB.position.z;
        });

        for (const entt::entity& entity : sortedEntities) {
            Tabby::GameObject gameObject = { entity, this };
            auto& transform = gameObject.GetComponent<TransformComponent>();
            auto& sprite = gameObject.GetComponent<SpriteRendererComponent>();

            Graphics::DrawSprite(transform.GetTransform(), sprite.Texture, sprite.srcRec, transform.position, transform.rotation, { 0.0f, 0.0f }, { transform.scale.x, transform.scale.y }, sprite.Tint);
            if (Tabby::CameraTools::IsSphereInFrustum(m_Registry.get<CameraComponent>(m_ActiveCamera).frustum, transform.position, transform.scale.x > transform.scale.y ? transform.scale.x : transform.scale.y)) {
                isTrue = true;
            } else {
                isTrue = false;
            }
        }
    }

    // // Draw Octree bounds
    // {
    //     octree.Draw();
    // }

    EndMode3D();

    if (isTrue) {
        DrawText("Visible", 20, 40, 20, WHITE);
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
