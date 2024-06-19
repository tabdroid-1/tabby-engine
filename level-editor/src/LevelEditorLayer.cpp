#include <LevelEditorLayer.h>
#include <Prefab/ExamplePrefab.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>

namespace Tabby {

float fps = 0;

LevelEditorLayer::LevelEditorLayer()
    : Layer("LevelEditorLayer")
{
}

void LevelEditorLayer::OnAttach()
{
    TB_PROFILE_SCOPE();

    Tabby::World::Init();
    Tabby::Application::Get().GetWindow().SetVSync(false);

    Tabby::FramebufferSpecification fbSpec;
    fbSpec.Attachments = { Tabby::FramebufferTextureFormat::RGBA8, Tabby::FramebufferTextureFormat::RED_INTEGER, Tabby::FramebufferTextureFormat::DEPTH24STENCIL8 };
    fbSpec.Width = 2560;
    fbSpec.Height = 1600;
    m_Framebuffer = Tabby::Framebuffer::Create(fbSpec);

    m_Camera.SetOrthographic(10, -1, 1000);
    m_Canvas = CreateScope<Canvas>(m_Framebuffer, m_ViewportSize, m_ViewportBounds);
    m_LevelEditorLogic = CreateScope<LevelEditorLogic>(&m_Camera, &m_CameraTransform);
    m_ContentBrowserPanel = CreateScope<ContentBrowserPanel>();

    m_LevelEditorLogic->OnAttach();
}

void LevelEditorLayer::OnDetach()
{
    TB_PROFILE_SCOPE();
}

void LevelEditorLayer::OnUpdate(Timestep ts)
{

    m_Camera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);

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

    // m_Framebuffer->ClearAttachment(1, -1);

    Renderer2D::BeginScene(m_Camera, m_CameraTransform.GetTransform());

    if (m_Canvas->ShouldShowGameView())
        m_LevelEditorLogic->Update(ts);
    m_Canvas->Update(ts);

    Renderer2D::EndScene();

    m_Framebuffer->Unbind();
    fps = 1.0f / ts;
}

void LevelEditorLayer::OnImGuiRender()
{
    TB_PROFILE_SCOPE();

    // Note: Switch this to true to enable dockspace
    static bool dockspaceOpen = true;
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

    ImGui::BeginMainMenuBar();
    if (ImGui::MenuItem("File")) {
        ImGui::OpenPopup("menu_bar_file");
    };
    if (ImGui::MenuItem("View")) {
        ImGui::OpenPopup("menu_bar_view");
    };
    // if (ImGui::BeginPopup("menu_bar_file")) {
    //     // if (ImGui::MenuItem("Open project", "Ctrl + O")) {
    //     //     LoadProject();
    //     // };
    //     // if (ImGui::MenuItem("Save project", "Ctrl + S")) {
    //     //     SaveProject();
    //     // };
    //     // if (ImGui::MenuItem("New project", "Ctrl + N")) {
    //     //     NewProject();
    //     // };
    //     // ImGui::EndPopup();
    // };
    if (ImGui::BeginPopup("menu_bar_view")) {
        // if (ImGui::MenuItem("Scene hierarchy")) {
        //     m_HierarchyPanel->Open(true);
        // }
        // if (ImGui::MenuItem("Properties")) {
        //     m_PropertiesPanel->Open(true);
        // }
        if (ImGui::MenuItem("Content browser")) {
            if (m_ContentBrowserPanel->IsOpen())
                m_ContentBrowserPanel->Open(false);
            else
                m_ContentBrowserPanel->Open(true);
        }
        ImGui::EndPopup();
    }
    ImGui::EndMainMenuBar();

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
    ImGui::Text("Viewport Size: %f, %f", m_ViewportSize.x, m_ViewportSize.y);

    float orthoSize = m_Camera.GetOrthographicSize();

    ImGui::Text("Camera Settings:");
    ImGui::DragFloat("Size", &orthoSize);
    m_Camera.SetOrthographicSize(orthoSize);

    ImGui::DragFloat3("Camera Position", glm::value_ptr((glm::vec3&)m_CameraTransform.Translation));
    ImGui::DragFloat3("Camera Rotation", glm::value_ptr((glm::vec3&)m_CameraTransform.Rotation));
    ImGui::DragFloat3("Camera Scale", glm::value_ptr((glm::vec3&)m_CameraTransform.Scale));

    ImGui::End();

    m_ContentBrowserPanel->OnImGuiRender();
    m_LevelEditorLogic->ImGuiRender();
    m_Canvas->ImGuiRender();

    ImGui::End();
}

void LevelEditorLayer::OnEvent(Tabby::Event& e)
{
    Tabby::EventDispatcher dispatcher(e);
    // dispatcher.Dispatch<Tabby::KeyPressedEvent>(TB_BIND_EVENT_FN(LevelEditorLayer::OnKeyPressed));
    // dispatcher.Dispatch<Tabby::MouseButtonPressedEvent>(TB_BIND_EVENT_FN(LevelEditorLayer::OnMouseButtonPressed));

    m_LevelEditorLogic->OnEvent(e);
}

}
