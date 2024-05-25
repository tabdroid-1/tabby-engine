#include <Core/LevelEditor.h>

#include <imgui/imgui.h>
#include <imguizmo/ImGuizmo.h>

namespace Tabby {

LevelEditor::LevelEditor(Camera* camera, TransformComponent* cameraTransform)
    : m_Camera(camera)
    , m_CameraTransform(cameraTransform)
{
}

void LevelEditor::OnAttach()
{
}

void LevelEditor::OnDetach()
{
}

void LevelEditor::OnUpdate(Timestep ts)
{
    Frustum frustum(m_Camera->GetProjection(), m_CameraTransform->GetTransform());

    if (Input::IsKeyPressed(Key::D))
        m_CameraTransform->Translation.x
            += 5 * ts;
    if (Input::IsKeyPressed(Key::A))
        m_CameraTransform->Translation.x -= 5 * ts;
    if (Input::IsKeyPressed(Key::W))
        m_CameraTransform->Translation.y += 5 * ts;
    if (Input::IsKeyPressed(Key::S))
        m_CameraTransform->Translation.y -= 5 * ts;

    if (Tabby::Input::IsKeyPressed(Tabby::Key::Q))
        m_GizmoType = -1;
    if (Tabby::Input::IsKeyPressed(Tabby::Key::T))
        m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
    if (Tabby::Input::IsKeyPressed(Tabby::Key::S))
        m_GizmoType = ImGuizmo::OPERATION::SCALE;
    if (Tabby::Input::IsKeyPressed(Tabby::Key::R))
        m_GizmoType = ImGuizmo::OPERATION::ROTATE;

    // Draw Grid lines
    {

        glm::vec3 xPoint1 = { frustum.GetPoints()[1].x, 0.0f, 0.0f };
        glm::vec3 xPoint2 = { frustum.GetPoints()[2].x, 0.0f, 0.0f };
        Renderer2D::DrawLine(xPoint1, xPoint2, glm::vec4(0.7f, 0.0f, 0.0f, 1.0f));

        glm::vec3 yPoint1 = { 0.0f, frustum.GetPoints()[1].y, 0.0f };
        glm::vec3 yPoint2 = { 0.0f, frustum.GetPoints()[2].y, 0.0f };
        Renderer2D::DrawLine(yPoint1, yPoint2, glm::vec4(0.0f, 0.7f, 0.0f, 1.0f));

        Renderer2D::DrawRotatedQuad({ 0.0f, 0.0f }, { 0.1f, 0.1f }, 45.0f, { 0.35f, 0.35f, 0.0f, 1.0f });
    }
    // {
    //
    //     int xTotalLines = Math::Abs(frustum.GetPoints()[1].x - frustum.GetPoints()[2].x) + 2;
    //     xTotalLines *= 2;
    //     int yTotalLines = Math::Abs(frustum.GetPoints()[1].y - frustum.GetPoints()[2].y) + 2;
    //     yTotalLines *= 2;
    //     for (int x = 0; x < xTotalLines; x++) {
    //
    //         glm::vec3 point1;
    //         glm::vec3 point2;
    //
    //         point1 = {
    //             std::floor(frustum.GetPoints()[1].x) + (float)x / 2,
    //             std::floor(frustum.GetPoints()[1].y + 1),
    //             -1
    //         };
    //         point2 = {
    //             std::floor(frustum.GetPoints()[1].x) + (float)x / 2,
    //             std::floor(frustum.GetPoints()[2].y),
    //             -1
    //         };
    //
    //         if (point1.x == 0)
    //             Renderer2D::DrawLine(point1, point2, glm::vec4(0.0f, 0.7f, 0.0f, 1.0f));
    //         else if (x % 2 == 0)
    //             Renderer2D::DrawLine(point1, point2, glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
    //         else if (x % 2 != 0)
    //             Renderer2D::DrawLine(point1, point2, glm::vec4(0.4f, 0.4f, 0.4f, 0.25f));
    //     }
    //
    //     for (int y = 0; y < yTotalLines; y++) {
    //
    //         glm::vec3 point1 = {
    //             std::floor(frustum.GetPoints()[1].x),
    //             std::floor(frustum.GetPoints()[1].y) - (float)y / 2,
    //             -1
    //         };
    //         glm::vec3 point2 = {
    //             std::floor(frustum.GetPoints()[2].x + 1),
    //             std::floor(frustum.GetPoints()[1].y) - (float)y / 2,
    //             -1
    //         };
    //
    //         if (point1.y == 0)
    //             Renderer2D::DrawLine(point1, point2, glm::vec4(0.7f, 0.0f, 0.0f, 1.0f));
    //         else if (y % 2 == 0)
    //             Renderer2D::DrawLine(point1, point2, glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
    //         else if (y % 2 != 0)
    //             Renderer2D::DrawLine(point1, point2, glm::vec4(0.4f, 0.4f, 0.4f, 0.25f));
    //     }
    // }
}

void LevelEditor::OnImGuiRender()
{
}

void LevelEditor::OnEvent(Event& e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<MouseScrolledEvent>(TB_BIND_EVENT_FN(LevelEditor::OnMouseScroll));
}

bool LevelEditor::OnMouseScroll(MouseScrolledEvent& e)
{
    // m_CameraZoom += e.GetYOffset() * 0.25f;
    // m_CameraZoom = std::max(m_CameraZoom, 0.25f);
    // m_CameraZoom = std::min(m_CameraZoom, 1.2f);
    //
    // TB_INFO("{0}", m_CameraZoom);

    // m_Camera->SetOrthographic(m_Camera->GetOrthographicSize() * m_CameraZoom, -1, 1000);

    return false;
}

}
