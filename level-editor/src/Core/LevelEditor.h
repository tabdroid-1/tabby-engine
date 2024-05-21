#pragma once

#include <Tabby.h>
#include <Tabby/UI/Panels/ContentBrowserPanel/ContentBrowserPanel.h>

namespace Tabby {

class LevelEditor {
public:
    LevelEditor(Camera* camera, TransformComponent* cameraTransform);
    ~LevelEditor() = default;

    void OnAttach();
    void OnDetach();

    void OnUpdate(Timestep ts);
    void OnImGuiRender();
    void OnEvent(Event& e);

private:
    bool OnMouseScroll(MouseScrolledEvent& e);

private:
    Scope<ContentBrowserPanel> m_ContentBrowserPanel;
    int m_GizmoType = -1;

    float m_CameraZoom = 1;
    Camera* m_Camera;
    TransformComponent* m_CameraTransform;
};

}
