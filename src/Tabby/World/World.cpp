#include <Tabby/Physics/2D/Physics2D.h>
#include <Tabby/Renderer/Renderer2D.h>
#include <Tabby/Asset/AssetManager.h>
#include <Tabby/Audio/AudioEngine.h>
#include <Tabby/Audio/AudioSource.h>
#include <Tabby/Core/Application.h>
#include <Tabby/Core/Time/Time.h>
#include <Tabby/Renderer/Mesh.h>
#include <Tabby/World/Entity.h>
#include <Tabby/World/World.h>
#include <Tabby/Debug/Debug.h>
#include <Tabby/Math/Math.h>

#include <glm/gtx/quaternion.hpp>
#include <box2d/box2d.h>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>

namespace Tabby {

struct ParallelTransformUpdate : enki::ITaskSet {
    ParallelTransformUpdate(Entity e)
    {
        m_Entity = e;
    }
    void ExecuteRange(enki::TaskSetPartition range_, uint32_t threadnum_) override
    {
        TB_PROFILE_SCOPE_NAME("Tabby::World::PreUpdate::TransformUpdate::Iteration");
        auto& hierarchy_node_component = m_Entity.GetComponent<HierarchyNodeComponent>();
        auto& transform = m_Entity.GetComponent<TransformComponent>();

        if (Entity parent = m_Entity.GetParent(); !parent.Valid()) {
            transform.worldPosition = transform.position;
            transform.worldRotation = transform.rotation;
            transform.worldScale = transform.scale;
        }

        Matrix4 localRotation = glm::toMat4(Quaternion(glm::radians((Vector3&)transform.rotation)));
        transform.transformMatrix = glm::translate(Matrix4(1.0f), (Vector3&)transform.position) * localRotation * glm::scale(Matrix4(1.0f), (Vector3&)transform.scale);

        Matrix4 rotation = glm::toMat4(Quaternion(glm::radians((Vector3&)transform.worldRotation)));
        transform.worldTransformMatrix = glm::translate(Matrix4(1.0f), (Vector3&)transform.worldPosition) * rotation * glm::scale(Matrix4(1.0f), (Vector3&)transform.worldScale);

        for (auto& child : hierarchy_node_component.Children) {
            auto& childTransform = World::GetRegistry().get<TransformComponent>(child.second);
            childTransform.ApplyWorldTransform(transform.GetWorldTransform() * childTransform.GetTransform());
        }

        if (m_Entity.HasComponent<Rigidbody2DComponent>()) {
            auto& rb2d = m_Entity.GetComponent<Rigidbody2DComponent>();

            if (B2_IS_NON_NULL(rb2d.RuntimeBodyId))
                b2Body_SetTransform(rb2d.RuntimeBodyId, { transform.worldPosition.x, transform.worldPosition.y }, b2MakeRot(Math::DEG2RAD * transform.worldRotation.z));
        }
    }

    Entity m_Entity;
};

void World::UpdateEntityTransforms()
{
    auto view = World::GetRegistry().view<TransformComponent>();

    std::vector<enki::TaskSet*> tasks;
    for (auto e : view) {
        ParallelTransformUpdate* task = new ParallelTransformUpdate(e);
        s_Instance->m_TaskScheduler.AddTaskSetToPipe(task);
    }
    s_Instance->m_TaskScheduler.WaitforAll();
    for (auto* task : tasks) {
        delete task;
    }
    tasks.clear();
}

World::World()
{
    TB_PROFILE_SCOPE_NAME("Tabby::World::Constructor");
    TB_CORE_ASSERT_TAGGED(!s_Instance, "World already exists!");
    s_Instance = this;
}

void World::Init()
{

    TB_PROFILE_SCOPE_NAME("Tabby::World::Init");
    if (!s_Instance) {

        s_Instance = new World();
        Physics2D::Init();

        s_Instance->m_TaskScheduler.Initialize((int)(enki::GetNumHardwareThreads() * 0.75f));

        AddSystem(Schedule::PreUpdate, [](entt::registry&) {
            TB_PROFILE_SCOPE_NAME("Tabby::World::PreUpdate::UpdatePhysics2DWorld");
            Physics2D::UpdateWorld();

            // Retrieve transform from Box2D
            auto view = World::GetRegistry().view<Rigidbody2DComponent>();

            std::vector<enki::TaskSet*> tasks;
            for (auto e : view) {
                enki::TaskSet* task = new enki::TaskSet(1, [e](enki::TaskSetPartition range_, uint32_t threadnum_) {
                    TB_PROFILE_SCOPE_NAME("Tabby::World::PreUpdate::UpdatePhysics2DWorld::UpdateRigidbodyEntity");
                    Entity entity = { e };
                    auto& transform = entity.GetComponent<TransformComponent>();
                    auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

                    const auto& position = b2Body_GetPosition(rb2d.RuntimeBodyId);
                    transform.position.x = position.x;
                    transform.position.y = position.y;
                    transform.rotation.z = Math::RAD2DEG * b2Rot_GetAngle(b2Body_GetRotation(rb2d.RuntimeBodyId));

                    if (Entity parent = entity.GetParent(); parent.Valid()) {
                        transform.position.x -= transform.GetWorldPosition().x;
                        transform.position.y -= transform.GetWorldPosition().y;
                        transform.rotation.z -= transform.GetWorldRotation().z;
                    }
                });

                tasks.push_back(task);
                s_Instance->m_TaskScheduler.AddTaskSetToPipe(task);
            }

            s_Instance->m_TaskScheduler.WaitforAll();
            for (auto* task : tasks) {
                delete task;
            }
            tasks.clear();
        });

        AddSystem(Schedule::PostUpdate, [](entt::registry&) {
            TB_PROFILE_SCOPE_NAME("Tabby::World::PostUpdate::SetCurrentCamera");

            auto view = World::GetRegistry().view<TransformComponent, CameraComponent>();
            for (auto entity : view) {

                TB_PROFILE_SCOPE_NAME("Tabby::World::PostUpdate::SetCurrentCamera::Iteration");
                auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

                if (camera.Primary) {
                    World::SetCurrentCamera(&camera.Camera, &transform.GetWorldTransform());
                    break;
                }
            }
        });
        AddSystem(Schedule::PostUpdate, [](entt::registry& registry) {
            TB_PROFILE_SCOPE_NAME("Tabby::World::PostUpdate::UpdateAudioSource");
            auto view = World::GetRegistry().view<TransformComponent, AudioSourceComponent>();

            std::vector<enki::TaskSet*> tasks;
            for (auto entity : view) {
                TB_PROFILE_SCOPE_NAME("Tabby::World::PostUpdate::UpdateAudioSource::Iteration");
                enki::TaskSet* task = new enki::TaskSet(1, [entity, view](enki::TaskSetPartition range_, uint32_t threadnum_) {
                    auto [transform, source] = view.get<TransformComponent, AudioSourceComponent>(entity);

                    source.Source->SetPosition(transform.GetWorldPosition());
                    source.Source->SetDirection(transform.GetWorldRotation());

                    if (World::GetRegistry().any_of<Rigidbody2DComponent>(entity)) {
                        auto& rb = World::GetRegistry().get<Rigidbody2DComponent>(entity);

                        source.Source->SetVelocity({ rb.GetVelocity().x, rb.GetVelocity().y, 0 });
                    }
                });

                tasks.push_back(task);
                s_Instance->m_TaskScheduler.AddTaskSetToPipe(task);
            }

            s_Instance->m_TaskScheduler.WaitforAll();
            for (auto* task : tasks) {
                delete task;
            }
            tasks.clear();
        });

        AddSystem(Schedule::PostUpdate, [](entt::registry& registry) {
            TB_PROFILE_SCOPE_NAME("Tabby::World::PostUpdate::UpdateAudioListener");

            int numberOfListener = 0;

            auto view = World::GetRegistry().view<TransformComponent, AudioListenerComponent>();
            for (auto entity : view) {
                TB_PROFILE_SCOPE_NAME("Tabby::World::PostUpdate::UpdateAudioListener::Iteration");

                auto [transform, listener] = view.get<TransformComponent, AudioListenerComponent>(entity);

                if (numberOfListener == 0) {

                    glm::vec3 lookVector = glm::normalize(glm::vec3(transform.GetWorldTransform()[0][2], transform.GetWorldTransform()[1][2], transform.GetWorldTransform()[2][2]));
                    glm::vec3 upVector = glm::normalize(glm::vec3(transform.GetWorldTransform()[0][1], transform.GetWorldTransform()[1][1], transform.GetWorldTransform()[2][1]));
                    lookVector.z *= -1;
                    AudioEngine::SetPosition(transform.GetWorldPosition());
                    AudioEngine::SetOrientation(lookVector, upVector);

                    if (registry.any_of<Rigidbody2DComponent>(entity)) {
                        auto& rb = registry.get<Rigidbody2DComponent>(entity);

                        AudioEngine::SetVelocity({ rb.GetVelocity().x, rb.GetVelocity().y, 0 });
                    }
                }

                numberOfListener++;
            }

            static bool warnOnce = false;
            if (!warnOnce && numberOfListener > 1) {
                TB_CORE_WARN("There is multiple listener in scene. Only one is allowed! (warn once)");
                warnOnce = true;
            }
        });

        AddSystem(Schedule::Draw, [](entt::registry&) {
            TB_PROFILE_SCOPE_NAME("Tabby::World::Draw::RenderCircle");
            auto view = World::GetRegistry().view<TransformComponent, CircleRendererComponent>();

            for (auto entity : view) {
                TB_PROFILE_SCOPE_NAME("Tabby::World::Draw::RenderCircle::Iteration");

                auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);
                Renderer2D::DrawCircle(transform.GetWorldTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
            }
        });

        AddSystem(Schedule::Draw, [](entt::registry&) {
            TB_PROFILE_SCOPE_NAME("Tabby::World::Draw::RenderSprites");

            World::GetRegistry().sort<SpriteRendererComponent>([](const auto& lhs, const auto& rhs) {
                TB_PROFILE_SCOPE_NAME("Tabby::World::Draw::RenderSprites::UpdateOrder");
                return lhs.renderOrder < rhs.renderOrder;
            });

            auto sprite_view = World::GetRegistry().view<SpriteRendererComponent>();

            for (const entt::entity entity : sprite_view) {
                TB_PROFILE_SCOPE_NAME("Tabby::World::Draw::RenderSprites::Iteration");

                auto transform = World::GetRegistry().get<TransformComponent>(entity);
                auto sprite = World::GetRegistry().get<SpriteRendererComponent>(entity);

                Renderer2D::DrawSprite(transform.GetWorldTransform(), sprite, (int)entity);
            }
        });

        AddSystem(Schedule::Draw, [](entt::registry&) {
            TB_PROFILE_SCOPE_NAME("Tabby::World::Draw::RenderGLTF");
            auto view = World::GetRegistry().view<TransformComponent, MeshComponent>();
            for (auto entity : view) {
                TB_PROFILE_SCOPE_NAME("Tabby::World::Draw::RenderGLTF::Iteration");

                auto [transform, mC] = view.get<TransformComponent, MeshComponent>(entity);

                if (mC.m_Mesh) {
                    mC.m_Mesh->SetTransform(transform.GetWorldTransform());
                    mC.m_Mesh->Render();
                }
            }
        });

        AddSystem(Schedule::Draw, [](entt::registry&) {
            TB_PROFILE_SCOPE_NAME("Tabby::World::Draw::RenderText");

            auto view = GetRegistry().view<TransformComponent, TextComponent>();
            for (auto entity : view) {
                TB_PROFILE_SCOPE_NAME("Tabby::World::Draw::RenderText::Iteration");

                auto [transform, text] = view.get<TransformComponent, TextComponent>(entity);

                Renderer2D::DrawString(text.TextString, transform.GetWorldTransform(), text, (int)entity);
            }
        });

        AddSystem(Schedule::Draw, [](entt::registry&) {
            TB_PROFILE_SCOPE_NAME("Tabby::World::OnUpdate::RenderScene::DebugDraw");

            Debug::ProcessDrawCalls();
        });
    }
}

template <typename... Component>
static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
{
    TB_PROFILE_SCOPE_NAME("Tabby::World::CopyComponent");
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
    TB_PROFILE_SCOPE_NAME("Tabby::World::CopyComponentIfExists");

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

void World::AddSystem(Schedule schedule, const std::function<void(entt::registry&)>& function)
{
    TB_PROFILE_SCOPE_NAME("Tabby::World::AddSystem");

    switch (schedule) {
    case Schedule::PreStartup:
        s_Instance->m_PreStartupSystems.push_back(function);
        break;
    case Schedule::Startup:
        s_Instance->m_StartupSystems.push_back(function);
        break;
    case Schedule::PostStartup:
        s_Instance->m_PostStartupSystems.push_back(function);
        break;

    case Schedule::PreUpdate:
        s_Instance->m_PreUpdateSystems.push_back(function);
        break;
    case Schedule::Update:
        s_Instance->m_UpdateSystems.push_back(function);
        break;
    case Schedule::PostUpdate:
        s_Instance->m_PostUpdateSystems.push_back(function);
        break;

    case Schedule::FixedPreUpdate:
        s_Instance->m_FixedPreUpdateSystems.push_back(function);
        break;
    case Schedule::FixedUpdate:
        s_Instance->m_FixedUpdateSystems.push_back(function);
        break;
    case Schedule::FixedPostUpdate:
        s_Instance->m_FixedPostUpdateSystems.push_back(function);
        break;

    case Schedule::Draw:
        s_Instance->m_DrawSystems.push_back(function);
        break;
    }
}

Entity World::CreateEntity(const std::string& name)
{
    return CreateEntityWithUUID(UUID(), name);
}

Entity World::CreateEntityWithUUID(UUID uuid, const std::string& name)
{
    TB_PROFILE_SCOPE_NAME("Tabby::World::CreateEntityWithUUID");

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
    TB_PROFILE_SCOPE_NAME("Tabby::World::DestroyEntity");

    if (entity.HasComponent<Rigidbody2DComponent>()) {
        auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

        if (B2_IS_NON_NULL(rb2d.RuntimeBodyId)) {
            b2DestroyBody(rb2d.RuntimeBodyId);
            rb2d.RuntimeBodyId = b2_nullBodyId;
        }
    }

    auto& hc = entity.GetComponent<HierarchyNodeComponent>();
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
    TB_PROFILE_SCOPE_NAME("Tabby::World::DestroyEntityWithChildren");

    if (entity.HasComponent<Rigidbody2DComponent>()) {
        auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

        if (B2_IS_NON_NULL(rb2d.RuntimeBodyId)) {
            b2DestroyBody(rb2d.RuntimeBodyId);
            rb2d.RuntimeBodyId = b2_nullBodyId;
        }
    }

    // IDComponent uuid_component = entity.GetComponent<IDComponent>();
    HierarchyNodeComponent& hierarchy_node_component = entity.GetComponent<HierarchyNodeComponent>();
    for (auto& child : hierarchy_node_component.Children) {
        DestroyEntityWithChildren(Entity(child.second));
    }
    DestroyEntity(entity);
}

Entity World::DuplicateEntity(Entity entity)
{
    TB_PROFILE_SCOPE_NAME("Tabby::World::DuplicateEntity");
    // Copy name because we're going to modify component data structure
    std::string name = entity.GetName();
    Entity newEntity = CreateEntity(name);
    CopyComponentIfExists(AllComponents {}, newEntity, entity);
    return newEntity;
}

Entity World::FindEntityByName(std::string_view name)
{
    TB_PROFILE_SCOPE_NAME("Tabby::World::FindEntityByName");

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
    TB_PROFILE_SCOPE_NAME("Tabby::World::FindEntityByName");
    // TODO(Yan): Maybe should be assert
    if (s_Instance->m_EntityMap.find(uuid) != s_Instance->m_EntityMap.end())
        return { s_Instance->m_EntityMap.at(uuid) };

    TB_CORE_ASSERT_TAGGED(false, "Entity not found!");
    return {};
}

Entity World::GetPrimaryCameraEntity()
{
    TB_PROFILE_SCOPE_NAME("Tabby::World::GetPrimaryCameraEntity");

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
    TB_PROFILE_SCOPE_NAME("Tabby::World::OnStart");

    s_Instance->m_IsRunning = true;

    UpdateEntityTransforms();

    for (const auto& preStartup : s_Instance->m_PreStartupSystems)
        preStartup(s_Instance->m_EntityRegistry);

    for (const auto& startup : s_Instance->m_StartupSystems)
        startup(s_Instance->m_EntityRegistry);

    for (const auto& postStartup : s_Instance->m_PostStartupSystems)
        postStartup(s_Instance->m_EntityRegistry);
}

void World::OnStop()
{
    TB_PROFILE_SCOPE_NAME("Tabby::World::OnStop");
    s_Instance->m_IsRunning = false;

    // TODO: Probably IsPersistent needs to be removed from engine
    {
        auto view = GetRegistry().view<IDComponent>();
        for (auto entityHandle : view) {

            Entity entity = { entityHandle };

            DestroyEntity(entity);
        }
    }
}

void World::Update()
{
    TB_PROFILE_SCOPE_NAME("Tabby::World::Update");

    if (!s_Instance->m_IsPaused || s_Instance->m_StepFrames-- > 0) {

        UpdateEntityTransforms();

        for (const auto& preUpdate : s_Instance->m_PreUpdateSystems)
            preUpdate(s_Instance->m_EntityRegistry);

        for (const auto& update : s_Instance->m_UpdateSystems)
            update(s_Instance->m_EntityRegistry);

        for (const auto& postUpdate : s_Instance->m_PostUpdateSystems)
            postUpdate(s_Instance->m_EntityRegistry);

        double fixedTimeStep = 1.0 / Application::GetSpecification().FixedUpdateRate;

        s_Instance->m_FixedUpdateAccumulator += Time::GetDeltaTime();

        while (s_Instance->m_FixedUpdateAccumulator >= fixedTimeStep) {

            for (const auto& fixedPreUpdate : s_Instance->m_FixedPreUpdateSystems)
                fixedPreUpdate(s_Instance->m_EntityRegistry);

            for (const auto& fixedUpdate : s_Instance->m_FixedUpdateSystems)
                fixedUpdate(s_Instance->m_EntityRegistry);

            for (const auto& fixedPostUpdate : s_Instance->m_FixedPostUpdateSystems)
                fixedPostUpdate(s_Instance->m_EntityRegistry);

            s_Instance->m_FixedUpdateAccumulator -= fixedTimeStep;
        }
    }

    if (s_Instance->m_CurrentCamera) {

#ifndef TB_HEADLESS
        Renderer2D::BeginScene(*s_Instance->m_CurrentCamera, *s_Instance->m_CurrentCameraTransform);

        for (const auto& draw : s_Instance->m_DrawSystems)
            draw(s_Instance->m_EntityRegistry);

        Renderer2D::EndScene();
#endif
    }
}

void World::OnViewportResize(uint32_t width, uint32_t height)
{
    TB_PROFILE_SCOPE_NAME("Tabby::World::OnViewportResize");

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
    TB_PROFILE_SCOPE_NAME("Tabby::World::Step");

    s_Instance->m_StepFrames = frames;
}

void World::SetCurrentCamera(Camera* currentCamera, Matrix4* currentCameraTransform)
{
    TB_PROFILE_SCOPE_NAME("Tabby::World::SetCurrentCamera");

    s_Instance->m_CurrentCamera = currentCamera;
    s_Instance->m_CurrentCameraTransform = currentCameraTransform;
}

entt::registry& World::GetRegistry()
{
    TB_PROFILE_SCOPE_NAME("Tabby::World::GetRegistry");

    return s_Instance->m_EntityRegistry;
}

template <typename T>
void World::OnComponentAdded(Entity entity, T& component)
{
    // static_assert(sizeof(T) == 0);
}

template <>
void World::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
{
    TB_PROFILE_SCOPE_NAME("Tabby::World::OnComponentAdded<IDComponent>");
}

template <>
void World::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
{
    TB_PROFILE_SCOPE_NAME("Tabby::World::OnComponentAdded<TransformComponent>");
}

template <>
void World::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
{
    TB_PROFILE_SCOPE_NAME("Tabby::World::OnComponentAdded<CameraComponent>");

    if (s_Instance->m_ViewportWidth > 0 && s_Instance->m_ViewportHeight > 0)
        component.Camera.SetViewportSize(s_Instance->m_ViewportWidth, s_Instance->m_ViewportHeight);
}

template <>
void World::OnComponentAdded<AudioSourceComponent>(Entity entity, AudioSourceComponent& component)
{
    TB_PROFILE_SCOPE_NAME("Tabby::World::OnComponentAdded<AudioSourceComponent>");

    if (!component.Source)
        component.Source = AudioEngine::CreateAudioSource();
}

template <>
void World::OnComponentAdded<AudioListenerComponent>(Entity entity, AudioListenerComponent& component)
{
    TB_PROFILE_SCOPE_NAME("Tabby::World::OnComponentAdded<AudioSourceComponent>");
}

template <>
void World::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
{
    TB_PROFILE_SCOPE_NAME("Tabby::World::OnComponentAdded<SpriteRendererComponent>");
}

template <>
void World::OnComponentAdded<HierarchyNodeComponent>(Entity entity, HierarchyNodeComponent& component)
{
    TB_PROFILE_SCOPE_NAME("Tabby::World::OnComponentAdded<HierarchyNodeComponent>");
}

template <>
void World::OnComponentAdded<CircleRendererComponent>(Entity entity, CircleRendererComponent& component)
{
    TB_PROFILE_SCOPE_NAME("Tabby::World::OnComponentAdded<CircleRendererComponent>");
}

template <>
void World::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
{
    TB_PROFILE_SCOPE_NAME("Tabby::World::OnComponentAdded<TagComponent>");
}

template <>
void World::OnComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component)
{
    TB_PROFILE_SCOPE_NAME("Tabby::World::OnComponentAdded<Rigidbody2DComponent>");

    if (B2_IS_NULL(Physics2D::GetPhysicsWorld()))
        return;

    if (B2_IS_NON_NULL(component.RuntimeBodyId))
        return;

    BodyInfo2D bodyInfo {
        entity,
    };

    Physics2D::EnqueueBodyInit(bodyInfo);
}

template <>
void World::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component)
{
    TB_PROFILE_SCOPE_NAME("Tabby::World::OnComponentAdded<BoxCollider2DComponent>");

    if (component.QueuedForInitialization || B2_IS_NON_NULL(component.RuntimeShapeId))
        return;

    component.worldId = Physics2D::GetPhysicsWorld();

    ShapeInfo2D ShapeInfo = {
        entity,
        ColliderType2D::Box,
    };

    Physics2D::EnqueueShapeInit(ShapeInfo);
    component.QueuedForInitialization = true;
}

template <>
void World::OnComponentAdded<CircleCollider2DComponent>(Entity entity, CircleCollider2DComponent& component)
{
    TB_PROFILE_SCOPE_NAME("Tabby::World::OnComponentAdded<CircleCollider2DComponent>");

    if (component.QueuedForInitialization || B2_IS_NON_NULL(component.RuntimeShapeId))
        return;

    component.WorldId = Physics2D::GetPhysicsWorld();

    ShapeInfo2D shapeInfo {
        entity,
        ColliderType2D::Circle
    };

    Physics2D::EnqueueShapeInit(shapeInfo);
    component.QueuedForInitialization = true;
}

template <>
void World::OnComponentAdded<CapsuleCollider2DComponent>(Entity entity, CapsuleCollider2DComponent& component)
{
    TB_PROFILE_SCOPE_NAME("Tabby::World::OnComponentAdded<CapsuleCollider2DComponent>");

    if (component.QueuedForInitialization || B2_IS_NON_NULL(component.RuntimeShapeId))
        return;

    component.WorldId = Physics2D::GetPhysicsWorld();

    ShapeInfo2D shapeInfo {
        entity,
        ColliderType2D::Capsule
    };

    Physics2D::EnqueueShapeInit(shapeInfo);
    component.QueuedForInitialization = true;
}

template <>
void World::OnComponentAdded<PolygonCollider2DComponent>(Entity entity, PolygonCollider2DComponent& component)
{
    TB_PROFILE_SCOPE_NAME("Tabby::World::OnComponentAdded<PolygonCollider2DComponent>");

    if (component.QueuedForInitialization || B2_IS_NON_NULL(component.RuntimeShapeId))
        return;

    component.WorldId = Physics2D::GetPhysicsWorld();

    ShapeInfo2D shapeInfo {
        entity,
        ColliderType2D::Polygon
    };

    Physics2D::EnqueueShapeInit(shapeInfo);
    component.QueuedForInitialization = true;
}

template <>
void World::OnComponentAdded<SegmentCollider2DComponent>(Entity entity, SegmentCollider2DComponent& component)
{
    TB_PROFILE_SCOPE_NAME("Tabby::World::OnComponentAdded<SegmentCollider2DComponent>");

    if (component.QueuedForInitialization || B2_IS_NON_NULL(component.RuntimeShapeId))
        return;

    component.WorldId = Physics2D::GetPhysicsWorld();

    ShapeInfo2D shapeInfo {
        entity,
        ColliderType2D::Segment
    };

    Physics2D::EnqueueShapeInit(shapeInfo);
    component.QueuedForInitialization = true;
}

template <>
void World::OnComponentAdded<TextComponent>(Entity entity, TextComponent& component)
{
    TB_PROFILE_SCOPE_NAME("Tabby::World::OnComponentAdded<TextComponent>");

    component.Font = AssetManager::LoadAssetSource("fonts/opensans/OpenSans-Regular.ttf");
}

template <>
void World::OnComponentAdded<MeshComponent>(Entity entity, MeshComponent& component)
{
    TB_PROFILE_SCOPE_NAME("Tabby::World::OnComponentAdded<MeshComponent>");
}
}
