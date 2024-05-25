#include <Tabby.h>

#include <Core/LevelEditor.h>

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
    Tabby::Shared<Framebuffer> m_Framebuffer;
    glm::vec2 m_ViewportSize = { 1280.0f, 720.0f };
    glm::vec2 m_ViewportBounds[2];
    bool m_ViewportFocused = false, m_ViewportHovered = false;

    TransformComponent m_CameraTransform;
    Camera m_Camera;

    Scope<LevelEditor> m_LevelEditor;
    Scope<ContentBrowserPanel> m_ContentBrowserPanel;
};

}
