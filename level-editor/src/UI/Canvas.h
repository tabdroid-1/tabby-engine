#pragma once

#include <Tabby.h>

namespace Tabby {

class Canvas {
public:
    Canvas(Shared<Framebuffer> framebuffer, Vector2& viewportSize, std::array<Vector2, 2>& viewportBounds);

    void Update(Timestep ts);
    void ImGuiRender();

    bool ShouldShowGameView() { return m_ShowGameView; }

private:
    // Settings
    bool m_ShowGameView = false;

    // Framebuffer/Viewport
    Vector2& m_ViewportSize;
    std::array<Vector2, 2>& m_ViewportBounds;
    Shared<Framebuffer> m_Framebuffer;
    bool m_ViewportFocused = false, m_ViewportHovered = false;
};

}
