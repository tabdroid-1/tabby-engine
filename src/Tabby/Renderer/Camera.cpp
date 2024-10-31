#include <Tabby/Renderer/Camera.h>
#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>

namespace Tabby {

Camera::Camera()
{
    m_Type = CameraProjectionType::PERSPECTIVE;
    m_ProjectionMatrix = glm::mat4(1.0f);
    m_ZNear = cMinNearPlaneDistance;
    m_ZFar = cMaxFarPlaneDistance;
    m_AspectRatio = 16.0f / 9.0f;
    m_FieldOfView = glm::radians(60.0f);
    m_Position = Vector3(0.0);
    m_Rotation = Vector3(0.0);
    m_Scale = Vector3(1.0);
    m_ViewProjectionMatrix = Matrix4(1.0f);
    m_ProjectionMatrix = Matrix4(1.0f);
    m_ViewMatrix = Matrix4(1.0f);

    CalculateMatrix();
}

Camera::Camera(CameraProjectionType type)
{
    m_Type = type;
    m_ProjectionMatrix = glm::mat4(1.0f);
    m_ZNear = cMinNearPlaneDistance;
    m_ZFar = cMaxFarPlaneDistance;
    m_AspectRatio = 16.0f / 9.0f;
    m_FieldOfView = glm::radians(60.0f);
    m_Position = Vector3(0.0);
    m_Rotation = Vector3(0.0);
    m_Scale = Vector3(1.0);
    m_ViewProjectionMatrix = Matrix4(1.0f);
    m_ProjectionMatrix = Matrix4(1.0f);
    m_ViewMatrix = Matrix4(1.0f);

    CalculateMatrix();
}

void Camera::SetType(CameraProjectionType type)
{
    m_Type = type;
}

void Camera::SetFOV(float fov)
{
    m_FieldOfView = fov;
}

void Camera::SetNearClip(float near_clip_distance)
{
    m_ZNear = near_clip_distance;
}

void Camera::SetFarClip(float far_clip_distance)
{
    m_ZFar = far_clip_distance;
}

void Camera::SetAspectRatio(float ratio)
{
    m_AspectRatio = ratio;
}

void Camera::SetPosition(const Vector3& position)
{
    m_Position = position;
}

void Camera::SetRotation(const Quaternion& rotation)
{
    m_Rotation = rotation;
}

void Camera::SetRotation(const Vector3& rotation)
{
    m_Rotation = rotation;
}

void Camera::SetScale(const Vector3& scale)
{
    m_Scale = scale;
}

void Camera::CalculateMatrix()
{
    if (m_Type == CameraProjectionType::PERSPECTIVE)
        m_ProjectionMatrix = glm::perspective(m_FieldOfView, m_AspectRatio, m_ZNear, m_ZFar);
    else if (m_Type == CameraProjectionType::ORTHOGRAPHIC) {
        float orthoLeft = -m_FieldOfView * m_AspectRatio * 0.5f;
        float orthoRight = m_FieldOfView * m_AspectRatio * 0.5f;
        float orthoBottom = -m_FieldOfView * 0.5f;
        float orthoTop = m_FieldOfView * 0.5f;

        m_ProjectionMatrix = glm::ortho(orthoLeft, orthoRight,
            orthoBottom, orthoTop, m_ZNear, m_ZFar);
    }

    m_ViewMatrix = glm::translate(Matrix4(1.0f), (Vector3&)m_Position) * glm::toMat4(m_Rotation) * glm::scale(Matrix4(1.0f), (Vector3&)m_Scale);

    GenerateFrustum();
}

void Camera::GenerateFrustum()
{
    m_Frustum.planes[0].normal = Vector3(m_ProjectionMatrix[0][3] - m_ProjectionMatrix[0][1], m_ProjectionMatrix[1][3] - m_ProjectionMatrix[1][1], m_ProjectionMatrix[2][3] - m_ProjectionMatrix[2][1]);
    m_Frustum.planes[0].distance = m_ProjectionMatrix[3][3] - m_ProjectionMatrix[3][1];

    m_Frustum.planes[1].normal = Vector3(m_ProjectionMatrix[0][3] + m_ProjectionMatrix[0][1], m_ProjectionMatrix[1][3] + m_ProjectionMatrix[1][1], m_ProjectionMatrix[2][3] + m_ProjectionMatrix[2][1]);
    m_Frustum.planes[1].distance = m_ProjectionMatrix[3][3] + m_ProjectionMatrix[3][1];

    m_Frustum.planes[2].normal = Vector3(m_ProjectionMatrix[0][3] - m_ProjectionMatrix[0][0], m_ProjectionMatrix[1][3] - m_ProjectionMatrix[1][0], m_ProjectionMatrix[2][3] - m_ProjectionMatrix[2][0]);
    m_Frustum.planes[2].distance = m_ProjectionMatrix[3][3] - m_ProjectionMatrix[3][0];

    m_Frustum.planes[3].normal = Vector3(m_ProjectionMatrix[0][3] + m_ProjectionMatrix[0][0], m_ProjectionMatrix[1][3] + m_ProjectionMatrix[1][0], m_ProjectionMatrix[2][3] + m_ProjectionMatrix[2][0]);
    m_Frustum.planes[3].distance = m_ProjectionMatrix[3][3] + m_ProjectionMatrix[3][0];

    m_Frustum.planes[4].normal = Vector3(m_ProjectionMatrix[0][3] - m_ProjectionMatrix[0][2], m_ProjectionMatrix[1][3] - m_ProjectionMatrix[1][2], m_ProjectionMatrix[2][3] - m_ProjectionMatrix[2][2]);
    m_Frustum.planes[4].distance = m_ProjectionMatrix[3][3] - m_ProjectionMatrix[3][2];

    m_Frustum.planes[5].normal = Vector3(m_ProjectionMatrix[0][3] + m_ProjectionMatrix[0][2], m_ProjectionMatrix[1][3] + m_ProjectionMatrix[1][2], m_ProjectionMatrix[2][3] + m_ProjectionMatrix[2][2]);
    m_Frustum.planes[5].distance = m_ProjectionMatrix[3][3] + m_ProjectionMatrix[3][2];

    for (auto& plane : m_Frustum.planes)
        plane.normal = glm::normalize(plane.normal);
}

glm::mat4 Camera::BuildNonReversedProjection() const
{
    return glm::perspective(m_FieldOfView, m_AspectRatio, m_ZNear, m_ZFar);
}

}
