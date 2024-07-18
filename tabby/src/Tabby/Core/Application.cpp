#include "Tabby/Renderer/Renderer.h"
#include <Tabby/Core/Input/Input.h>

#include "Tabby/Utils/PlatformUtils.h"
#include <Tabby/Asset/AssetManager.h>
#include <Tabby/Audio/AudioEngine.h>
#include <Tabby/Core/Filesystem/Filesystem.h>

namespace Tabby {

Application* Application::s_Instance = nullptr;

Application::Application(const ApplicationSpecification& specification)
    : m_Specification(specification)
{
    TB_PROFILE_SCOPE();

    TB_CORE_ASSERT_TAGGED(!s_Instance, "Application already exists!");
    s_Instance = this;

#ifndef TB_PLATFORM_WEB
    if (!m_Specification.WorkingDirectory.empty()) {
        FileSystem::SetWorkingDirectory(m_Specification.WorkingDirectory);
    }
#endif

    m_Window = Window::Create(WindowProps(m_Specification.Name));
    m_Window->SetEventCallback(TB_BIND_EVENT_FN(Application::OnEvent));

    AssetManager::Init();
    TB_CORE_INFO("1");
    Audio::Engine::Init();
    TB_CORE_INFO("2");
    Renderer::Init();
    Input::Init();

    m_ImGuiLayer = new ImGuiLayer();
    PushOverlay(m_ImGuiLayer);
}

Application::~Application()
{
    TB_PROFILE_SCOPE();
    Audio::Engine::Shutdown();
    Renderer::Shutdown();

    AssetManager::Get()->FullUnload();
    AssetManager::Shutdown();
}

void Application::PushLayer(Layer* layer)
{
    TB_PROFILE_SCOPE();

    m_LayerStack.PushLayer(layer);
    layer->OnAttach();
}

void Application::PushOverlay(Layer* layer)
{
    TB_PROFILE_SCOPE();

    m_LayerStack.PushOverlay(layer);
    layer->OnAttach();
}

void Application::Close()
{
    m_Running = false;
}

void Application::SubmitToMainThread(const std::function<void()>& function)
{
    std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);

    m_MainThreadQueue.emplace_back(function);
}

void Application::OnEvent(Event& e)
{
    TB_PROFILE_SCOPE();

    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowCloseEvent>(TB_BIND_EVENT_FN(Application::OnWindowClose));
    dispatcher.Dispatch<WindowResizeEvent>(TB_BIND_EVENT_FN(Application::OnWindowResize));

    for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it) {
        if (e.Handled)
            break;
        (*it)->OnEvent(e);
    }
}

void Application::Run()
{

    while (m_Running) {
        TB_PROFILE_FRAME("Application");

        double time = Time::GetTime();
        Time::SetDeltaTime(time - m_LastFrameTime);
        m_LastFrameTime = time;

        ExecuteMainThreadQueue();

        if (!m_Minimized && Time::GetDeltaTime() < 200.0f) { // ts < 200  because ts is really high number in first frame
            // and that breaks the phyiscs and some other stuff.
            // this happens because m_LastFrameTime is 0 in first frame.
            {
                TB_PROFILE_SCOPE_NAME("LayerStack OnUpdate");

                // TODO: Remove TimeStep
                for (Layer* layer : m_LayerStack)
                    layer->OnUpdate(Time::GetDeltaTime());
            }

            m_ImGuiLayer->Begin();
            {
                TB_PROFILE_SCOPE_NAME("LayerStack OnImGuiRender");

                for (Layer* layer : m_LayerStack)
                    layer->OnImGuiRender();
            }
            m_ImGuiLayer->End();
        }

        m_Window->OnUpdate();

        // Framerate limiter. this will do nothing if maxFPS is 0.
        if (m_Specification.maxFPS > 0.0) {
            double frameTime = Time::GetTime() - time;

            double frameTimeLimit = 1.0 / m_Specification.maxFPS;
            if (frameTime < frameTimeLimit) {
                double sleepTime = frameTimeLimit - frameTime;
                std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));
            }
        }
    }
}

bool Application::OnWindowClose(WindowCloseEvent& e)
{
    m_Running = false;
    return true;
}

bool Application::OnWindowResize(WindowResizeEvent& e)
{
    TB_PROFILE_SCOPE();

    if (e.GetWidth() == 0 || e.GetHeight() == 0) {
        m_Minimized = true;
        return false;
    }

    m_Minimized = false;
    Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

    return false;
}

void Application::ExecuteMainThreadQueue()
{
    std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);

    for (auto& func : m_MainThreadQueue)
        func();

    m_MainThreadQueue.clear();
}

}
