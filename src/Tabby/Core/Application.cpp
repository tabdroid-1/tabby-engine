#include <Tabby/Core/Filesystem/Filesystem.h>
#include <Tabby/Asset/AssetManager.h>
#include <Tabby/Renderer/Renderer.h>
#include <Tabby/Audio/AudioEngine.h>
#include <Tabby/Core/Application.h>
#include <Tabby/Core/Input/Input.h>
#include <Tabby/Core/Time/Time.h>

#include <SDL_rwops.h>
#include <bgfx/bgfx.h>

namespace Tabby {

Application::Application(const ApplicationSpecification& specification)
{
    TB_PROFILE_SCOPE_NAME("Tabby::Application::Constructor");

    TB_CORE_ASSERT_TAGGED(!s_Instance, "Application already exists!");
    s_Instance = this;
    m_Specification = specification;

    if (!m_Specification.WorkingDirectory.empty()) {
        FileSystem::SetWorkingDirectory(m_Specification.WorkingDirectory);
    }

    ParseEngineConfig();

    m_Window = Window::Create(WindowProps(m_Specification.Name, m_Specification.Width, m_Specification.Height, m_Specification.MinWidth, m_Specification.MinHeight, m_Specification.FullscreenMode, m_Specification.Resizable, m_Specification.VSync));
    m_Window->SetEventCallback(TB_BIND_EVENT_FN(Application::OnEvent));

    Renderer::Init();

    m_ImGuiRenderer = new ImGuiRenderer;
    m_ImGuiRenderer->Launch(m_Window->GetNativeWindow());
}

Application::~Application()
{
    TB_PROFILE_SCOPE_NAME("Tabby::Application::Destructor");

    m_LayerStack.Destroy();

    Renderer::Shutdown();
    // AudioEngine::Shutdown();
    AssetManager::FullUnload();
}

void Application::PushLayer(Layer* layer)
{
    TB_PROFILE_SCOPE_NAME("Tabby::Application::PushLayer");

    s_Instance->m_LayerStack.PushLayer(layer);
    layer->OnAttach();
}

void Application::PushOverlay(Layer* layer)
{
    TB_PROFILE_SCOPE_NAME("Tabby::Application::PushOverlay");

    s_Instance->m_LayerStack.PushOverlay(layer);
    layer->OnAttach();
}

void Application::Close()
{
    TB_PROFILE_SCOPE_NAME("Tabby::Application::Close");

    s_Instance->m_Running = false;
}

void Application::SubmitToMainThread(const std::function<void()>& function)
{
    TB_PROFILE_SCOPE_NAME("Tabby::Application::SubmitToMainThread");

    std::scoped_lock<std::mutex> lock(s_Instance->m_MainThreadQueueMutex);

    s_Instance->m_MainThreadQueue.emplace_back(function);
}

void Application::OnEvent(Event& e)
{
    TB_PROFILE_SCOPE_NAME("Tabby::Application::OnEvent");

    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowCloseEvent>(TB_BIND_EVENT_FN(Application::OnWindowClose));
    dispatcher.Dispatch<WindowResizeEvent>(TB_BIND_EVENT_FN(Application::OnWindowResize));
    dispatcher.Dispatch<MouseScrolledEvent>(TB_BIND_EVENT_FN(Application::OnMouseScroll));

    for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it) {
        if (e.Handled)
            break;
        (*it)->OnEvent(e);
    }
}

void Application::Run()
{
#ifdef TB_PLATFORM_WEB
    if (m_Running) {
#else
    while (s_Instance->m_Running) {
#endif
        TB_PROFILE_FRAME();
        TB_PROFILE_SCOPE_NAME("Tabby::Application::Update");

        double time = Time::GetTime();
        Time::SetDeltaTime(time - s_Instance->m_LastFrameTime);
        s_Instance->m_LastFrameTime = time;

        Input::Update();

        s_Instance->ExecuteMainThreadQueue();

        if (!s_Instance->m_Minimized && Time::GetDeltaTime() < 200.0f) { // ts < 200  because ts is really high number in first frame
                                                                         // and that breaks the phyiscs and some other stuff.
                                                                         // this happens because m_LastFrameTime is 0 in first frame.

            bgfx::touch(0);
            {
                TB_PROFILE_SCOPE_NAME("Tabby::Application::LayerStackUpdate");

                for (Layer* layer : s_Instance->m_LayerStack)
                    layer->OnUpdate();
            }

            s_Instance->m_ImGuiRenderer->BeginFrame();
            {
                TB_PROFILE_SCOPE_NAME("Tabby::Application::LayerStackOnImGuiRender");

                for (Layer* layer : s_Instance->m_LayerStack)
                    layer->OnImGuiRender();
            }

            s_Instance->m_ImGuiRenderer->EndFrame();

            Renderer::Frame();
        }

        Input::s_Instance->m_MouseScrollDelta = { 0.0f, 0.0f };
        s_Instance->m_Window->OnUpdate();
        ProcessApplicationSpec();

        // Framerate limiter. this will do nothing if maxFPS is 0 or less.
        if (s_Instance->m_Specification.MaxFPS > 0.0) {
            TB_PROFILE_SCOPE_NAME("Tabby::Application::FramerateLimiter");
            double frameTime = Time::GetTime() - time;

            double frameTimeLimit = 1.0 / s_Instance->m_Specification.MaxFPS;
            if (frameTime < frameTimeLimit) {
                double sleepTime = frameTimeLimit - frameTime;
                std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));
            }
        }
    }
}

void Application::ProcessApplicationSpec()
{
    if (GetWindow().IsVSync() != GetSpecification().VSync) {
        GetWindow().SetVSync(GetSpecification().VSync);
    }

    if (GetWindow().IsResizable() != GetSpecification().Resizable) {
        GetWindow().SetResizable(GetSpecification().Resizable);
    }

    // This is anoying
    // if (GetWindow().GetFullscreenMode() != GetSpecification().FullscreenMode) {
    //     GetWindow().SetFullscreen(GetSpecification().FullscreenMode);
    // }
}

bool Application::OnWindowClose(WindowCloseEvent& e)
{
    TB_PROFILE_SCOPE_NAME("Tabby::Application::OnWindowClose");

    m_Running = false;
    return true;
}

bool Application::OnWindowResize(WindowResizeEvent& e)
{
    TB_PROFILE_SCOPE_NAME("Tabby::Application::OnWindowResize");

    if (e.GetWidth() == 0 || e.GetHeight() == 0) {
        m_Minimized = true;
        return false;
    }

    m_Minimized = false;
    // Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

    return false;
}

bool Application::OnMouseScroll(MouseScrolledEvent& e)
{
    TB_PROFILE_SCOPE_NAME("Tabby::Application::OnMouseScroll");

    Input::s_Instance->m_MouseScrollDelta = { e.GetXOffset(), e.GetYOffset() };
    return false;
}

void Application::ExecuteMainThreadQueue()
{
    TB_PROFILE_SCOPE_NAME("Tabby::Application::ExecuteMainThreadQueue");

    std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);

    for (auto& func : m_MainThreadQueue)
        func();

    m_MainThreadQueue.clear();
}

void Application::ParseEngineConfig()
{
    std::vector<std::string> lines;
    SDL_RWops* rw = SDL_RWFromFile("tbconfig", "rb");
    if (rw != nullptr) {
        Sint64 size = SDL_RWsize(rw);

        uint32_t offset = 0;
        while (offset < size) {

            std::string line;
            char c;
            do {
                c = SDL_ReadU8(rw);
                if (c != '\n' && c != '\000')
                    line.push_back(c);
                offset++;
            } while (c != '\n' && c != '\000');

            lines.push_back(line);
        }

        SDL_RWclose(rw);
    } else {
        TB_CORE_ERROR("Could not find tbconfig file");
    }

    for (auto line : lines) {
        {
            size_t a = line.find("graphics_api");
            if (a != line.npos) {
                std::string option;
                size_t offset = a + 12 + 1; // 12 is the size of 'graphics_api' and 1 is '='
                option = line.substr(offset, line.size());

                /*if (option == "gles3") {*/
                /*    Renderer::s_API = Renderer::API::OpenGLES3;*/
                /*    GetSpecification().RendererAPI = ApplicationSpecification::RendererAPI::OpenGLES3;*/
                /**/
                /*} else if (option == "gl33") {*/
                /*    Renderer::s_API = Renderer::API::OpenGL33;*/
                /*    GetSpecification().RendererAPI = ApplicationSpecification::RendererAPI::OpenGL33;*/
                /**/
                /*} else if (option == "gl46") {*/
                /*    Renderer::s_API = Renderer::API::OpenGL46;*/
                /*    GetSpecification().RendererAPI = ApplicationSpecification::RendererAPI::OpenGL46;*/
                /**/
                /*} else*/
                /*    TB_CORE_ERROR("Unknown graphics_api option in config!");*/

                continue;
            }
        }

        {
            size_t a = line.find("max_fps");
            if (a != line.npos) {
                std::string option;
                size_t offset = a + 7 + 1; // 7 is the size of 'graphics_api' and 1 is '='
                option = line.substr(offset, line.size());

                float value = std::stof(option);

                if (value > 0)
                    GetSpecification().MaxFPS = value;
                else
                    TB_CORE_ERROR("Invalid max_fps value in config!");

                continue;
            }
        }

        {
            size_t a = line.find("vsync");
            if (a != line.npos) {
                std::string option;
                size_t offset = a + 5 + 1; // 5 is the size of 'vsync' and 1 is '='
                option = line.substr(offset, line.size());

                int value = std::stoi(option);

                if (value >= 0 && value <= 1)
                    GetSpecification().VSync = value;
                else
                    TB_CORE_ERROR("Invalid fullscreen value in config!");

                continue;
            }
        }

        {
            size_t a = line.find("fullscreen");
            if (a != line.npos) {
                std::string option;
                size_t offset = a + 10 + 1; // 10 is the size of 'fullscreen' and 1 is '='
                option = line.substr(offset, line.size());

                int value = std::stoi(option);

                if (value >= 0 && value <= 2)
                    GetSpecification().FullscreenMode = value;
                else
                    TB_CORE_ERROR("Invalid fullscreen value in config!");

                continue;
            }
        }
    }
}
}
