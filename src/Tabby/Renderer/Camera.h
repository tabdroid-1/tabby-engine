#pragma once

#include <Tabby/Foundation/Types.h>

namespace Tabby {

enum class CameraProjectionType {
    PERSPECTIVE,
    ORTHOGRAPHIC
};

class Camera {
public:
    Camera();
    Camera(CameraProjectionType types);

    const Matrix4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }
    const Matrix4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
    const Matrix4& GetViewMatrix() const { return m_ViewMatrix; }
    const Vector3 GetForwardVector() const;

    CameraProjectionType GetType() const { return m_Type; }
    float GetNearClip() const { return m_ZNear; }
    float GetFarClip() const { return m_ZFar; }
    float GetFOV() const { return m_FieldOfView; }
    float GetAspectRatio() const { return m_AspectRatio; }
    const Vector3& GetPosition() const { return m_Position; }
    const Quaternion& GetRotation() const { return m_Rotation; }
    const Vector3& GetScale() const { return m_Scale; }

    void SetType(CameraProjectionType type);
    void SetNearClip(float near_clip_distance);
    void SetFarClip(float far_clip_distance);
    void SetFOV(float field_of_view);
    void SetAspectRatio(float ratio);
    void SetPosition(const Vector3& position);
    void SetRotation(const Quaternion& rotation);
    void SetRotation(const Vector3& rotation);
    void SetScale(const Vector3& scale);

    void CalculateMatrix();

private:
    void GenerateFrustum();
    Matrix4 BuildNonReversedProjection() const;

private:
    CameraProjectionType m_Type;
    float m_AspectRatio = 1.0f;
    float m_ZNear = 0.0f;
    float m_ZFar = 1000.0f;
    float m_FieldOfView; // This will be used as FOV in PERSPECTIVE, and as size in ORTHOGRAPHIC

    Frustum m_Frustum;

    Matrix4 m_ViewProjectionMatrix;
    Matrix4 m_ProjectionMatrix;
    Matrix4 m_ViewMatrix;

    Vector3 m_Position;
    Quaternion m_Rotation;
    Vector3 m_Scale;

    static constexpr float cMinNearPlaneDistance = 10e-3f;
    static constexpr float cMaxFarPlaneDistance = 10e2f;
};
}
