#include "SDL_hints.h"
#include "SDL_stdinc.h"
#include "Tabby/Core/Assert.h"
#include <tbpch.h>
#include <Tabby/Core/Events/ApplicationEvent.h>
#include <Tabby/Core/Events/MouseEvent.h>
#include <Platforms/MacOS/MacOSWindow.h>
#include <Tabby/Core/Events/KeyEvent.h>
#include <Tabby/Renderer/Renderer.h>
#include <Tabby/Core/Input/Input.h>
#include <Tabby/Core/Application.h>

#include <backends/imgui_impl_sdl2.h>
#include <SDL.h>
#include <SDL_vulkan.h>

namespace Tabby {

static uint8_t s_SDLWindowCount = 0;

MacOSWindow::MacOSWindow(const WindowProps& props)
{
    TB_PROFILE_SCOPE_NAME("Tabby::MacOSWindow::Constructor");

    Init(props);
}

MacOSWindow::~MacOSWindow()
{
    TB_PROFILE_SCOPE_NAME("Tabby::MacOSWindow::Destructor");

    Shutdown();
}

void MacOSWindow::Init(const WindowProps& props)
{
    TB_PROFILE_SCOPE_NAME("Tabby::MacOSWindow::Init");

    m_Data.Title = props.Title;
    m_Data.Width = props.Width;
    m_Data.Height = props.Height;
    m_Data.MinWidth = props.MinWidth;
    m_Data.MinHeight = props.MinHeight;
    m_Data.Resizable = props.Resizable;
    m_Data.FullscreenMode = props.FullscreenMode;
    m_Data.VSync = props.VSync;

    TB_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

    if (s_SDLWindowCount == 0) {
        TB_PROFILE_SCOPE_NAME("Tabby::MacOSWindow::Init::SDL_Init");
        int success = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
        if (success != 0) {
            TB_CORE_INFO("{}", SDL_GetError());
        }
        TB_CORE_ASSERT_TAGGED(success == 0, "Could not initialize SDL2!");
    }

    {
        TB_PROFILE_SCOPE_NAME("Tabby::LinuxWindow::Init::SDL_CreateWindow");

        SDL_WindowFlags window_flags = SDL_WINDOW_SHOWN;

        if (Renderer::GetAPI() == Renderer::API::OpenGL46) {

            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#if defined(TB_DEBUG)
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif
            window_flags = (SDL_WindowFlags)(window_flags | SDL_WINDOW_OPENGL);
        } else if (Renderer::GetAPI() == Renderer::API::OpenGL33) {

            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#if defined(TB_DEBUG)
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif
            window_flags = (SDL_WindowFlags)(window_flags | SDL_WINDOW_OPENGL);
        } else if (Renderer::GetAPI() == Renderer::API::OpenGLES3) {

            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#if defined(TB_DEBUG)
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif
            window_flags = (SDL_WindowFlags)(window_flags | SDL_WINDOW_OPENGL);
        } else if (Renderer::GetAPI() == Renderer::API::Vulkan) {
            window_flags = (SDL_WindowFlags)(window_flags | SDL_WINDOW_VULKAN);
            SDL_Vulkan_LoadLibrary(nullptr);
        }

        m_Window = SDL_CreateWindow(
            m_Data.Title.c_str(),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            props.Width, props.Height,
            window_flags);
        ++s_SDLWindowCount;
    }

    SDL_SetWindowData(m_Window, "WindowData", &m_Data);
    SetVSync(props.VSync);
    SetResizable(props.Resizable);
    SetMinSize(props.MinWidth, props.MinHeight);

    Input::Init();
}

void MacOSWindow::Shutdown()
{
    TB_PROFILE_SCOPE_NAME("Tabby::MacOSWindow::Shutdown");

    SDL_DestroyWindow(m_Window);
    --s_SDLWindowCount;

    SDL_Vulkan_UnloadLibrary();
    if (s_SDLWindowCount == 0) {
        SDL_Quit();
    }
}

void MacOSWindow::OnUpdate()
{
    TB_PROFILE_SCOPE_NAME("Tabby::MacOSWindow::OnUpdate");

    SDL_Event event;
    while (SDL_PollEvent(&event)) {

        // ImGui_ImplSDL2_ProcessEvent(&event);
        switch (event.type) {
        case SDL_WINDOWEVENT: {
            if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                m_Data.Width = event.window.data1;
                m_Data.Height = event.window.data2;

                WindowResizeEvent resizeEvent(m_Data.Width, m_Data.Height);
                m_Data.EventCallback(resizeEvent);
            }
            break;
        }

        case SDL_QUIT: {
            WindowCloseEvent closeEvent;
            m_Data.EventCallback(closeEvent);
            break;
        }

        case SDL_KEYDOWN: {
            if (event.key.repeat == 0) {
                KeyPressedEvent keyPressedEvent(event.key.keysym.scancode, false);
                m_Data.EventCallback(keyPressedEvent);
            }
            break;
        }

        case SDL_KEYUP: {
            KeyReleasedEvent keyReleasedEvent(event.key.keysym.scancode);
            m_Data.EventCallback(keyReleasedEvent);
            break;
        }

        case SDL_TEXTINPUT: {
            KeyTypedEvent keyTypedEvent(event.text.text[0]);
            m_Data.EventCallback(keyTypedEvent);
            break;
        }

        case SDL_MOUSEBUTTONDOWN: {
            if (event.button.state == SDL_PRESSED) {
                MouseButtonPressedEvent mouseButtonPressedEvent(event.button.button);
                m_Data.EventCallback(mouseButtonPressedEvent);
            }
            break;
        }

        case SDL_MOUSEBUTTONUP: {
            if (event.button.state == SDL_RELEASED) {
                MouseButtonReleasedEvent mouseButtonReleasedEvent(event.button.button);
                m_Data.EventCallback(mouseButtonReleasedEvent);
            }
            break;
        }

        case SDL_MOUSEMOTION: {
            MouseMovedEvent mouseMovedEvent(static_cast<float>(event.motion.x), static_cast<float>(event.motion.y));
            m_Data.EventCallback(mouseMovedEvent);
            break;
        }

        case SDL_MOUSEWHEEL: {
            MouseScrolledEvent mouseScrolledEvent(static_cast<float>(event.wheel.x), static_cast<float>(event.wheel.y));
            m_Data.EventCallback(mouseScrolledEvent);
            break;
        }
        }
    }
}

void MacOSWindow::SetVSync(bool enabled)
{
    TB_PROFILE_SCOPE_NAME("Tabby::MacOSWindow::SetVSync");

    if (enabled)
        SDL_GL_SetSwapInterval(1);
    else
        SDL_GL_SetSwapInterval(0);

    m_Data.VSync = enabled;
}

bool MacOSWindow::IsVSync() const
{
    TB_PROFILE_SCOPE_NAME("Tabby::MacOSWindow::IsVSync");

    return m_Data.VSync;
}

void MacOSWindow::SetResizable(bool enabled)
{
    TB_PROFILE_SCOPE_NAME("Tabby::MacOSWindow::SetResizable");

    if (enabled)
        SDL_SetWindowResizable(m_Window, SDL_TRUE);
    else
        SDL_SetWindowResizable(m_Window, SDL_FALSE);
    m_Data.Resizable = enabled;
}

bool MacOSWindow::IsResizable() const
{
    TB_PROFILE_SCOPE_NAME("Tabby::MacOSWindow::GetResizable");

    return m_Data.Resizable;
}

void MacOSWindow::SetMinSize(uint32_t minWidth, uint32_t minHeight)
{
    TB_PROFILE_SCOPE_NAME("Tabby::MacOSWindow::SetMinSize");

    SDL_SetWindowMinimumSize(m_Window, minWidth, minHeight);
    m_Data.MinWidth = minWidth;
    m_Data.MinHeight = minHeight;
}

void MacOSWindow::SetFullscreen(uint8_t mode)
{
    if (mode > 2) {
        TB_CORE_ERROR("Invalid window fullscreen mode! It should be 0, 1 or 2.");
        return;
    }

    m_Data.FullscreenMode = mode;

    if (mode == 0)
        SDL_SetWindowFullscreen(m_Window, 0);
    else if (mode == 1)
        SDL_SetWindowFullscreen(m_Window, SDL_WINDOW_FULLSCREEN);
    else if (mode == 2)
        SDL_SetWindowFullscreen(m_Window, SDL_WINDOW_FULLSCREEN_DESKTOP);
}

uint8_t MacOSWindow::GetFullscreenMode() const
{
    return m_Data.FullscreenMode;
}

}
