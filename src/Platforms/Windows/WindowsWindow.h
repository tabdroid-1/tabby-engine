#pragma once
#include <Tabby/Core/Window.h>
#include <Tabby/Renderer/GraphicsContext.h>

class SDL_Window;

namespace Tabby {

class WindowsWindow : public Window {
public:
    WindowsWindow(const WindowProps& props);
    virtual ~WindowsWindow();

    void OnUpdate() override;

    unsigned int GetWidth() const override { return m_Data.Width; }
    unsigned int GetHeight() const override { return m_Data.Height; }

    // Window attributes
    void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }

    void SetVSync(bool enabled) override;
    bool IsVSync() const override;
    void SetResizable(bool enabled) override;
    bool IsResizable() const override;
    void SetFullscreen(uint8_t mode) override;
    uint8_t GetFullscreenMode() const override;

    void SetMinSize(uint32_t minWidth, uint32_t minHeight) override;

    virtual void* GetNativeWindow() const override { return m_Window; }

private:
    virtual void Init(const WindowProps& props);
    virtual void Shutdown();

private:
    SDL_Window* m_Window;
    Scope<GraphicsContext> m_Context;

    struct WindowData {
        std::string Title;
        bool Resizable;
        unsigned int Width, Height;
        unsigned int MinWidth, MinHeight;
        uint8_t FullscreenMode;
        bool VSync;

        EventCallbackFn EventCallback;
    };

    WindowData m_Data;
};

}
