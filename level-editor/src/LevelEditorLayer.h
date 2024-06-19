#include <Tabby.h>

#include <Core/LevelEditorLogic.h>
#include <UI/Canvas.h>

namespace Tabby {

class LevelEditorLayer : public Layer {
public:
    LevelEditorLayer();
    virtual ~LevelEditorLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;

    void OnUpdate(Timestep ts) override;
    virtual void OnImGuiRender() override;
    void OnEvent(Event& e) override;

private:
    Shared<Framebuffer> m_Framebuffer;
    Vector2 m_ViewportSize = { 1280.0f, 720.0f };
    std::array<Vector2, 2> m_ViewportBounds;

    TransformComponent m_CameraTransform;
    Camera m_Camera;

    Scope<LevelEditorLogic> m_LevelEditorLogic;
    Scope<ContentBrowserPanel> m_ContentBrowserPanel;
    Scope<Canvas> m_Canvas;
};

}
