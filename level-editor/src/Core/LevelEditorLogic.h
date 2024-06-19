#pragma once

#include <Tabby.h>
#include <Tabby/UI/Panels/ContentBrowserPanel/ContentBrowserPanel.h>

namespace Tabby {

class LevelEditorLogic {
public:
    LevelEditorLogic(Camera* camera, TransformComponent* cameraTransform);
    ~LevelEditorLogic() = default;

    void OnAttach();
    void OnDetach();

    void Update(Timestep ts);
    void ImGuiRender();
    void OnEvent(Event& e);

private:
    bool OnMouseScroll(MouseScrolledEvent& e);

private:
    int m_GizmoType = -1;

    float m_CameraZoom = 1;
    Camera* m_Camera;
    TransformComponent* m_CameraTransform;
};

}
