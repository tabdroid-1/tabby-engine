#pragma once

#include <Tabby/Renderer/GraphicsContext.h>
#include <Tabby/Core/Window.h>

class SDL_Window;

namespace Tabby {

class NullPlatformWindow : public Window {
public:
    NullPlatformWindow(const WindowProps& props);
    virtual ~NullPlatformWindow();

    void OnUpdate() override;

    unsigned int GetWidth() const override { return 0; }
    unsigned int GetHeight() const override { return 0; }

    // Window attributes
    void SetEventCallback(const EventCallbackFn& callback) override { }

    void SetVSync(bool enabled) override;
    bool IsVSync() const override;
    void SetResizable(bool enabled) override;
    bool IsResizable() const override;
    void SetFullscreen(uint8_t mode) override;
    uint8_t GetFullscreenMode() const override;

    void SetMinSize(uint32_t minWidth, uint32_t minHeight) override;

    virtual void* GetNativeWindow() const override { return nullptr; }

private:
    virtual void Init(const WindowProps& props);
    virtual void Shutdown();
};

}
