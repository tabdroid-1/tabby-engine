#include "Scene.h"
#include "Entity.h"
#include "ScriptableEntity.h"
#include "glm/fwd.hpp"

#include "Components.h"
#include "Tabby/Physics/2D/Physics2D.h"
#include "Tabby/Renderer/Renderer2D.h"
// #include <Tabby/Audio/AudioManager.h>
// #include <Tabby/Audio/AudioSource.h>
#include <Tabby/Audio/AudioEngine.h>

#include <Tabby/Math/Math.h>
#include <Tabby/Physics/2D/Physics2DTypes.h>
#include <algorithm>
#include <cstdint>
#include <glm/glm.hpp>
#include <type_traits>

// Box2D
#include "box2d/b2_body.h"
#include "box2d/b2_circle_shape.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_settings.h"
#include "box2d/b2_world.h"

namespace Tabby {

Scene::Scene()
{
}

Scene::~Scene()
{
}

template <typename... Component>
static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
{
    // ([&]() {
    //     auto view = src.view<Component>();
    //     for (auto srcEntity : view) {
    //         entt::entity dstEntity = enttMap.at(src.get<IDComponent>(srcEntity).ID);
    //
    //         auto& srcComponent = src.get<Component>(srcEntity);
    //         dst.emplace_or_replace<Component>(dstEntity, srcComponent);
    //     }
    // }(),
    //     ...);
    ([&]() {
        auto view = src.view<Component>();
        for (auto srcEntity : view) {
            UUID srcUUID = src.get<IDComponent>(srcEntity).ID;
            if (enttMap.find(srcUUID) != enttMap.end()) {
                entt::entity dstEntity = enttMap.at(srcUUID);
                auto& srcComponent = src.get<Component>(srcEntity);
                dst.emplace_or_replace<Component>(dstEntity, srcComponent);

                if (std::is_same_v<Component, Rigidbody2DComponent>) {
                    auto& rb = dst.get<Rigidbody2DComponent>(dstEntity);

                    b2Body* body = (b2Body*)rb.RuntimeBody;
                    body->GetUserData().pointer = static_cast<std::uintptr_t>(dstEntity);
                }

                // if (std::is_same_v<Component, TransformComponent>) {

                // auto& transformComponent = dst.get<TransformComponent>(dstEntity);
                // for (auto& childTransform : transformComponent.Children) {
                //     childTransform.second = enttMap.find(childTransform.first)->second;
                // }
                //
                // if (transformComponent.Parent.first != NULL) {
                //     auto it = enttMap.find(transformComponent.Parent.first);
                //     if (it != enttMap.end()) {
                //         transformComponent.Parent.second = it->second;
                //     }
                // }
                // }
            }
        }
    }(),
        ...);
}

template <typename... Component>
static void CopyComponent(ComponentGroup<Component...>, entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
{
    CopyComponent<Component...>(dst, src, enttMap);
}

template <typename... Component>
static void CopyComponentIfExists(Entity dst, Entity src)
{
    ([&]() {
        if (src.HasComponent<Component>())
            dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
    }(),
        ...);
}

template <typename... Component>
static void CopyComponentIfExists(ComponentGroup<Component...>, Entity dst, Entity src)
{
    CopyComponentIfExists<Component...>(dst, src);
}

Shared<Scene> Scene::Copy(Shared<Scene> other)
{
    // Shared<Scene> newScene = CreateShared<Scene>();
    //
    // newScene->m_ViewportWidth = other->m_ViewportWidth;
    // newScene->m_ViewportHeight = other->m_ViewportHeight;
    //
    // auto& srcSceneRegistry = other->m_Registry;
    // auto& dstSceneRegistry = newScene->m_Registry;
    // std::unordered_map<UUID, entt::entity> enttMap;
    //
    // // Create entities in new scene
    // auto idView = srcSceneRegistry.view<IDComponent>();
    // for (auto e : idView) {
    //     UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
    //     const auto& name = srcSceneRegistry.get<TagComponent>(e).Tag;
    //     Entity newEntity = newScene->CreateEntityWithUUID(uuid, name);
    //     enttMap[uuid] = (entt::entity)newEntity;
    // }
    //
    // // Copy components (except IDComponent and TagComponent)
    // CopyComponent(AllComponents {}, dstSceneRegistry, srcSceneRegistry, enttMap);
    //
    // return newScene;
}

void Scene::GetPersistentEntities(Shared<Scene> other)
{

    // auto& srcSceneRegistry = other->m_Registry;
    // std::unordered_map<UUID, entt::entity> enttMap;
    //
    // // Create entities in new scene
    // auto idView = srcSceneRegistry.view<IDComponent>();
    // for (auto e : idView) {
    //
    //     // If should be destroyed on load ignore and if it has Rigidbody2DComponent delete body from world.
    //     if (!srcSceneRegistry.get<IDComponent>(e).DoNotDestroyOnLoad) {
    //         if (srcSceneRegistry.any_of<Rigidbody2DComponent>(e)) {
    //             auto& rb = srcSceneRegistry.get<Rigidbody2DComponent>(e);
    //             if ((b2Body*)rb.RuntimeBody)
    //                 Physisc2D::GetPhysicsWorld()->DestroyBody((b2Body*)rb.RuntimeBody);
    //         }
    //         continue;
    //     }
    //
    //     UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
    //     const auto& name = srcSceneRegistry.get<TagComponent>(e).Tag;
    //     Entity newEntity = CreateEntityWithUUID(uuid, name);
    //     enttMap[uuid] = (entt::entity)newEntity;
    // }
    //
    // // Copy components (except IDComponent and TagComponent)
    // CopyComponent(AllComponents {}, m_Registry, srcSceneRegistry, enttMap);
}

Entity Scene::CreateEntity(const std::string& name)
{
    return CreateEntityWithUUID(UUID(), name);
}

Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
{
    Entity entity = { SceneManager::GetRegistry().create() };
    entity.AddComponent<IDComponent>(uuid);
    entity.AddComponent<TransformComponent>();
    entity.AddComponent<HierarchyNodeComponent>();
    auto& tag = entity.AddComponent<TagComponent>();
    tag.Tag = name.empty() ? "Entity" : name;

    m_EntityMap[uuid] = entity;

    return entity;
}

void Scene::DestroyEntity(Entity entity)
{
    if (entity.HasComponent<Rigidbody2DComponent>()) {
        auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

        if (rb2d.RuntimeBody) {
            Physisc2D::GetPhysicsWorld()->DestroyBody((b2Body*)rb2d.RuntimeBody);
            rb2d.RuntimeBody = nullptr;
        }
    }

    if (entity.HasComponent<NativeScriptComponent>()) {

        auto& nsc = entity.GetComponent<NativeScriptComponent>();

        nsc.DestroyScript(&nsc);
    }

    auto& hc = entity.GetComponent<HierarchyNodeComponent>();
    auto& tc = entity.GetComponent<TransformComponent>();
    auto& id_component = entity.GetComponent<IDComponent>();

    for (auto& child : hc.Children) {
        HierarchyNodeComponent& child_node_component = Entity(child.second).GetComponent<HierarchyNodeComponent>();
        child_node_component.Parent = hc.Parent.first == 0 ? std::make_pair(UUID(0), entt::null) : hc.Parent;
    }
    if (hc.Parent.first.Valid()) {
        Entity parent = Entity(hc.Parent.second);
        HierarchyNodeComponent& parent_node_component = parent.GetComponent<HierarchyNodeComponent>();

        for (auto& child : hc.Children)
            parent_node_component.Children.push_back(child);

        for (auto i = parent_node_component.Children.begin(); i != parent_node_component.Children.end(); i++) {
            if (i->first == id_component.ID) {
                parent_node_component.Children.erase(i);
                break;
            }
        }
    }
    m_EntityMap.erase(entity.GetUUID());
    SceneManager::GetRegistry().destroy(entity);
}

void Scene::DestroyEntityWithChildren(Entity entity)
{
    if (entity.HasComponent<Rigidbody2DComponent>()) {
        auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

        if (rb2d.RuntimeBody) {
            Physisc2D::GetPhysicsWorld()->DestroyBody((b2Body*)rb2d.RuntimeBody);
            rb2d.RuntimeBody = nullptr;
        }
    }

    if (entity.HasComponent<NativeScriptComponent>()) {

        auto& nsc = entity.GetComponent<NativeScriptComponent>();

        nsc.DestroyScript(&nsc);
    }

    IDComponent uuid_component = entity.GetComponent<IDComponent>();
    HierarchyNodeComponent& hierarchy_node_component = entity.GetComponent<HierarchyNodeComponent>();
    for (auto& child : hierarchy_node_component.Children) {
        DestroyEntityWithChildren(Entity(child.second));
    }
    DestroyEntity(entity);
}

void Scene::OnStart()
{

    m_IsRunning = true;

    glm::vec2 gravity(0.0f, -20.8f);
    Physisc2D::InitWorld(gravity);

    OnActivate();
}

void Scene::OnStop()
{
    m_IsRunning = false;

    {
        auto view = SceneManager::GetRegistry().view<IDComponent>();
        for (auto entityHandle : view) {

            Entity entity = { entityHandle };
            if (entity.GetComponent<IDComponent>().IsPersistent)
                continue;

            DestroyEntity(entity);
        }
    }

    OnDeactivate();
}

void Scene::OnUpdate(Timestep ts)
{
    SceneManager::ProcessQueue(Physisc2D::IsQueueEmpty());

    if (!m_IsPaused || m_StepFrames-- > 0) {

        // Update scripts
        {
            SceneManager::GetRegistry().view<NativeScriptComponent>().each([=](auto entity, auto& nsc) {
                // TODO: Move to Scene::OnScenePlay
                if (!nsc.Instance) {
                    nsc.Instance = nsc.InstantiateScript();
                    nsc.Instance->m_Entity = Entity { entity };
                    nsc.Instance->OnCreate();
                }

                nsc.Instance->Update(ts);
                nsc.Instance->LateUpdate(ts);

                const double fixedTimeStep = 1.0 / FIXED_UPDATE_RATE;

                m_FixedUpdateAccumulator += ts;

                while (m_FixedUpdateAccumulator >= fixedTimeStep) {
                    nsc.Instance->FixedUpdate(fixedTimeStep);
                    m_FixedUpdateAccumulator -= fixedTimeStep;
                }
            });
        }
        // Physics
        {
            // Physisc2D::ProcessBodyQueue();
            // Physisc2D::ProcessFixtureQueue(); This is in Physisc2D::UpdateWorld now

            const int32_t velocityIterations = 6;
            const int32_t positionIterations = 2;
            Physisc2D::UpdateWorld(ts, velocityIterations, positionIterations);

            // Retrieve transform from Box2D
            auto view = SceneManager::GetRegistry().view<Rigidbody2DComponent>();
            for (auto e : view) {
                Entity entity = { e };
                auto& transform = entity.GetComponent<TransformComponent>();
                auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
                b2Body* body = (b2Body*)rb2d.RuntimeBody;

                const auto& position = body->GetPosition();
                transform.Translation.x = position.x;
                transform.Translation.y = position.y;
                transform.Rotation.z = Math::RAD2DEG * body->GetAngle();
            }
        }

        // Apply transform to children
        {
            auto view = SceneManager::GetRegistry().view<TransformComponent>();

            for (auto entity : view) {
                auto& hierarchy_node_component = SceneManager::GetRegistry().get<HierarchyNodeComponent>(entity);
                auto& transform = SceneManager::GetRegistry().get<TransformComponent>(entity);

                for (auto& child : hierarchy_node_component.Children) {
                    auto& childTransform = SceneManager::GetRegistry().get<TransformComponent>(child.second);
                    childTransform.ApplyTransform(transform.GetTransform() * childTransform.GetLocalTransform());
                }
            };
        }
    }

    // Sound
    {
        auto view = SceneManager::GetRegistry().view<SoundComponent>();
        for (auto e : view) {
            Entity entity = { e };
            auto& sc = entity.GetComponent<SoundComponent>();

            // sc.Sound->SetGain(sc.Gain);

            // if (sc.Playing)
            //     sc.Sound->Play();
        }
    }

    // Render 2D
    Camera* mainCamera = nullptr;
    glm::mat4 cameraTransform;
    {
        auto view = SceneManager::GetRegistry().view<TransformComponent, CameraComponent>();
        for (auto entity : view) {
            auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

            if (camera.Primary) {
                mainCamera = &camera.Camera;
                cameraTransform = transform.GetTransform();
                break;
            }
        }
    }

    if (mainCamera) {
        Renderer2D::BeginScene(*mainCamera, cameraTransform);

        // Draw circles
        {
            auto view = SceneManager::GetRegistry().view<TransformComponent, CircleRendererComponent>();
            for (auto entity : view) {
                auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);

                Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
            }
        }

        // Draw sprites
        {

            std::vector<entt::entity> sortedEntities;

            auto view = SceneManager::GetRegistry().view<SpriteRendererComponent>();
            for (auto entity : view) {
                sortedEntities.push_back(entity);
            }

            std::sort(sortedEntities.begin(), sortedEntities.end(), [this](const entt::entity& a, const entt::entity& b) {
                return SceneManager::GetRegistry().get<SpriteRendererComponent>(a).renderOrder < SceneManager::GetRegistry().get<SpriteRendererComponent>(b).renderOrder;
            });

            for (const entt::entity entity : sortedEntities) {
                auto transform = SceneManager::GetRegistry().get<TransformComponent>(entity);
                auto sprite = SceneManager::GetRegistry().get<SpriteRendererComponent>(entity);

                Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
            }
        }

        // Draw text
        {
            auto view = SceneManager::GetRegistry().view<TransformComponent, TextComponent>();
            for (auto entity : view) {
                auto [transform, text] = view.get<TransformComponent, TextComponent>(entity);

                Renderer2D::DrawString(text.TextString, transform.GetTransform(), text, (int)entity);
            }
        }

        Renderer2D::EndScene();
    }
}

void Scene::OnViewportResize(uint32_t width, uint32_t height)
{
    if (m_ViewportWidth == width && m_ViewportHeight == height)
        return;

    m_ViewportWidth = width;
    m_ViewportHeight = height;

    // Resize our non-FixedAspectRatio cameras
    auto view = SceneManager::GetRegistry().view<CameraComponent>();
    for (auto entity : view) {
        auto& cameraComponent = view.get<CameraComponent>(entity);
        if (!cameraComponent.FixedAspectRatio)
            cameraComponent.Camera.SetViewportSize(width, height);
    }
}

Entity Scene::GetPrimaryCameraEntity()
{
    auto view = SceneManager::GetRegistry().view<CameraComponent>();
    for (auto entity : view) {
        const auto& camera = view.get<CameraComponent>(entity);
        if (camera.Primary)
            return Entity { entity };
    }
    return {};
}

void Scene::Step(int frames)
{
    m_StepFrames = frames;
}

Entity Scene::DuplicateEntity(Entity entity)
{
    // Copy name because we're going to modify component data structure
    std::string name = entity.GetName();
    Entity newEntity = CreateEntity(name);
    CopyComponentIfExists(AllComponents {}, newEntity, entity);
    return newEntity;
}

Entity Scene::FindEntityByName(std::string_view name)
{
    auto view = SceneManager::GetRegistry().view<TagComponent>();
    for (auto entity : view) {
        const TagComponent& tc = view.get<TagComponent>(entity);
        if (tc.Tag == name)
            return Entity { entity };
    }
    return {};
}

Entity Scene::GetEntityByUUID(UUID uuid)
{
    // TODO(Yan): Maybe should be assert
    if (m_EntityMap.find(uuid) != m_EntityMap.end())
        return { m_EntityMap.at(uuid) };

    return {};
}

template <typename T>
void Scene::OnComponentAdded(Entity entity, T& component)
{
    static_assert(sizeof(T) == 0);
}

template <>
void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
{
}

template <>
void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
{
}

template <>
void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
{
    if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
        component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
}

template <>
void Scene::OnComponentAdded<SoundComponent>(Entity entity, SoundComponent& component)
{
}

template <>
void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
{
}

template <>
void Scene::OnComponentAdded<HierarchyNodeComponent>(Entity entity, HierarchyNodeComponent& component)
{
}

template <>
void Scene::OnComponentAdded<CircleRendererComponent>(Entity entity, CircleRendererComponent& component)
{
}

template <>
void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
{
}

template <>
void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
{
}

template <>
void Scene::OnComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component)
{
    // TODO: Move this to ProcessBodyQueue

    if (Physisc2D::GetPhysicsWorld())
        return;

    auto& transform = entity.GetComponent<TransformComponent>();
    auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

    if (rb2d.RuntimeBody)
        return;

    // b2BodyUserData bodyData;
    // bodyData.pointer = static_cast<std::uintptr_t>(entity.GetEntityHandle());
    //
    // b2BodyDef* bodyDef = new b2BodyDef;
    // bodyDef->type = Utils::Rigidbody2DTypeToBox2DBody(rb2d.Type);
    // bodyDef->position.Set(transform.Translation.x, transform.Translation.y);
    // bodyDef->angle = transform.Rotation.z;
    // bodyDef->userData = bodyData;

    BodyInfo2D bodyInfo {
        entity,
    };

    Physisc2D::EnqueueBody(bodyInfo);

    // if (entity.HasComponent<BoxCollider2DComponent>()) {
    //     auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
    //
    //     if (!bc2d.queuedForInitialization) {
    //         // b2PolygonShape* boxShape = new b2PolygonShape;
    //         // boxShape->SetAsBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y, b2Vec2(bc2d.Offset.x, bc2d.Offset.y), 0.0f);
    //         //
    //         // b2FixtureDef* fixtureDef = new b2FixtureDef;
    //         // fixtureDef->shape = boxShape;
    //         // fixtureDef->density = bc2d.Density;
    //         // fixtureDef->friction = bc2d.Friction;
    //         // fixtureDef->restitution = bc2d.Restitution;
    //         // fixtureDef->restitutionThreshold = bc2d.RestitutionThreshold;
    //         //
    //         FixtureInfo2D fixtureInfo {
    //             entity,
    //             Box
    //         };
    //
    //         Physisc2D::EnqueueFixture(fixtureInfo);
    //         bc2d.queuedForInitialization = true;
    //     }
    // }

    // if (entity.HasComponent<CircleCollider2DComponent>()) {
    //     auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();
    //
    //     if (!cc2d.queuedForInitialization) {
    //
    //         b2CircleShape* circleShape = new b2CircleShape;
    //         circleShape->m_p.Set(cc2d.Offset.x, cc2d.Offset.y);
    //         circleShape->m_radius = transform.Scale.x * cc2d.Radius;
    //
    //         b2FixtureDef* fixtureDef = new b2FixtureDef;
    //         fixtureDef->shape = circleShape;
    //         fixtureDef->density = cc2d.Density;
    //         fixtureDef->friction = cc2d.Friction;
    //         fixtureDef->restitution = cc2d.Restitution;
    //         fixtureDef->restitutionThreshold = cc2d.RestitutionThreshold;
    //
    //         FixtureInfo2D fixtureInfo {
    //             entity,
    //             Circle
    //         };
    //
    //         Physisc2D::EnqueueFixture(fixtureInfo);
    //         component.queuedForInitialization = true;
    //     }
    // }
}

template <>
void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component)
{
    if (component.queuedForInitialization || component.RuntimeFixture)
        return;

    FixtureInfo2D fixtureInfo = {
        entity,
        ColliderType2D::Box,
    };

    Physisc2D::EnqueueFixture(fixtureInfo);
    component.queuedForInitialization = true;

    return;

    // if (entity.HasComponent<Rigidbody2DComponent>()) {
    //
    //     // b2PolygonShape* boxShape = new b2PolygonShape;
    //     // boxShape->SetAsBox(component.Size.x * transform.Scale.x, component.Size.y * transform.Scale.y, b2Vec2(component.Offset.x, component.Offset.y), 0.0f);
    //     //
    //     // b2FixtureDef* fixtureDef = new b2FixtureDef;
    //     // fixtureDef->shape = boxShape;
    //     // fixtureDef->density = component.Density;
    //     // fixtureDef->friction = component.Friction;
    //     // fixtureDef->restitution = component.Restitution;
    //     // fixtureDef->restitutionThreshold = component.RestitutionThreshold;
    //
    //     FixtureInfo2D fixtureInfo {
    //         entity,
    //         Box
    //     };
    //
    //     Physisc2D::EnqueueFixture(fixtureInfo);
    //     component.queuedForInitialization = true;
    //
    //     return;
    // }
    //
    // // Iterate through all children recursively and add colliders to parent rigidbody
    // auto AddChildColliders = [](auto&& self, entt::entity startedEntity, entt::entity parentEntity) -> void {
    //     auto& hierarchyNode = Entity(parentEntity).GetComponent<HierarchyNodeComponent>();
    //
    //     for (auto& child : hierarchyNode.Children) {
    //
    //         if (Entity(child.second).HasComponent<Rigidbody2DComponent>())
    //             continue;
    //
    //         auto& transform = Entity(child.second).GetComponent<TransformComponent>();
    //
    //         if (Entity(child.second).HasComponent<BoxCollider2DComponent>()) {
    //             auto& bc2d = Entity(child.second).GetComponent<BoxCollider2DComponent>();
    //
    //             b2PolygonShape* boxShape = new b2PolygonShape;
    //             boxShape->SetAsBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y, b2Vec2(bc2d.Offset.x, bc2d.Offset.y), 0.0f);
    //
    //             b2FixtureDef* fixtureDef = new b2FixtureDef;
    //             fixtureDef->shape = boxShape;
    //             fixtureDef->density = bc2d.Density;
    //             fixtureDef->friction = bc2d.Friction;
    //             fixtureDef->restitution = bc2d.Restitution;
    //             fixtureDef->restitutionThreshold = bc2d.RestitutionThreshold;
    //             Physisc2D::EnqueueFixture(startedEntity, fixtureDef);
    //         }
    //
    //         if (Entity(child.second).HasComponent<CircleCollider2DComponent>()) {
    //             auto& cc2d = Entity(child.second).GetComponent<CircleCollider2DComponent>();
    //
    //             b2CircleShape* circleShape = new b2CircleShape;
    //             circleShape->m_p.Set(cc2d.Offset.x, cc2d.Offset.y);
    //             circleShape->m_radius = transform.Scale.x * cc2d.Radius;
    //
    //             b2FixtureDef* fixtureDef = new b2FixtureDef;
    //             fixtureDef->shape = circleShape;
    //             fixtureDef->density = cc2d.Density;
    //             fixtureDef->friction = cc2d.Friction;
    //             fixtureDef->restitution = cc2d.Restitution;
    //             fixtureDef->restitutionThreshold = cc2d.RestitutionThreshold;
    //             Physisc2D::EnqueueFixture(startedEntity, fixtureDef);
    //         }
    //
    //         self(self, startedEntity, child.second);
    //     }
    // };
    //
    // AddChildColliders(AddChildColliders, entity, entity);
}

template <>
void Scene::OnComponentAdded<CircleCollider2DComponent>(Entity entity, CircleCollider2DComponent& component)
{
    // TODO: Move this to ProcessFixtureQueu

    auto& transform = entity.GetComponent<TransformComponent>();

    if (component.queuedForInitialization || component.RuntimeFixture)
        return;

    FixtureInfo2D fixtureInfo {
        entity,
        ColliderType2D::Circle
    };

    Physisc2D::EnqueueFixture(fixtureInfo);
    component.queuedForInitialization = true;

    // if (entity.HasComponent<Rigidbody2DComponent>()) {
    //
    //     b2CircleShape* circleShape = new b2CircleShape;
    //     circleShape->m_p.Set(component.Offset.x, component.Offset.y);
    //     circleShape->m_radius = transform.Scale.x * component.Radius;
    //
    //     b2FixtureDef* fixtureDef = new b2FixtureDef;
    //     fixtureDef->shape = circleShape;
    //     fixtureDef->density = component.Density;
    //     fixtureDef->friction = component.Friction;
    //     fixtureDef->restitution = component.Restitution;
    //     fixtureDef->restitutionThreshold = component.RestitutionThreshold;
    //
    //     FixtureInfo2D fixtureInfo {
    //         entity,
    //         ColliderType2D::Circle
    //     };
    //
    //     Physisc2D::EnqueueFixture(fixtureInfo);
    //     component.queuedForInitialization = true;
    // }
    //
    // // Iterate through all children recursively and add colliders to parent rigidbody
    // auto AddChildColliders = [](auto&& self, entt::entity startedEntity, entt::entity parentEntity) -> void {
    //     auto& hierarchyNode = Entity(parentEntity).GetComponent<HierarchyNodeComponent>();
    //
    //     for (auto& child : hierarchyNode.Children) {
    //
    //         if (Entity(child.second).HasComponent<Rigidbody2DComponent>())
    //             continue;
    //
    //         auto& transform = Entity(child.second).GetComponent<TransformComponent>();
    //
    //         if (Entity(child.second).HasComponent<BoxCollider2DComponent>()) {
    //             auto& bc2d = Entity(child.second).GetComponent<BoxCollider2DComponent>();
    //
    //             b2PolygonShape* boxShape = new b2PolygonShape;
    //             boxShape->SetAsBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y, b2Vec2(bc2d.Offset.x, bc2d.Offset.y), 0.0f);
    //
    //             b2FixtureDef* fixtureDef = new b2FixtureDef;
    //             fixtureDef->shape = boxShape;
    //             fixtureDef->density = bc2d.Density;
    //             fixtureDef->friction = bc2d.Friction;
    //             fixtureDef->restitution = bc2d.Restitution;
    //             fixtureDef->restitutionThreshold = bc2d.RestitutionThreshold;
    //             Physisc2D::EnqueueFixture(startedEntity, fixtureDef);
    //         }
    //
    //         if (Entity(child.second).HasComponent<CircleCollider2DComponent>()) {
    //             auto& cc2d = Entity(child.second).GetComponent<CircleCollider2DComponent>();
    //
    //             b2CircleShape* circleShape = new b2CircleShape;
    //             circleShape->m_p.Set(cc2d.Offset.x, cc2d.Offset.y);
    //             circleShape->m_radius = transform.Scale.x * cc2d.Radius;
    //
    //             b2FixtureDef* fixtureDef = new b2FixtureDef;
    //             fixtureDef->shape = circleShape;
    //             fixtureDef->density = cc2d.Density;
    //             fixtureDef->friction = cc2d.Friction;
    //             fixtureDef->restitution = cc2d.Restitution;
    //             fixtureDef->restitutionThreshold = cc2d.RestitutionThreshold;
    //             Physisc2D::EnqueueFixture(startedEntity, fixtureDef);
    //         }
    //
    //         self(self, startedEntity, child.second);
    //     }
    // };
    //
    // AddChildColliders(AddChildColliders, entity, entity);
}

template <>
void Scene::OnComponentAdded<TextComponent>(Entity entity, TextComponent& component)
{
}
}
