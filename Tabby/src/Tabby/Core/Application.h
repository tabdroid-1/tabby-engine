#pragma once

#include "Tabby/Core/Base.h"

#include "Tabby/Core/LayerStack.h"
#include "Tabby/Core/Window.h"
#include "Tabby/Events/ApplicationEvent.h"
#include "Tabby/Events/Event.h"

#include "Tabby/Core/Timestep.h"

#include "Tabby/ImGui/ImGuiLayer.h"

int main(int argc, char** argv);

namespace Tabby {

class Application {
public:
    Application(const std::string& name = "Tabby App");
    virtual ~Application();

    void OnEvent(Event& e);

    void PushLayer(Layer* layer);
    void PushOverlay(Layer* layer);

    Window& GetWindow() { return *m_Window; }

    void Close();

    ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

    static Application& Get() { return *s_Instance; }

private:
    void Run();
    bool OnWindowClose(WindowCloseEvent& e);
    bool OnWindowResize(WindowResizeEvent& e);

private:
    std::unique_ptr<Window> m_Window;
    ImGuiLayer* m_ImGuiLayer;
    bool m_Running = true;
    bool m_Minimized = false;
    LayerStack m_LayerStack;
    float m_LastFrameTime = 0.0f;

private:
    static Application* s_Instance;
    friend int ::main(int argc, char** argv);
};

// To be defined in CLIENT
Application* CreateApplication();

}
