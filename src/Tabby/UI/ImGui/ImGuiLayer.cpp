#include <tbpch.h>
#include <Tabby/Renderer/RendererAPI.h>
#include <Tabby/UI/ImGui/ImGuiLayer.h>
#include <Tabby/Core/Application.h>

#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl2.h>
#include <imgui_internal.h>
#include <imgui.h>
#include <SDL.h>
#include <gl.h>
#include <ImGuizmo.h>

namespace Tabby {

ImGuiLayer::ImGuiLayer()
    : Layer("ImGuiLayer")
{
    TB_PROFILE_SCOPE_NAME("Tabby::ImGuiLayer::Constructor");
}

void ImGuiLayer::OnAttach()
{
    TB_PROFILE_SCOPE_NAME("Tabby::ImGuiLayer::OnAttach");

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

    // float fontSize = 18.0f; // *2.0f;
    // io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Bold.ttf", fontSize);
    // io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Regular.ttf", fontSize);

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    SetDarkThemeColors();

    SDL_Window* window = static_cast<SDL_Window*>(Application::GetWindow().GetNativeWindow());

    // Setup Platform/Renderer bindings
    ImGui_ImplSDL2_InitForOpenGL(window, SDL_GL_GetCurrentContext());
    if (RendererAPI::GetAPI() == RendererAPI::API::OpenGL46)
        ImGui_ImplOpenGL3_Init("#version 460 core");
    else if (RendererAPI::GetAPI() == RendererAPI::API::OpenGL33)
        ImGui_ImplOpenGL3_Init("#version 330 core");
    else if (RendererAPI::GetAPI() == RendererAPI::API::OpenGLES3)
        ImGui_ImplOpenGL3_Init("#version 300 es");
}

void ImGuiLayer::OnDetach()
{
    TB_PROFILE_SCOPE_NAME("Tabby::ImGuiLayer::OnDetach");

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiLayer::OnEvent(Event& e)
{
    TB_PROFILE_SCOPE_NAME("Tabby::ImGuiLayer::OnEvent");

    if (m_BlockEvents) {
        ImGuiIO& io = ImGui::GetIO();
        e.Handled |= e.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
        e.Handled |= e.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
    }
}

void ImGuiLayer::Begin()
{
    TB_PROFILE_SCOPE_NAME("Tabby::ImGuiLayer::Begin");

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
}

void ImGuiLayer::End()
{
    TB_PROFILE_SCOPE_NAME("Tabby::ImGuiLayer::End");

    ImGuiIO& io = ImGui::GetIO();
    // Application& app = Application::Get();
    io.DisplaySize = ImVec2((float)Application::GetWindow().GetWidth(), (float)Application::GetWindow().GetHeight());

    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();

        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        SDL_GL_MakeCurrent(static_cast<SDL_Window*>(Application::GetWindow().GetNativeWindow()), backup_current_context);
    }
}

void ImGuiLayer::SetDarkThemeColors()
{
    TB_PROFILE_SCOPE_NAME("Tabby::ImGuiLayer::SetDarkThemeColors");

    auto& colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4 { 0.1f, 0.105f, 0.11f, 1.0f };

    // Headers
    colors[ImGuiCol_Header] = ImVec4 { 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_HeaderHovered] = ImVec4 { 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_HeaderActive] = ImVec4 { 0.15f, 0.1505f, 0.151f, 1.0f };

    // Buttons
    colors[ImGuiCol_Button] = ImVec4 { 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_ButtonHovered] = ImVec4 { 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_ButtonActive] = ImVec4 { 0.15f, 0.1505f, 0.151f, 1.0f };

    // Frame BG
    colors[ImGuiCol_FrameBg] = ImVec4 { 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_FrameBgHovered] = ImVec4 { 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_FrameBgActive] = ImVec4 { 0.15f, 0.1505f, 0.151f, 1.0f };

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4 { 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TabHovered] = ImVec4 { 0.38f, 0.3805f, 0.381f, 1.0f };
    colors[ImGuiCol_TabActive] = ImVec4 { 0.28f, 0.2805f, 0.281f, 1.0f };
    colors[ImGuiCol_TabUnfocused] = ImVec4 { 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4 { 0.2f, 0.205f, 0.21f, 1.0f };

    // Title
    colors[ImGuiCol_TitleBg] = ImVec4 { 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TitleBgActive] = ImVec4 { 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4 { 0.15f, 0.1505f, 0.151f, 1.0f };
}

uint32_t ImGuiLayer::GetActiveWidgetID() const
{
    TB_PROFILE_SCOPE_NAME("Tabby::ImGuiLayer::GetActiveWidgetID");

    return GImGui->ActiveId;
}

}
