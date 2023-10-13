#include "Graphics/Graphics.h"
#include "raylib.h"
#include "raymath.h"
#include "rcamera.h"
#include "rlgl.h"
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

void Scene::InitScene()
{
    rlDisableBackfaceCulling();
}

void Scene::InitPhysics()
{
    physics.Init({ 0.0, 0.0 });

    auto view = m_Registry.view<RigidBodyComponent>();
    for (auto e : view) {
        GameObject gameObject = { e, this };
        auto& transform = gameObject.GetComponent<TransformComponent>();
        auto& rb = gameObject.GetComponent<RigidBodyComponent>();

        b2BodyDef bodyDef;
        // bodyDef.type = (b2BodyType)rb.Type;
        bodyDef.type = b2_staticBody;
        bodyDef.position.Set(transform.Position.x / physics.GetPhysicsWorldScale(), transform.Position.y / physics.GetPhysicsWorldScale());
        bodyDef.angle = transform.Rotation.z;

        b2Body* body = physics.GetPhysicsWorld().CreateBody(&bodyDef);
        body->SetFixedRotation(rb.FixedRotation);

        if (gameObject.HasComponent<BoxCollider2DComponent>()) {
            auto& bc2D = gameObject.GetComponent<BoxCollider2DComponent>();

            b2PolygonShape shape;

            shape.SetAsBox(bc2D.Size.x * transform.Scale.x, bc2D.Size.y * transform.Scale.y);

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

void Scene::Update(float dt)
{
    {
        m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc) {
            // Scene On ScenePlay
            if (!nsc.Instance) {

                nsc.Instance = nsc.InstantiateScript();
                nsc.Instance->m_GameObject = { entity, this };
                nsc.Instance->OnCreate();
            }
            nsc.Instance->Update(dt);
        });
    }

    {
        auto group = m_Registry.group<TransformComponent>(entt::get<CameraComponent>);
        for (auto entity : group) {

            auto [transform, camera] = group.get<TransformComponent, CameraComponent>(entity);

            // camera.camera.target = { transform.Position.x, transform.Position.y, transform.Position.z + 1 };
            camera.camera.position = transform.Position;
            // // Create a rotation matrix (e.g., a 45-degree rotation around the Y-axis)
            // Matrix rotationMatrix = MatrixRotateX(transform.Rotation.x * DEG2RAD);
            // rotationMatrix = MatrixRotateY(transform.Rotation.y * DEG2RAD);
            // rotationMatrix = MatrixRotateZ(transform.Rotation.z * DEG2RAD);
            //
            // // Apply the rotation matrix to the camera's target vector
            // camera.camera.target = Vector3Transform(camera.camera.target, rotationMatrix);
            //
            // // Optionally, apply the same rotation to the camera's up vector to maintain its orientation
            // camera.camera.up = Vector3Transform(camera.camera.up, rotationMatrix);

            if (camera.isMainCamera) {

                SetActiveCamera(camera.camera);
            }

            if (camera.debugCameraMovement) {

                UpdateCamera(&camera.camera, camera.cameraMode);
            }
        }
    }

    {

        auto view = m_Registry.view<CameraComponent>();
        for (auto e : view) {
            Tabby::GameObject gameObject = { e, this };
            auto& transform = gameObject.GetComponent<TransformComponent>();
            auto& camera = gameObject.GetComponent<CameraComponent>();
        }
    }

    {
        auto group = m_Registry.group<AnimationComponent>(entt::get<SpriteRendererComponent>);
        for (auto entity : group) {

            auto [animator, sprite] = group.get<AnimationComponent, SpriteRendererComponent>(entity);

            if (animator.currentAnimation.first != "null") {
                bool newFrame = animator.currentAnimation.second->UpdateFrame(dt);

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

void Scene::LateUpdate(float dt)
{

    {
        m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc) {
            // Scene On ScenePlay
            if (!nsc.Instance) {

                nsc.Instance = nsc.InstantiateScript();
                nsc.Instance->m_GameObject = { entity, this };
                nsc.Instance->OnCreate();
            }
            nsc.Instance->LateUpdate(dt);
        });
    }

    {
        const int32_t velocityIteration = 6;
        const int32_t positionIterations = 2;

        // m_PhysiscWorld->Step(dt, velocityIteration, positionIterations);
        physics.Update(dt);
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

    BeginMode3D(ActiveCamera);

    physics.Draw();

#ifdef DEBUG
    DrawGrid(100, 1.0f);
#endif // DEBUG

    {
        auto view = m_Registry.view<SpriteRendererComponent>();
        for (auto e : view) {
            Tabby::GameObject gameObject = { e, this };
            auto& transform = gameObject.GetComponent<TransformComponent>();
            auto& sprite = gameObject.GetComponent<SpriteRendererComponent>();

            Graphics::DrawSprite(transform.GetTransform(), sprite.Texture, sprite.srcRec, transform.Position, transform.Rotation, { 0.0f, 0.0f }, { transform.Scale.x, transform.Scale.y }, sprite.Tint);
        }
    }

    EndMode3D();
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
