#include <Tabby/Scene/Components.h>
#include <Tabby/Physics/2D/Physics2D.h>
#include <Tabby/Physics/2D/Physics2DTypes.h>
#include <Tabby/Math/Math.h>

#include <box2d/box2d.h>
#include "glm/fwd.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Tabby {

// ----- TransformComponent ---------------------
glm::mat4 TransformComponent::GetTransform() const
{
    glm::mat4 rotation = glm::toMat4(glm::quat(glm::radians(Rotation)));

    return glm::translate(glm::mat4(1.0f), Translation) * rotation * glm::scale(glm::mat4(1.0f), Scale);
}

glm::mat4 TransformComponent::GetLocalTransform() const
{
    glm::mat4 rotation = glm::toMat4(glm::quat(glm::radians(LocalRotation)));

    return glm::translate(glm::mat4(1.0f), LocalTranslation) * rotation * glm::scale(glm::mat4(1.0f), LocalScale);
}

void TransformComponent::ApplyTransform(const glm::mat4& transform)
{

    using namespace glm;
    using T = float;

    mat4 LocalMatrix(transform);

    // Normalize the matrix.
    if (epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), epsilon<T>()))
        return;

    // First, isolate perspective.  This is the messiest.
    if (epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) || epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) || epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>())) {
        // Clear the perspective partition
        LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
        LocalMatrix[3][3] = static_cast<T>(1);
    }

    // Next take care of translation (easy).
    Translation = vec3(LocalMatrix[3]);
    LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

    vec3 Row[3], Pdum3;

    // Now get scale and shear.
    for (length_t i = 0; i < 3; ++i)
        for (length_t j = 0; j < 3; ++j)
            Row[i][j] = LocalMatrix[i][j];

    // Compute X scale factor and normalize first row.
    Scale.x = length(Row[0]);
    Row[0] = detail::scale(Row[0], static_cast<T>(1));
    Scale.y = length(Row[1]);
    Row[1] = detail::scale(Row[1], static_cast<T>(1));
    Scale.z = length(Row[2]);
    Row[2] = detail::scale(Row[2], static_cast<T>(1));

    // At this point, the matrix (in rows[]) is orthonormal.
    // Check for a coordinate system flip.  If the determinant
    // is -1, then negate the matrix and the scaling factors.
#if 0
        		Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
        		if (dot(Row[0], Pdum3) < 0)
        		{
        			for (length_t i = 0; i < 3; i++)
        			{
        				scale[i] *= static_cast<T>(-1);
        				Row[i] *= static_cast<T>(-1);
        			}
        		}
#endif

    Rotation.y = asin(-Row[0][2]);
    if (cos(Rotation.y) != 0) {
        Rotation.x = atan2(Row[1][2], Row[2][2]);
        Rotation.z = atan2(Row[0][1], Row[0][0]);
    } else {
        Rotation.x = atan2(-Row[2][0], Row[1][1]);
        Rotation.z = 0;
    }

    Rotation.x *= Math::RAD2DEG;
    Rotation.y *= Math::RAD2DEG;
    Rotation.z *= Math::RAD2DEG;
}

// --------------------------------------------------
//
//
//
// ----- Rigidbody2DComponent -----------------------

void Rigidbody2DComponent::SetFixedRotation(bool enable)
{
    if (B2_IS_NON_NULL(RuntimeBodyId))
        b2Body_SetFixedRotation(RuntimeBodyId, enable);
    FixedRotation = enable;
}

void Rigidbody2DComponent::SetVelocity(glm::vec2 velocity)
{
    if (B2_IS_NON_NULL(RuntimeBodyId))
        b2Body_SetLinearVelocity(RuntimeBodyId, { velocity.x, velocity.y });
}

void Rigidbody2DComponent::SetAngularVelocity(float velocity)
{
    if (B2_IS_NON_NULL(RuntimeBodyId))
        b2Body_SetAngularVelocity(RuntimeBodyId, velocity);
}

glm::vec2 Rigidbody2DComponent::GetVelocity() const
{
    if (B2_IS_NON_NULL(RuntimeBodyId)) {
        b2Vec2 velocity = b2Body_GetLinearVelocity(RuntimeBodyId);
        return { velocity.x, velocity.y };
    } else
        return { 0.0f, 0.0f };
}

float Rigidbody2DComponent::GetAngularVelocity() const
{
    if (B2_IS_NON_NULL(RuntimeBodyId))
        return b2Body_GetAngularVelocity(RuntimeBodyId);
    else
        return 0.0f;
}

float Rigidbody2DComponent::GetAngle() const
{
    float angleRadians;

    if (B2_IS_NON_NULL(RuntimeBodyId))
        angleRadians = b2Body_GetAngle(RuntimeBodyId);

    // Convert the angle from radians to degrees
    float angleDegrees = angleRadians * (180.0f / b2_pi);

    // Wrap the angle to the range [0, 360]
    angleDegrees = std::fmod(angleDegrees, 360.0f);

    // If the angle is negative, add 360 to make it positive
    if (angleDegrees < 0.0f) {
        angleDegrees += 360.0f;
    }

    return angleDegrees;
}
// --------------------------------------------------
//
//
//
// ----- BoxCollider2DComponent -----------------------

void BoxCollider2DComponent::RefreshShape()
{
    ShapeInfo2D shapeInfo {
        static_cast<ShapeUserData2D*>(b2Shape_GetUserData(RuntimeShapeId))->shapeEntity,
        ColliderType2D::Circle
    };

    Physisc2D::EnqueueShapeUpdate(shapeInfo);
}

// --------------------------------------------------
//
//
//
// ----- CircleCollider2DComponent -----------------------

void CircleCollider2DComponent::RefreshShape()
{
    ShapeInfo2D shapeInfo {
        static_cast<ShapeUserData2D*>(b2Shape_GetUserData(RuntimeShapeId))->shapeEntity,
        ColliderType2D::Circle
    };

    Physisc2D::EnqueueShapeUpdate(shapeInfo);
}

// --------------------------------------------------
//
//
//
// ----- CapsuleCollider2DComponent -----------------------

void CapsuleCollider2DComponent::RefreshShape()
{
    ShapeInfo2D shapeInfo {
        static_cast<ShapeUserData2D*>(b2Shape_GetUserData(RuntimeShapeId))->shapeEntity,
        ColliderType2D::Circle
    };

    Physisc2D::EnqueueShapeUpdate(shapeInfo);
}

}
