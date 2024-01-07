#pragma once

#include "Tabby.h"

class Base : public Tabby::Layer {
public:
    Base();
    virtual ~Base() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;

    void OnUpdate(Tabby::Timestep ts) override;
    virtual void OnImGuiRender() override;
    void OnEvent(Tabby::Event& e) override;

private:
    Tabby::Ref<Tabby::Framebuffer> m_FrameBuffer;
    glm::vec2 m_ViewPortSize = { 1280.0f, 720.0f };
    glm::vec2 m_ViewportBounds[2];
    bool m_ViewportFocused = false, m_ViewportHovered = false;
};
