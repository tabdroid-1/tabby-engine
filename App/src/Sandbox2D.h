#pragma once

#include "Tabby.h"

class Sandbox2D : public Tabby::Layer {
public:
    Sandbox2D();
    virtual ~Sandbox2D() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;

    void OnUpdate(Tabby::Timestep ts) override;
    virtual void OnImGuiRender() override;
    void OnEvent(Tabby::Event& e) override;

private:
    Tabby::OrthographicCameraController m_CameraController;

    // Temp
    Tabby::Ref<Tabby::VertexArray> m_SquareVA;
    Tabby::Ref<Tabby::Shader> m_FlatColorShader;

    Tabby::Ref<Tabby::Texture2D> m_CheckerboardTexture;
    Tabby::Ref<Tabby::Texture2D> m_NoTexture;

    glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

    Tabby::Ref<Tabby::Framebuffer> m_FrameBuffer;
    glm::vec2 m_ViewPortSize = { 1280.0f, 720.0f };
    glm::vec2 m_ViewportBounds[2];
    bool m_ViewportFocused = false, m_ViewportHovered = false;
};
