#pragma once

#include "GameOfLife.h"
#include "Tabby.h"

class GameOfLifeBase : public Tabby::Layer {
public:
    GameOfLifeBase();
    virtual ~GameOfLifeBase() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;

    void OnUpdate(Tabby::Timestep ts) override;
    virtual void OnImGuiRender() override;
    void OnEvent(Tabby::Event& e) override;

private:
    Tabby::OrthographicCameraController m_CameraController;

    Tabby::Ref<Tabby::Texture2D> NoLifeTexture;
    Tabby::Ref<Tabby::Texture2D> AliveTexture;

    Tabby::Ref<Tabby::Framebuffer> m_FrameBuffer;
    glm::vec2 m_ViewPortSize = { 1280.0f, 720.0f };
    glm::vec2 m_ViewportBounds[2];
    bool m_ViewportFocused = false, m_ViewportHovered = false;
};
