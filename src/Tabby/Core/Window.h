#pragma once
#include <Tabby/Core/Events/Event.h>

namespace Tabby {

struct WindowProps {
    std::string Title;
    uint32_t Width;
    uint32_t Height;
    uint32_t MinWidth;
    uint32_t MinHeight;
    uint8_t FullscreenMode;
    bool Resizable;
    bool VSync;

    WindowProps(const std::string& title = "Tabby Engine",
        uint32_t width = 1600,
        uint32_t height = 900,
        uint32_t minWidth = 1600,
        uint32_t minHeight = 900,
        uint8_t FullscreenMode = 0,
        bool resizeable = true,
        bool vsync = true)
        : Title(title)
        , Width(width)
        , Height(height)
        , MinWidth(minWidth)
        , MinHeight(minHeight)
        , Resizable(resizeable)
        , VSync(vsync)
    {
    }
};

// Interface representing a desktop system based Window
class Window {
public:
    using EventCallbackFn = std::function<void(Event&)>;

    virtual ~Window() = default;

    virtual void OnUpdate() = 0;

    virtual uint32_t GetWidth() const = 0;
    virtual uint32_t GetHeight() const = 0;

    // Window attributes
    virtual void SetEventCallback(const EventCallbackFn& callback) = 0;

    virtual void SetVSync(bool enabled) = 0;
    virtual bool IsVSync() const = 0;
    virtual void SetResizable(bool enabled) = 0;
    virtual bool IsResizable() const = 0;
    virtual void SetFullscreen(uint8_t mode) = 0;
    virtual uint8_t GetFullscreenMode() const = 0;

    virtual void SetMinSize(uint32_t minWidth, uint32_t minHeight) = 0;

    virtual void* GetNativeWindow() const = 0;

    static Scope<Window> Create(const WindowProps& props = WindowProps());
};

}
