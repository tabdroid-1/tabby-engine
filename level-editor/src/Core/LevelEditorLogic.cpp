#include <Core/LevelEditorLogic.h>

#include <imgui/imgui.h>
#include <imguizmo/ImGuizmo.h>

#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Tabby {

LevelEditorLogic::LevelEditorLogic(Camera* camera, TransformComponent* cameraTransform)
    : m_Camera(camera)
    , m_CameraTransform(cameraTransform)
{
}

void LevelEditorLogic::OnAttach()
{
}

void LevelEditorLogic::OnDetach()
{
}

void LevelEditorLogic::Update(Timestep ts)
{
    Frustum frustum(m_Camera->GetProjection(), m_CameraTransform->GetTransform());

    if (Input::IsKeyPressed(Key::D))
        m_CameraTransform->Translation.x += 5 * ts;
    if (Input::IsKeyPressed(Key::A))
        m_CameraTransform->Translation.x -= 5 * ts;
    if (Input::IsKeyPressed(Key::W))
        m_CameraTransform->Translation.y += 5 * ts;
    if (Input::IsKeyPressed(Key::S))
        m_CameraTransform->Translation.y -= 5 * ts;

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

    glm::mat4 rotation = glm::toMat4(glm::quat(glm::radians((glm::vec3)m_CameraTransform->Rotation)));
    m_CameraTransform->TransformMatrix = glm::translate(glm::mat4(1.0f), (glm::vec3)m_CameraTransform->Translation) * rotation * glm::scale(glm::mat4(1.0f), (glm::vec3)m_CameraTransform->Scale);
}

void LevelEditorLogic::ImGuiRender()
{
}

void LevelEditorLogic::OnEvent(Event& e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<MouseScrolledEvent>(TB_BIND_EVENT_FN(LevelEditorLogic::OnMouseScroll));
}

bool LevelEditorLogic::OnMouseScroll(MouseScrolledEvent& e)
{

    return false;
}

}
