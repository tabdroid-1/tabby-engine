#include <tbpch.h>

#include "Application.h"
#include "raylib.h"
#include "rlImGui/rlImGui.h"
#include <GLFW/glfw3.h>
#include <filesystem>

namespace Tabby {

Application* Application::s_Instance = nullptr;

Application::Application(const ApplicationSpecification& specification)
    : m_Specification(specification)
{

    TB_ASSERT(!s_Instance, "Application already exists!");
    s_Instance = this;

    if (!m_Specification.WorkingDirectory.empty())
        std::filesystem::current_path(m_Specification.WorkingDirectory);

    int monitor = GetCurrentMonitor();
    InitWindow(GetMonitorWidth(monitor), GetMonitorHeight(monitor), m_Specification.Name.c_str());

    frameBuffer = LoadRenderTexture(GetMonitorWidth(GetCurrentMonitor()), GetMonitorHeight(GetCurrentMonitor()));

    int maxHeight = GetMonitorHeight(monitor) - 40;
    if (GetScreenHeight() > maxHeight)
        SetWindowSize(GetScreenWidth(), maxHeight);

    rlImGuiSetup(true);

    SetExitKey(0);
}

Application::~Application()
{
    rlImGuiShutdown();
}

void Application::Run()
{

    while (IsRunning()) {

        sceneManager.Update();
        sceneManager.LateUpdate();

        BeginTextureMode(frameBuffer);
        // textures and stuff goes here
        ClearBackground(BLACK);
        sceneManager.Draw();
        EndTextureMode();

        BeginDrawing();
        DrawTexturePro(frameBuffer.texture, { 0, 0, (float)frameBuffer.texture.width, -(float)frameBuffer.texture.height },
            { 0, 0, (float)GetMonitorWidth(GetCurrentMonitor()), (float)GetMonitorHeight(GetCurrentMonitor()) }, { 0, 0 }, 0, WHITE);

        sceneManager.DrawHud();

        rlImGuiBegin();

        sceneManager.DrawImGui();

        rlImGuiEnd();

        EndDrawing();
    }
}

}
