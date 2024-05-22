#include <Tabby.h>

// #include <Tabby/World/World.h>
// #include <Tabby/World/Entity.h>
//
// #include <Tabby/Physics/2D/Physics2D.h>
// #include <Tabby/Physics/2D/Physics2DTypes.h>
// #include <Tabby/World/ScriptableEntity.h>
// #include <Tabby/Math/Math.h>
// #include <Tabby/Renderer/Renderer2D.h>
// #include <Tabby/World/Components.h>
// #include <Tabby/Asset/AssetManager.h>
// #include <Tabby/Audio/AudioEngine.h>
// #include <Tabby/Debug/Debug.h>

#include <glm/fwd.hpp>
#include <glm/glm.hpp>

#include <glm/gtx/quaternion.hpp>

namespace Tabby {

World* World::s_Instance = nullptr;
std::string World::m_CurrentMap = "";
entt::registry World::m_EntityRegistry;

World::World()
{
    TB_CORE_ASSERT(!s_Instance, "Scene State Machine already exists!");
    s_Instance = this;
}

template <typename... Component>
static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
{
    ([&]() {
        auto view = src.view<Component>();
        for (auto srcEntity : view) {
            entt::entity dstEntity = enttMap.at(src.get<IDComponent>(srcEntity).ID);

            auto& srcComponent = src.get<Component>(srcEntity);
            dst.emplace_or_replace<Component>(dstEntity, srcComponent);
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

void World::LoadMap(const std::string& mapPath)
{

    TB_PROFILE_SCOPE_NAME("World::SwitchTo");
    // auto it = s_Instance->scenes.find(SceneName);
    // if (it != s_Instance->scenes.end()) {
    //
    //     if (s_Instance->curScene.second) {
    //         nextSceneName = it->first;
    //     } else {
    //         private_SwitchTo(SceneName);
    //     }
    // }
}

Entity World::CreateEntity(const std::string& name)
{
    return CreateEntityWithUUID(UUID(), name);
}

Entity World::CreateEntityWithUUID(UUID uuid, const std::string& name)
{
    Entity entity = { s_Instance->GetRegistry().create() };
    entity.AddComponent<IDComponent>(uuid);
    entity.AddComponent<TransformComponent>();
    entity.AddComponent<HierarchyNodeComponent>();
    auto& tag = entity.AddComponent<TagComponent>();
    tag.Tag = name.empty() ? "Entity" : name;

    s_Instance->m_EntityMap[uuid] = entity;

    return entity;
}

void World::DestroyEntity(Entity entity)
{
    if (entity.HasComponent<Rigidbody2DComponent>()) {
        auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

        if (B2_IS_NON_NULL(rb2d.RuntimeBodyId)) {
            b2DestroyBody(rb2d.RuntimeBodyId);
            rb2d.RuntimeBodyId = b2_nullBodyId;
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
    s_Instance->m_EntityMap.erase(entity.GetUUID());
    GetRegistry().destroy(entity);
}

void World::DestroyEntityWithChildren(Entity entity)
{
    if (entity.HasComponent<Rigidbody2DComponent>()) {
        auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

        if (B2_IS_NON_NULL(rb2d.RuntimeBodyId)) {
            b2DestroyBody(rb2d.RuntimeBodyId);
            rb2d.RuntimeBodyId = b2_nullBodyId;
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

Entity World::DuplicateEntity(Entity entity)
{
    // Copy name because we're going to modify component data structure
    std::string name = entity.GetName();
    Entity newEntity = CreateEntity(name);
    CopyComponentIfExists(AllComponents {}, newEntity, entity);
    return newEntity;
}

Entity World::FindEntityByName(std::string_view name)
{
    auto view = GetRegistry().view<TagComponent>();
    for (auto entity : view) {
        const TagComponent& tc = view.get<TagComponent>(entity);
        if (tc.Tag == name)
            return Entity { entity };
    }
    return {};
}

Entity World::GetEntityByUUID(UUID uuid)
{
    // TODO(Yan): Maybe should be assert
    if (s_Instance->m_EntityMap.find(uuid) != s_Instance->m_EntityMap.end())
        return { s_Instance->m_EntityMap.at(uuid) };

    return {};
}

Entity World::GetPrimaryCameraEntity()
{
    auto view = GetRegistry().view<CameraComponent>();
    for (auto entity : view) {
        const auto& camera = view.get<CameraComponent>(entity);
        if (camera.Primary)
            return Entity { entity };
    }
    return {};
}

void World::OnStart()
{
    TB_PROFILE_SCOPE_NAME("World::OnStart");

    if (!s_Instance)
        s_Instance = new World();

    s_Instance->m_IsRunning = true;

    glm::vec2 gravity(0.0f, -20.8f);
    Physisc2D::InitWorld(gravity);
}

void World::OnStop()
{
    TB_PROFILE_SCOPE_NAME("World::OnStop");
    s_Instance->m_IsRunning = false;

    {
        auto view = GetRegistry().view<IDComponent>();
        for (auto entityHandle : view) {

            Entity entity = { entityHandle };
            if (entity.GetComponent<IDComponent>().IsPersistent)
                continue;

            DestroyEntity(entity);
        }
    }
}

void World::Update(Timestep ts)
{
    if (!s_Instance) {
        s_Instance = new World();
    }

    TB_PROFILE_SCOPE_NAME("World::Update");
    // SceneManager::ProcessQueue(Physisc2D::IsQueueEmpty());

    if (!s_Instance->m_IsPaused || s_Instance->m_StepFrames-- > 0) {

        TB_PROFILE_SCOPE_NAME("World::OnUpdate::EntityUpdate");

        // Update scripts
        {
            GetRegistry().view<NativeScriptComponent>().each([=](auto entity, auto& nsc) {
                TB_PROFILE_SCOPE_NAME("World::OnUpdate::EntityUpdate::NativeScriptComponentUpdate");

                if (!nsc.Instance) {
                    nsc.Instance = nsc.InstantiateScript();
                    nsc.Instance->m_Entity = Entity { entity };
                    nsc.Instance->OnCreate();
                }

                nsc.Instance->Update(ts);
                nsc.Instance->LateUpdate(ts);

                const double fixedTimeStep = 1.0 / FIXED_UPDATE_RATE;

                s_Instance->m_FixedUpdateAccumulator += ts;

                while (s_Instance->m_FixedUpdateAccumulator >= fixedTimeStep) {
                    nsc.Instance->FixedUpdate(fixedTimeStep);
                    s_Instance->m_FixedUpdateAccumulator -= fixedTimeStep;
                }
            });
        }
        // Physics
        {
            TB_PROFILE_SCOPE_NAME("World::OnUpdate::EntityUpdate::UpdateWorld");
            const int32_t subStepCount = 6;
            Physisc2D::UpdateWorld(ts, subStepCount);

            // Retrieve transform from Box2D
            auto view = GetRegistry().view<Rigidbody2DComponent>();
            for (auto e : view) {
                Entity entity = { e };
                auto& transform = entity.GetComponent<TransformComponent>();
                auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
                // b2Body* body = (b2Body*)rb2d.RuntimeBody;

                const auto& position = b2Body_GetPosition(rb2d.RuntimeBodyId);
                transform.Translation.x = position.x;
                transform.Translation.y = position.y;
                transform.Rotation.z = Math::RAD2DEG * b2Body_GetAngle(rb2d.RuntimeBodyId);
            }
        }

        // Apply transform to children
        {
            TB_PROFILE_SCOPE_NAME("World::OnUpdate::EntityUpdate::TransformUpdate");
            auto view = GetRegistry().view<TransformComponent>();

            for (auto entity : view) {
                auto& hierarchy_node_component = GetRegistry().get<HierarchyNodeComponent>(entity);
                auto& transform = GetRegistry().get<TransformComponent>(entity);

                glm::mat4 rotation = glm::toMat4(glm::quat(glm::radians(transform.Rotation)));
                transform.TransformMatrix = glm::translate(glm::mat4(1.0f), transform.Translation) * rotation * glm::scale(glm::mat4(1.0f), transform.Scale);

                glm::mat4 localRotation = glm::toMat4(glm::quat(glm::radians(transform.LocalRotation)));
                transform.LocalTransformMatrix = glm::translate(glm::mat4(1.0f), transform.LocalTranslation) * localRotation * glm::scale(glm::mat4(1.0f), transform.LocalScale);

                for (auto& child : hierarchy_node_component.Children) {
                    auto& childTransform = GetRegistry().get<TransformComponent>(child.second);
                    childTransform.ApplyTransform(transform.GetTransform() * childTransform.GetLocalTransform());
                }
            };
        }
    }

    // Sound
    {
        auto view = GetRegistry().view<SoundComponent>();
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
        auto view = GetRegistry().view<TransformComponent, CameraComponent>();
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

        TB_PROFILE_SCOPE_NAME("World::OnUpdate::RenderScene");
        Renderer2D::BeginScene(*mainCamera, cameraTransform);

        // Draw circles
        {
            TB_PROFILE_SCOPE_NAME("World::OnUpdate::RenderScene::RenderCircle");
            auto view = GetRegistry().view<TransformComponent, CircleRendererComponent>();
            for (auto entity : view) {
                auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);

                Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
            }
        }

        // Draw sprites
        {
            TB_PROFILE_SCOPE_NAME("World::OnUpdate::RenderScene::RenderSprites");

            // std::vector<entt::entity> sortedEntities;
            //
            // auto view = SceneManager::GetRegistry().view<SpriteRendererComponent>();
            // for (auto entity : view) {
            //     sortedEntities.push_back(entity);
            // }
            //
            // std::sort(sortedEntities.begin(), sortedEntities.end(), [this](const entt::entity& a, const entt::entity& b) {
            //     return SceneManager::GetRegistry().get<SpriteRendererComponent>(a).renderOrder < SceneManager::GetRegistry().get<SpriteRendererComponent>(b).renderOrder;
            // });

            GetRegistry().sort<SpriteRendererComponent>([](const auto& lhs, const auto& rhs) {
                return lhs.renderOrder < rhs.renderOrder;
            });

            auto sprite_view = GetRegistry().view<SpriteRendererComponent>();

            for (const entt::entity entity : sprite_view) {
                auto transform = GetRegistry().get<TransformComponent>(entity);
                auto sprite = GetRegistry().get<SpriteRendererComponent>(entity);

                Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
            }
        }

        // Draw text
        {
            TB_PROFILE_SCOPE_NAME("World::OnUpdate::RenderScene::RenderText");

            auto view = GetRegistry().view<TransformComponent, TextComponent>();
            for (auto entity : view) {
                auto [transform, text] = view.get<TransformComponent, TextComponent>(entity);

                Renderer2D::DrawString(text.TextString, transform.GetTransform(), text, (int)entity);
            }
        }

        // Draw DebugShapes
        {
            TB_PROFILE_SCOPE_NAME("World::OnUpdate::RenderScene::DebugDraw");

            Debug::ProcessDrawCalls();
        }

        Renderer2D::EndScene();
    }
}

void World::DrawImGui()
{
    TB_PROFILE_SCOPE_NAME("World::DrawImGui");
}

void World::OnViewportResize(uint32_t width, uint32_t height)
{
    TB_PROFILE_SCOPE_NAME("World::OnViewportResize");

    if (s_Instance->m_ViewportWidth == width && s_Instance->m_ViewportHeight == height)
        return;

    s_Instance->m_ViewportWidth = width;
    s_Instance->m_ViewportHeight = height;

    // Resize our non-FixedAspectRatio cameras
    auto view = GetRegistry().view<CameraComponent>();
    for (auto entity : view) {
        auto& cameraComponent = view.get<CameraComponent>(entity);
        if (!cameraComponent.FixedAspectRatio)
            cameraComponent.Camera.SetViewportSize(width, height);
    }
}

void World::Step(int frames)
{
    s_Instance->m_StepFrames = frames;
}

// void World::ProcessQueue(bool canSwitch)
// {
//     TB_PROFILE_SCOPE_NAME("World::ProcessQueue");
//     if (canSwitch && s_Instance->nextSceneName != "") {
//         private_SwitchTo(s_Instance->nextSceneName);
//         s_Instance->nextSceneName = "";
//     }
// }
//
// void World::private_SwitchTo(std::string SceneName)
// {
//     TB_PROFILE_SCOPE_NAME("World::private_SwitchTo");
//
//     auto it = s_Instance->scenes.find(SceneName);
//     if (it != s_Instance->scenes.end()) {
//         if (s_Instance->curScene.second) {
//             s_Instance->curScene.second->OnStop();
//         }
//
//         s_Instance->curScene.first = it->first;
//         s_Instance->curScene.second = it->second;
//
//         TB_CORE_TRACE("Switched to scene {0}", SceneName);
//
//         s_Instance->curScene.second->OnStart();
//     } else {
//         TB_CORE_ERROR("Scene {0} not found", SceneName);
//     }
// }

entt::registry& World::GetRegistry()
{
    return m_EntityRegistry;
}

template <typename T>
void World::OnComponentAdded(Entity entity, T& component)
{
    static_assert(sizeof(T) == 0);
}

template <>
void World::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
{
}

template <>
void World::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
{
}

template <>
void World::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
{
    if (s_Instance->m_ViewportWidth > 0 && s_Instance->m_ViewportHeight > 0)
        component.Camera.SetViewportSize(s_Instance->m_ViewportWidth, s_Instance->m_ViewportHeight);
}

template <>
void World::OnComponentAdded<SoundComponent>(Entity entity, SoundComponent& component)
{
}

template <>
void World::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
{
}

template <>
void World::OnComponentAdded<HierarchyNodeComponent>(Entity entity, HierarchyNodeComponent& component)
{
}

template <>
void World::OnComponentAdded<CircleRendererComponent>(Entity entity, CircleRendererComponent& component)
{
}

template <>
void World::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
{
}

template <>
void World::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
{
}

template <>
void World::OnComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component)
{
    if (B2_IS_NULL(Physisc2D::GetPhysicsWorld()))
        return;

    auto& transform = entity.GetComponent<TransformComponent>();

    if (B2_IS_NON_NULL(component.RuntimeBodyId))
        return;

    BodyInfo2D bodyInfo {
        entity,
    };

    Physisc2D::EnqueueBodyInit(bodyInfo);
}

template <>
void World::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component)
{
    if (component.queuedForInitialization || B2_IS_NON_NULL(component.RuntimeShapeId))
        return;

    component.worldId = Physisc2D::GetPhysicsWorld();

    ShapeInfo2D ShapeInfo = {
        entity,
        ColliderType2D::Box,
    };

    Physisc2D::EnqueueShapeInit(ShapeInfo);
    component.queuedForInitialization = true;
}

template <>
void World::OnComponentAdded<CircleCollider2DComponent>(Entity entity, CircleCollider2DComponent& component)
{
    auto& transform = entity.GetComponent<TransformComponent>();

    if (component.queuedForInitialization || B2_IS_NON_NULL(component.RuntimeShapeId))
        return;

    component.worldId = Physisc2D::GetPhysicsWorld();

    ShapeInfo2D shapeInfo {
        entity,
        ColliderType2D::Circle
    };

    Physisc2D::EnqueueShapeInit(shapeInfo);
    component.queuedForInitialization = true;
}

template <>
void World::OnComponentAdded<CapsuleCollider2DComponent>(Entity entity, CapsuleCollider2DComponent& component)
{
    auto& transform = entity.GetComponent<TransformComponent>();

    if (component.queuedForInitialization || B2_IS_NON_NULL(component.RuntimeShapeId))
        return;

    component.worldId = Physisc2D::GetPhysicsWorld();

    ShapeInfo2D shapeInfo {
        entity,
        ColliderType2D::Capsule
    };

    Physisc2D::EnqueueShapeInit(shapeInfo);
    component.queuedForInitialization = true;
}

template <>
void World::OnComponentAdded<SegmentCollider2DComponent>(Entity entity, SegmentCollider2DComponent& component)
{
    auto& transform = entity.GetComponent<TransformComponent>();

    if (component.queuedForInitialization || B2_IS_NON_NULL(component.RuntimeShapeId))
        return;

    component.worldId = Physisc2D::GetPhysicsWorld();

    ShapeInfo2D shapeInfo {
        entity,
        ColliderType2D::Segment
    };

    Physisc2D::EnqueueShapeInit(shapeInfo);
    component.queuedForInitialization = true;
}

template <>
void World::OnComponentAdded<TextComponent>(Entity entity, TextComponent& component)
{
}

}
