#pragma once
#include <tbpch.h>
#include <Tabby/Core/Events/ApplicationEvent.h>
#include <Tabby/Core/Events/MouseEvent.h>
#include <Tabby/Core/Layer/LayerStack.h>
#include <Tabby/UI/ImGui/ImGuiLayer.h>
#include <Tabby/Core/Events/Event.h>
#include <Tabby/Core/Window.h>

int main(int argc, char** argv);

namespace Tabby {

struct ApplicationCommandLineArgs {
    int Count = 0;
    char** Args = nullptr;

    const char* operator[](int index) const
    {
        TB_CORE_ASSERT(index < Count);
        return Args[index];
    }
};

struct ApplicationSpecification {
    enum class RendererAPI {
        None = 0,
        OpenGL46 = 1,
        OpenGL33 = 2,
        OpenGLES3 = 3
    };
    std::string Name = "Tabby Application";
    std::string WorkingDirectory = "assets";
    bool Resizeable = true;
    bool VSync = true;
    uint32_t Width = 1600;
    uint32_t Height = 900;
    uint32_t MinWidth = 160;
    uint32_t MinHeight = 90;
    float MaxFPS = 0.0f;
    uint32_t FixedUpdateRate = 60;
    RendererAPI RendererAPI = RendererAPI::OpenGL33;
    uint8_t MaxGamepads = 4;
    uint8_t MaxGamepadAxis = 8;
    ApplicationCommandLineArgs CommandLineArgs;
};

class Application {
public:
    Application(const ApplicationSpecification& specification);
    virtual ~Application();

    void OnEvent(Event& e);

    static void PushLayer(Layer* layer);
    static void PushOverlay(Layer* layer);

    static Window& GetWindow() { return *s_Instance->m_Window; }
    static ImGuiLayer& GetImGuiLayer() { return *s_Instance->m_ImGuiLayer; }
    static const ApplicationSpecification& GetSpecification() { return s_Instance->m_Specification; }

    static void SubmitToMainThread(const std::function<void()>& function);
    static void Run();
    static void Close();

private:
    bool OnWindowClose(WindowCloseEvent& e);
    bool OnWindowResize(WindowResizeEvent& e);
    bool OnMouseScroll(MouseScrolledEvent& e);

    void ExecuteMainThreadQueue();

private:
    inline static ApplicationSpecification m_Specification;
    inline static Scope<Window> m_Window;
    inline static ImGuiLayer* m_ImGuiLayer;
    inline static LayerStack m_LayerStack;

    inline static double m_LastFrameTime = 0.0f;
    inline static bool m_Minimized = false;
    inline static bool m_Running = true;

    inline static std::vector<std::function<void()>> m_MainThreadQueue;
    inline static std::mutex m_MainThreadQueueMutex;

private:
    inline static Application* s_Instance = nullptr;
    friend int ::main(int argc, char** argv);
};

// To be defined in CLIENT
Application* CreateApplication(ApplicationCommandLineArgs args);
}
