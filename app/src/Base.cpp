#include "Base.h"

#include <Tabby/World/Prefab.h>
#include <Tabby/Renderer/GLTF.h>

#include <Prefab/ExamplePrefab.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <ImGuizmo.h>
#include <CustomComponent/PlayerComponent.h>

static Tabby::Shared<Tabby::Font> s_Font;
float fps = 0;

Base::Base()
    : Layer("Base")
{
    s_Font = Tabby::Font::GetDefault();
}

void Base::OnAttach()
{
    TB_PROFILE_SCOPE();

    Tabby::World::Init();
    Tabby::Application::Get().GetWindow().SetVSync(false);

    Tabby::FramebufferSpecification fbSpec;
    fbSpec.Attachments = { Tabby::FramebufferTextureFormat::RGBA8, Tabby::FramebufferTextureFormat::RED_INTEGER, Tabby::FramebufferTextureFormat::DEPTH24STENCIL8 };
    fbSpec.Width = 2560;
    fbSpec.Height = 1600;
    m_Framebuffer = Tabby::Framebuffer::Create(fbSpec);

    Tabby::World::AddPlugins<PlayerPlugin>();

    Tabby::World::OnStart();

    {
        auto cameraEntity = Tabby::World::CreateEntity("cameraEntity");
        auto cc = cameraEntity.AddComponent<Tabby::CameraComponent>();
        cc.Camera.SetPerspectiveFarClip(10000);
    }

    // {
    //     auto GroundEntity = Tabby::World::CreateEntity("GroundEntity");
    //     // GroundEntity.AddComponent<Tabby::SpriteRendererComponent>();
    //     auto& rb = GroundEntity.AddComponent<Tabby::Rigidbody2DComponent>();
    //     rb.Type = Tabby::Rigidbody2DComponent::BodyType::Static;
    //     auto& bc = GroundEntity.AddComponent<Tabby::BoxCollider2DComponent>();
    //     bc.Size = { 3.0f, 0.5f };
    // }
    //
    // {
    //     auto DynamicEntity = Tabby::World::CreateEntity("DynamicEntity");
    //     DynamicEntity.AddComponent<Tabby::SpriteRendererComponent>();
    //     auto& rb = DynamicEntity.AddComponent<Tabby::Rigidbody2DComponent>();
    //     rb.Type = Tabby::Rigidbody2DComponent::BodyType::Dynamic;
    //     auto& bc = DynamicEntity.AddComponent<Tabby::BoxCollider2DComponent>();
    //     bc.Size = { 2.0f, 0.5f };
    //
    //     DynamicEntity.GetComponent<Tabby::TransformComponent>().Translation.y = 3;
    // }

    {
        auto GLTFEntity = Tabby::World::CreateEntity("GLTFEntity");
        auto& GLTFComponent = GLTFEntity.AddComponent<Tabby::GLTFComponent>();
        GLTFComponent.m_GLTF = Tabby::CreateShared<Tabby::GLTF>("scenes/sponza-small/sponza.gltf");
        // GLTFComponent.m_GLTF = Tabby::CreateShared<Tabby::GLTF>("scenes/sponza/Sponza.gltf");

        // DynamicEntity.GetComponent<Tabby::TransformComponent>().Translation.y = 3;
    }

    // Tabby::Shared<Tabby::Prefab> prefab = Tabby::CreateShared<Tabby::ExamplePrefab>();
    // prefab->Instantiate();

    // std::vector<uint8_t> data = Tabby::Prefab::SerializePrefab(prefab);
    // Tabby::Shared<Tabby::Prefab> deserializedPrefab = Tabby::Prefab::DeserializePrefab(data);
    // deserializedPrefab->Instantiate();

    // // Export Prefab
    // Tabby::Shared<Tabby::Prefab> createdPrefab = Tabby::CreateShared<Tabby::ExamplePrefab>(Tabby::UUID());
    // Tabby::Prefab::SerializePrefabToFile(createdPrefab, "prefabs/ExamplePrefab.tbpf");
    //
    // // Import Prefab
    // Tabby::AssetHandle exportedPrefabHande = Tabby::AssetManager::Get()->LoadAssetSource("prefabs/ExamplePrefab.tbpf", exportedPrefabHande);
    // Tabby::Shared<Tabby::Prefab> exportedPrefab = Tabby::AssetManager::Get()->GetAsset<Tabby::Prefab>(exportedPrefabHande);
    // exportedPrefab->Instantiate();
}

void Base::OnDetach()
{
    TB_PROFILE_SCOPE();

    Tabby::World::OnStop();
}

void Base::OnUpdate(Tabby::Timestep ts)
{

    // TB_INFO("{0}", fps);
    Tabby::World::OnViewportResize(m_ViewportSize.x, m_ViewportSize.y);

    if (Tabby::FramebufferSpecification spec = m_Framebuffer->GetSpecification();
        m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
        (spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y)) {
        m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
    }

    Tabby::Renderer2D::ResetStats();

    m_Framebuffer->Bind();
    {
        TB_PROFILE_SCOPE_NAME("Renderer Prep");
        Tabby::RenderCommand::SetClearColor({ 0.5f, 0.5f, 0.5f, 1 });
        Tabby::RenderCommand::Clear();
    }

    // BROKEN:
    // m_Framebuffer->ClearAttachment(1, -1);

    Tabby::World::Update(ts);
    OnOverlayRender();

    m_Framebuffer->Unbind();
    // tracy::QueueType::FrameImage(m_Framebuffer->GetColorAttachmentRendererID(), 320, 180, m_Framebuffer->GetColorAttachmentRendererID(), true);

    if (Tabby::Input::IsKeyPressed(Tabby::Key::Q))
        m_GizmoType = -1;
    if (Tabby::Input::IsKeyPressed(Tabby::Key::T))
        m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
    if (Tabby::Input::IsKeyPressed(Tabby::Key::S))
        m_GizmoType = ImGuizmo::OPERATION::SCALE;
    if (Tabby::Input::IsKeyPressed(Tabby::Key::R))
        m_GizmoType = ImGuizmo::OPERATION::ROTATE;

    fps = 1.0f / ts;
    // TB_INFO("FPS: {0} \n\t\tDeltaTime: {1}", fps, ts);
}

void Base::OnImGuiRender()
{
    TB_PROFILE_SCOPE();

    Tabby::World::DrawImGui();

    // Note: Switch this to true to enable dockspace
    static bool dockspaceOpen = false;
    // static bool dockspaceOpen = true;
    static bool opt_fullscreen_persistant = true;
    bool opt_fullscreen = opt_fullscreen_persistant;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen) {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
    ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // DockSpace
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    float minWinSizeX = style.WindowMinSize.x;
    style.WindowMinSize.x = 370.0f;
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    style.WindowMinSize.x = minWinSizeX;

    m_SceneHierarchyPanel.OnImGuiRender();
    m_PropertiesPanel.SetEntity(m_SceneHierarchyPanel.GetSelectedNode(), m_SceneHierarchyPanel.IsNodeSelected());
    m_PropertiesPanel.OnImGuiRender();

    ImGui::Begin("Stats");

    auto stats = Tabby::Renderer2D::GetStats();
    ImGui::Text("Renderer2D Stats:");
    ImGui::Text("Draw Calls: %d", stats.DrawCalls);
    ImGui::Text("Quads: %d", stats.QuadCount);
    ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
    ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

    ImGui::End();

    ImGui::Begin("Settings");

    ImGui::Text("FPS: %.2f", fps);
    ImGui::DragFloat2("Size", glm::value_ptr(m_ViewportSize));
    ImGui::Checkbox("Show physics colliders", &m_ShowPhysicsColliders);

    // ImGui::Image((ImTextureID)s_Font->GetAtlasTexture()->GetRendererID(), { 512, 512 }, { 0, 1 }, { 1, 0 });

    if (ImGui::Button("File Dialog Open(test)")) {
        TB_INFO("{0}", Tabby::FileDialogs::OpenFile(".png"));
    }
    if (ImGui::Button("File Dialog Save(test)")) {
        TB_INFO("{0}", Tabby::FileDialogs::SaveFile(".png"));
    }

    ImGui::End();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2 { 0, 0 });
    ImGui::Begin("Viewport");
    auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
    auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
    auto viewportOffset = ImGui::GetWindowPos();
    m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
    m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

    m_ViewportFocused = ImGui::IsWindowFocused();
    m_ViewportHovered = ImGui::IsWindowHovered();

    Tabby::Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportHovered);

    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

    uint64_t textureID = m_Framebuffer->GetColorAttachmentRendererID(0);
    ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2 { m_ViewportSize.x, m_ViewportSize.y }, ImVec2 { 0, 1 }, ImVec2 { 1, 0 });

    // if (ImGui::BeginDragDropTarget()) {
    //     if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
    //         const wchar_t* path = (const wchar_t*)payload->Data;
    //         OpenScene(path);
    //     }
    //     ImGui::EndDragDropTarget();
    // }

    // Gizmos
    Tabby::Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedNode();
    if (selectedEntity && m_GizmoType != -1) {
        ImGuizmo::SetOrthographic(true);
        ImGuizmo::SetDrawlist();

        ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

        // Camera

        // Runtime camera from entity
        auto cameraEntity = Tabby::World::GetPrimaryCameraEntity();
        const auto& camera = cameraEntity.GetComponent<Tabby::CameraComponent>().Camera;
        const glm::mat4& cameraProjection = camera.GetProjection();
        glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<Tabby::TransformComponent>().GetTransform());

        // Entity transform
        auto& tc = selectedEntity.GetComponent<Tabby::TransformComponent>();
        glm::mat4 transform = tc.GetTransform();

        // Snapping
        bool snap = Tabby::Input::IsKeyPressed(Tabby::Key::LeftControl);
        float snapValue = 0.5f; // Snap to 0.5m for translation/scale
        // Snap to 45 degrees for rotation
        if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
            snapValue = 45.0f;

        float snapValues[3] = { snapValue, snapValue, snapValue };

        ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
            (ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
            nullptr, snap ? snapValues : nullptr);

        if (ImGuizmo::IsUsing()) {
            Tabby::Vector3 translation, rotation, scale;
            Tabby::Math::DecomposeTransform(transform, (glm::vec3&)translation, (glm::vec3&)rotation, (glm::vec3&)scale);

            Tabby::Vector3 deltaRotation = rotation - tc.Rotation;
            tc.Translation = translation;
            tc.Rotation += deltaRotation;
            tc.Scale = scale;
        }
    }

    ImGui::End();
    ImGui::PopStyleVar();

    ImGui::End();
}
void Base::OnOverlayRender()
{
    Tabby::Entity camera = Tabby::World::GetPrimaryCameraEntity();
    if (!camera)
        return;

    Tabby::Renderer2D::BeginScene(camera.GetComponent<Tabby::CameraComponent>().Camera, camera.GetComponent<Tabby::TransformComponent>().GetTransform());

    if (m_ShowPhysicsColliders) {
        // Box Colliders
        {
            auto view = Tabby::World::GetAllEntitiesWith<Tabby::TransformComponent, Tabby::BoxCollider2DComponent>();
            for (auto entity : view) {
                auto [tc, bc2d] = view.get<Tabby::TransformComponent, Tabby::BoxCollider2DComponent>(entity);

                glm::vec3 translation = tc.Translation + glm::vec3(bc2d.Offset, 0.001f);
                glm::vec3 scale = glm::vec3(bc2d.Size * 2.0f, 1.0f);

                glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
                    * glm::rotate(glm::mat4(1.0f), Tabby::Math::DEG2RAD * tc.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f))
                    * glm::translate(glm::mat4(1.0f), glm::vec3(bc2d.Offset, 0.001f))
                    * glm::scale(glm::mat4(1.0f), scale);

                Tabby::Renderer2D::DrawRect(transform, glm::vec4(0, 1, 0, 1));
            }
        }

        // Circle Colliders
        {
            auto view = Tabby::World::GetAllEntitiesWith<Tabby::TransformComponent, Tabby::CircleCollider2DComponent>();
            for (auto entity : view) {
                auto [tc, cc2d] = view.get<Tabby::TransformComponent, Tabby::CircleCollider2DComponent>(entity);

                glm::vec3 translation = tc.Translation + glm::vec3(cc2d.Offset, 0.001f);
                glm::vec3 scale = (glm::vec3&)tc.Scale * glm::vec3(cc2d.Radius * 2.0f);

                glm::mat4 transform = glm::translate(glm::mat4(1.0f), (glm::vec3&)tc.Translation)
                    * glm::rotate(glm::mat4(1.0f), Tabby::Math::DEG2RAD * tc.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f))
                    * glm::translate(glm::mat4(1.0f), glm::vec3(cc2d.Offset, 0.001f))
                    * glm::scale(glm::mat4(1.0f), scale);

                Tabby::Renderer2D::DrawCircle(transform, glm::vec4(0, 1, 0, 1), 0.07f);
            }
        }

        // Capsule Colliders
        {
            auto view = Tabby::World::GetAllEntitiesWith<Tabby::TransformComponent, Tabby::CapsuleCollider2DComponent>();
            for (auto entity : view) {
                auto [tc, cc2d] = view.get<Tabby::TransformComponent, Tabby::CapsuleCollider2DComponent>(entity);

                glm::vec3 translation1 = tc.Translation + glm::vec3(cc2d.center1, 0.001f);
                glm::vec3 scale1 = (glm::vec3&)tc.Scale * glm::vec3(cc2d.Radius * 2.0f);
                glm::mat4 transform1 = glm::translate(glm::mat4(1.0f), (glm::vec3&)tc.Translation)
                    * glm::rotate(glm::mat4(1.0f), Tabby::Math::DEG2RAD * tc.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f))
                    * glm::translate(glm::mat4(1.0f), glm::vec3(cc2d.center1, 0.001f))
                    * glm::scale(glm::mat4(1.0f), scale1);
                Tabby::Renderer2D::DrawCircle(transform1, glm::vec4(0, 1, 0, 1), 0.1f);

                glm::vec3 translation2 = tc.Translation + glm::vec3(cc2d.center2, 0.001f);
                glm::vec3 scale2 = (glm::vec3&)tc.Scale * glm::vec3(cc2d.Radius * 2.0f);
                glm::mat4 transform2 = glm::translate(glm::mat4(1.0f), (glm::vec3&)tc.Translation)
                    * glm::rotate(glm::mat4(1.0f), Tabby::Math::DEG2RAD * tc.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f))
                    * glm::translate(glm::mat4(1.0f), glm::vec3(cc2d.center2, 0.001f))
                    * glm::scale(glm::mat4(1.0f), scale2);
                Tabby::Renderer2D::DrawCircle(transform2, glm::vec4(0, 1, 0, 1), 0.1f);

                glm::vec3 translation3 = tc.Translation + glm::vec3(cc2d.center2, 0.001f);
                glm::vec3 scale3 = (glm::vec3&)tc.Scale * glm::vec3(cc2d.Radius * 2.0f) * glm::vec3(0.95f, 1.5f, 0.95f);
                glm::mat4 transform3 = glm::translate(glm::mat4(1.0f), (glm::vec3&)tc.Translation)
                    * glm::rotate(glm::mat4(1.0f), Tabby::Math::DEG2RAD * tc.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f))
                    * glm::scale(glm::mat4(1.0f), scale3);
                Tabby::Renderer2D::DrawRect(transform3, glm::vec4(0, 1, 0, 1));
            }
        }

        // Segment Colliders
        {
            auto view = Tabby::World::GetAllEntitiesWith<Tabby::TransformComponent, Tabby::SegmentCollider2DComponent>();
            for (auto entity : view) {
                auto [tc, sc2d] = view.get<Tabby::TransformComponent, Tabby::SegmentCollider2DComponent>(entity);

                glm::vec3 point1 = tc.Translation + glm::vec3(sc2d.point1, 0.001f);
                glm::vec3 point2 = tc.Translation + glm::vec3(sc2d.point2, 0.001f);

                // Apply rotation to both endpoints
                glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), Tabby::Math::DEG2RAD * tc.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

                point1 = tc.Translation + glm::vec3(rotationMatrix * glm::vec4(sc2d.point1, 0.0f, 0.0f));
                point2 = tc.Translation + glm::vec3(rotationMatrix * glm::vec4(sc2d.point2, 0.0f, 0.0f));

                // Draw the line
                Tabby::Renderer2D::DrawLine(point1, point2, glm::vec4(0, 1, 0, 1));
            }
        }
    }

    // Draw selected entity outline
    if (Tabby::Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedNode()) {
        const Tabby::TransformComponent& transform = selectedEntity.GetComponent<Tabby::TransformComponent>();
        Tabby::Renderer2D::DrawRect(transform.GetTransform(), glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));
    }

    Tabby::Renderer2D::EndScene();
}

void Base::OnEvent(Tabby::Event& e)
{
    Tabby::EventDispatcher dispatcher(e);
    // dispatcher.Dispatch<Tabby::KeyPressedEvent>(TB_BIND_EVENT_FN(Base::OnKeyPressed));
    // dispatcher.Dispatch<Tabby::MouseButtonPressedEvent>(TB_BIND_EVENT_FN(Base::OnMouseButtonPressed));
}
