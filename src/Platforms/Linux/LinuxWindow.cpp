#include <tbpch.h>
#include <Tabby/Core/Events/ApplicationEvent.h>
#include <Tabby/Core/Events/MouseEvent.h>
#include <Platforms/Linux/LinuxWindow.h>
#include <Tabby/Core/Events/KeyEvent.h>
#include <Tabby/Renderer/Renderer.h>
#include <Tabby/Core/Input/Input.h>
#include <Tabby/Core/Application.h>

#include <SDL.h>
#include <SDL_syswm.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <backends/imgui_impl_sdl2.h>

namespace Tabby {

static uint8_t s_SDLWindowCount = 0;

LinuxWindow::LinuxWindow(const WindowProps& props)
{
    TB_PROFILE_SCOPE_NAME("Tabby::LinuxWindow::Constructor");

    Init(props);
}

LinuxWindow::~LinuxWindow()
{
    TB_PROFILE_SCOPE_NAME("Tabby::LinuxWindow::Destructor");

    Shutdown();
}

void LinuxWindow::Init(const WindowProps& props)
{

    // setenv("SDL_VIDEODRIVER", "wayland", 1);
    // setenv("SDL_VIDEODRIVER", "x11", 1);
    TB_PROFILE_SCOPE_NAME("Tabby::LinuxWindow::Init");

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
        TB_PROFILE_SCOPE_NAME("Tabby::LinuxWindow::Init::SDL_Init");
        int success = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
        if (success != 0) {
            TB_CORE_INFO("{}", SDL_GetError());
        }
        TB_CORE_ASSERT_TAGGED(success == 0, "Could not initialize SDL2!");
    }

    {
        TB_PROFILE_SCOPE_NAME("Tabby::LinuxWindow::Init::SDL_CreateWindow");

        SDL_WindowFlags window_flags = SDL_WINDOW_SHOWN;

        m_Window = SDL_CreateWindow(
            m_Data.Title.c_str(),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            props.Width, props.Height,
            window_flags);
        ++s_SDLWindowCount;
    }

    Input::Init();

    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(m_Window, &wmInfo);

    bgfx::PlatformData platform_data;
    if (wmInfo.subsystem == SDL_SYSWM_WAYLAND) {
        platform_data.nwh = (void*)wmInfo.info.wl.surface;
        platform_data.ndt = (void*)wmInfo.info.wl.display;
    } else {
        platform_data.nwh = (void*)wmInfo.info.x11.window;
        platform_data.ndt = (void*)wmInfo.info.x11.display;
    }

    if (wmInfo.subsystem == SDL_SYSWM_WAYLAND) {
        platform_data.type = bgfx::NativeWindowHandleType::Wayland;
    } else {
        platform_data.type = bgfx::NativeWindowHandleType::Default;
    }

    bgfx::Init bgfxInit;
    bgfxInit.type = bgfx::RendererType::Count; // Automatically choose a renderer.
    bgfxInit.platformData = platform_data;
    bgfxInit.resolution.width = props.Width;
    bgfxInit.resolution.height = props.Height;
    // bgfxInit.resolution.reset = BGFX_RESET_VSYNC;
    bgfx::init(bgfxInit);
    bgfx::setDebug(BGFX_DEBUG_TEXT /*| BGFX_DEBUG_STATS*/);

    SDL_SetWindowData(m_Window, "WindowData", &m_Data);
    SetVSync(props.VSync);
    SetResizable(props.Resizable);
    SetMinSize(props.MinWidth, props.MinHeight);
}

void LinuxWindow::Shutdown()
{
    TB_PROFILE_SCOPE_NAME("Tabby::LinuxWindow::Shutdown");

    SDL_DestroyWindow(m_Window);
    --s_SDLWindowCount;

    if (s_SDLWindowCount == 0) {
        SDL_Quit();
    }
}

void LinuxWindow::OnUpdate()
{
    TB_PROFILE_SCOPE_NAME("Tabby::LinuxWindow::OnUpdate");

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);

        switch (event.type) {
        case SDL_WINDOWEVENT: {
            if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                m_Data.Width = event.window.data1;
                m_Data.Height = event.window.data2;
                WindowResizeEvent resizeEvent(m_Data.Width, m_Data.Height);
                bgfx::reset(m_Data.Width, m_Data.Height);
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

void LinuxWindow::SetVSync(bool enabled)
{
    TB_PROFILE_SCOPE_NAME("Tabby::LinuxWindow::SetVSync");

    // TODO: overriding other flags
    if (enabled)
        bgfx::reset(m_Data.Width, m_Data.Height, BGFX_RESET_VSYNC);
    else
        bgfx::reset(m_Data.Width, m_Data.Height, BGFX_RESET_NONE);

    m_Data.VSync = enabled;
}

bool LinuxWindow::IsVSync() const
{
    TB_PROFILE_SCOPE_NAME("Tabby::LinuxWindow::IsVSync");

    return m_Data.VSync;
}

void LinuxWindow::SetResizable(bool enabled)
{
    TB_PROFILE_SCOPE_NAME("Tabby::LinuxWindow::SetResizable");

    if (enabled)
        SDL_SetWindowResizable(m_Window, SDL_TRUE);
    else
        SDL_SetWindowResizable(m_Window, SDL_FALSE);
    m_Data.Resizable = enabled;
}

bool LinuxWindow::IsResizable() const
{
    TB_PROFILE_SCOPE_NAME("Tabby::LinuxWindow::GetResizable");

    return m_Data.Resizable;
}

void LinuxWindow::SetMinSize(uint32_t minWidth, uint32_t minHeight)
{
    TB_PROFILE_SCOPE_NAME("Tabby::LinuxWindow::SetMinSize");

    SDL_SetWindowMinimumSize(m_Window, minWidth, minHeight);
    m_Data.MinWidth = minWidth;
    m_Data.MinHeight = minHeight;
}

void LinuxWindow::SetFullscreen(uint8_t mode)
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

uint8_t LinuxWindow::GetFullscreenMode() const
{
    return m_Data.FullscreenMode;
}
}
