#include <Tabby/Physics/2D/Physics2DTypes.h>
#include <Tabby/Physics/2D/Physics2DUtil.h>
#include <Tabby/Physics/2D/Physics2D.h>
#include <Tabby/Audio/AudioEngine.h>
#include <Tabby/Audio/AudioSource.h>
#include <Tabby/World/Components.h>
#include <Tabby/Math/Math.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <box2d/box2d.h>

namespace Tabby {

// ----- TransformComponent ---------------------
void TransformComponent::ApplyTransform(const Matrix4& transform)
{
    TB_PROFILE_SCOPE_NAME("Tabby::TransformComponent::ApplyTransform");

    // using namespace glm;
    using T = float;

    Matrix4 LocalMatrix(transform);

    // Normalize the matrix.
    if (glm::epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), glm::epsilon<T>()))
        return;

    // First, isolate perspective.  This is the messiest.
    if (glm::epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), glm::epsilon<T>()) || glm::epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), glm::epsilon<T>()) || glm::epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), glm::epsilon<T>())) {
        // Clear the perspective partition
        LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
        LocalMatrix[3][3] = static_cast<T>(1);
    }

    // Next take care of translation (easy).
    Translation = Vector3(LocalMatrix[3]);
    LocalMatrix[3] = Vector4(0, 0, 0, LocalMatrix[3].w);

    Vector3 Row[3];

    // Now get scale and shear.
    for (glm::length_t i = 0; i < 3; ++i)
        for (glm::length_t j = 0; j < 3; ++j)
            Row[i][j] = LocalMatrix[i][j];

    // Compute X scale factor and normalize first row.
    Scale.x = length(Row[0]);
    Row[0] = glm::detail::scale(Row[0], static_cast<T>(1));
    Scale.y = length(Row[1]);
    Row[1] = glm::detail::scale(Row[1], static_cast<T>(1));
    Scale.z = length(Row[2]);
    Row[2] = glm::detail::scale(Row[2], static_cast<T>(1));

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

void TransformComponent::ApplyTransformToLocal(const Matrix4& transform)
{
    TB_PROFILE_SCOPE_NAME("Tabby::TransformComponent::ApplyTransformToLocal");

    // using namespace glm;
    using T = float;

    Matrix4 LocalMatrix(transform);

    // Normalize the matrix.
    if (glm::epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), glm::epsilon<T>()))
        return;

    // First, isolate perspective.  This is the messiest.
    if (glm::epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), glm::epsilon<T>()) || glm::epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), glm::epsilon<T>()) || glm::epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), glm::epsilon<T>())) {
        // Clear the perspective partition
        LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
        LocalMatrix[3][3] = static_cast<T>(1);
    }

    // Next take care of translation (easy).
    LocalTranslation = Vector3(LocalMatrix[3]);
    LocalMatrix[3] = Vector4(0, 0, 0, LocalMatrix[3].w);

    Vector3 Row[3];

    // Now get scale and shear.
    for (glm::length_t i = 0; i < 3; ++i)
        for (glm::length_t j = 0; j < 3; ++j)
            Row[i][j] = LocalMatrix[i][j];

    // Compute X scale factor and normalize first row.
    LocalScale.x = length(Row[0]);
    Row[0] = glm::detail::scale(Row[0], static_cast<T>(1));
    LocalScale.y = length(Row[1]);
    Row[1] = glm::detail::scale(Row[1], static_cast<T>(1));
    LocalScale.z = length(Row[2]);
    Row[2] = glm::detail::scale(Row[2], static_cast<T>(1));

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

    LocalRotation.y = asin(-Row[0][2]);
    if (cos(Rotation.y) != 0) {
        LocalRotation.x = atan2(Row[1][2], Row[2][2]);
        LocalRotation.z = atan2(Row[0][1], Row[0][0]);
    } else {
        LocalRotation.x = atan2(-Row[2][0], Row[1][1]);
        LocalRotation.z = 0;
    }

    LocalRotation.x *= Math::RAD2DEG;
    LocalRotation.y *= Math::RAD2DEG;
    LocalRotation.z *= Math::RAD2DEG;
}

// --------------------------------------------------
//
//
//
// ----- AudioSourceComponent -----------------------

void AudioSourceComponent::SetAudio(AssetHandle audioHandle)
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSourceComponent::SetAudio");

    if (!Source)
        Source = AudioEngine::CreateAudioSource();

    Source->SetAudio(audioHandle);
}
void AudioSourceComponent::Play()
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSourceComponent::Play");

    if (!Source)
        Source = AudioEngine::CreateAudioSource();

    Source->Play();
}
void AudioSourceComponent::Pause()
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSourceComponent::Pause");

    if (!Source)
        Source = AudioEngine::CreateAudioSource();

    Source->Pause();
}
void AudioSourceComponent::TogglePlay()
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSourceComponent::TogglePlay");

    if (!Source)
        Source = AudioEngine::CreateAudioSource();

    Source->TogglePlay();
}
bool AudioSourceComponent::IsPlaying()
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSourceComponent::IsPlaying");

    if (!Source)
        Source = AudioEngine::CreateAudioSource();

    return Source->IsPlaying();
}

void AudioSourceComponent::SetPitch(float pitch)
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSourceComponent::SetPitch");

    if (!Source)
        Source = AudioEngine::CreateAudioSource();

    Source->SetPitch(pitch);
}

void AudioSourceComponent::SetGain(float gain)
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSourceComponent::SetGain");

    if (!Source)
        Source = AudioEngine::CreateAudioSource();

    Source->SetGain(gain);
}

void AudioSourceComponent::SetRolloffFactor(float rate)
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSourceComponent::SetRolloffFactor");

    if (!Source)
        Source = AudioEngine::CreateAudioSource();

    Source->SetRolloffFactor(rate);
}
void AudioSourceComponent::SetMaxDistance(float distance)
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSourceComponent::SetMaxDistance");

    if (!Source)
        Source = AudioEngine::CreateAudioSource();

    Source->SetMaxDistance(distance);
}
void AudioSourceComponent::SetReferenceDistance(float distance)
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSourceComponent::SetReferenceDistance");

    if (!Source)
        Source = AudioEngine::CreateAudioSource();

    Source->SetReferenceDistance(distance);
}

void AudioSourceComponent::SetRelative(bool relative)
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSourceComponent::SetRelative");

    if (!Source)
        Source = AudioEngine::CreateAudioSource();

    Source->SetRelative(relative);
}

void AudioSourceComponent::SetLooping(bool looping)
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSourceComponent::SetLooping");

    if (!Source)
        Source = AudioEngine::CreateAudioSource();

    Source->SetLooping(looping);
}

void AudioSourceComponent::SetMinGain(float gain)
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSourceComponent::SetMinGain");

    if (!Source)
        Source = AudioEngine::CreateAudioSource();

    Source->SetMinGain(gain);
}

void AudioSourceComponent::SetMaxGain(float gain)
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSourceComponent::SetMaxGain");

    if (!Source)
        Source = AudioEngine::CreateAudioSource();

    Source->SetMaxGain(gain);
}

// void AudioSourceComponent::SetDirection(const Vector3& direction)
// {
//     if (!Source)
//         Source = AudioEngine::CreateAudioSource();
// }

void AudioSourceComponent::SetConeOuterGain(float gain)
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSourceComponent::SetConeOuterGain");

    if (!Source)
        Source = AudioEngine::CreateAudioSource();

    Source->SetConeOuterGain(gain);
}

void AudioSourceComponent::SetConeInnerAngle(float angle)
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSourceComponent::SetConeInnerAngle");

    if (!Source)
        Source = AudioEngine::CreateAudioSource();

    Source->SetConeInnerAngle(angle);
}

void AudioSourceComponent::SetConeOuterAngle(float angle)
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSourceComponent::SetConeOuterAngle");

    if (!Source)
        Source = AudioEngine::CreateAudioSource();

    Source->SetConeOuterAngle(angle);
}

// --------------------------------------------------
//
//
//
// ----- Rigidbody2DComponent -----------------------

void Rigidbody2DComponent::SetBodyType(BodyType type)
{
    TB_PROFILE_SCOPE_NAME("Tabby::Rigidbody2DComponent::SetBodyType");

    if (B2_IS_NON_NULL(RuntimeBodyId))
        b2Body_SetType(RuntimeBodyId, Utils::Rigidbody2DTypeToBox2DBody(type));
    Type = type;
}

void Rigidbody2DComponent::SetFixedRotation(bool enable)
{
    TB_PROFILE_SCOPE_NAME("Tabby::Rigidbody2DComponent::SetFixedRotation");

    if (B2_IS_NON_NULL(RuntimeBodyId))
        b2Body_SetFixedRotation(RuntimeBodyId, enable);
    FixedRotation = enable;
}

void Rigidbody2DComponent::SetVelocity(Vector2 velocity)
{
    TB_PROFILE_SCOPE_NAME("Tabby::Rigidbody2DComponent::SetVelocity");

    if (B2_IS_NON_NULL(RuntimeBodyId))
        b2Body_SetLinearVelocity(RuntimeBodyId, { velocity.x, velocity.y });
}

void Rigidbody2DComponent::SetAngularVelocity(float velocity)
{
    TB_PROFILE_SCOPE_NAME("Tabby::Rigidbody2DComponent::SetAngularVelocity");

    if (B2_IS_NON_NULL(RuntimeBodyId))
        b2Body_SetAngularVelocity(RuntimeBodyId, velocity);
}

Rigidbody2DComponent::BodyType Rigidbody2DComponent::GetType() const
{
    TB_PROFILE_SCOPE_NAME("Tabby::Rigidbody2DComponent::GetType");

    return Type;
}

Vector2 Rigidbody2DComponent::GetVelocity() const
{
    TB_PROFILE_SCOPE_NAME("Tabby::Rigidbody2DComponent::GetVelocity");

    if (B2_IS_NON_NULL(RuntimeBodyId)) {
        b2Vec2 velocity = b2Body_GetLinearVelocity(RuntimeBodyId);
        return { velocity.x, velocity.y };
    } else
        return { 0.0f, 0.0f };
}

float Rigidbody2DComponent::GetAngularVelocity() const
{
    TB_PROFILE_SCOPE_NAME("Tabby::Rigidbody2DComponent::GetAngularVelocity");

    if (B2_IS_NON_NULL(RuntimeBodyId))
        return b2Body_GetAngularVelocity(RuntimeBodyId);
    else
        return 0.0f;
}

Vector2 Rigidbody2DComponent::GetPosition() const
{
    TB_PROFILE_SCOPE_NAME("Tabby::Rigidbody2DComponent::GetPosition");

    if (B2_IS_NON_NULL(RuntimeBodyId)) {
        b2Vec2 position = b2Body_GetPosition(RuntimeBodyId);
        return { position.x, position.y };
    } else
        return { 0.0f, 0.0f };
}

float Rigidbody2DComponent::GetAngle() const
{
    TB_PROFILE_SCOPE_NAME("Tabby::Rigidbody2DComponent::GetAngle");

    float angleRadians = 0;

    if (B2_IS_NON_NULL(RuntimeBodyId))
        angleRadians = b2Rot_GetAngle(b2Body_GetRotation(RuntimeBodyId));

    float angleDegrees = angleRadians * Math::RAD2DEG;

    angleDegrees = std::fmod(angleDegrees, 360.0f);

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

void BoxCollider2DComponent::SetCollisionLayer(uint32_t layer)
{
    TB_PROFILE_SCOPE_NAME("Tabby::BoxCollider2DComponent::SetCollisionLayer");

    CollisionLayer = layer;
    if (B2_IS_NON_NULL(RuntimeShapeId)) {
        b2Filter filter = b2Shape_GetFilter(RuntimeShapeId);
        filter.categoryBits = layer;
        b2Shape_SetFilter(RuntimeShapeId, filter);
    }
}

uint32_t BoxCollider2DComponent::GetCollisionLayer() const
{
    TB_PROFILE_SCOPE_NAME("Tabby::BoxCollider2DComponent::GetCollisionLayer");

    return CollisionLayer;
}

void BoxCollider2DComponent::SetCollisionMask(uint32_t mask)
{
    TB_PROFILE_SCOPE_NAME("Tabby::BoxCollider2DComponent::SetCollisionMask");

    CollisionMask = mask;
    if (B2_IS_NON_NULL(RuntimeShapeId)) {
        b2Filter filter = b2Shape_GetFilter(RuntimeShapeId);
        filter.maskBits = mask;
        b2Shape_SetFilter(RuntimeShapeId, filter);
    }
}

uint32_t BoxCollider2DComponent::GetCollisionMask() const
{
    TB_PROFILE_SCOPE_NAME("Tabby::BoxCollider2DComponent::GetCollisionMask");

    return CollisionMask;
}

void BoxCollider2DComponent::SetCollisionLayerValue(int layerNumber, bool value)
{
    TB_PROFILE_SCOPE_NAME("Tabby::BoxCollider2DComponent::SetCollisionLayerValue");

    TB_CORE_VERIFY_TAGGED(layerNumber >= 1, "Collision layer number must be between 1 and 32 inclusive.");
    TB_CORE_VERIFY_TAGGED(layerNumber <= 32, "Collision layer number must be between 1 and 32 inclusive.");
    if (value) {
        CollisionLayer |= 1 << (layerNumber - 1);
    } else {
        CollisionLayer &= ~(1 << (layerNumber - 1));
    }

    if (B2_IS_NON_NULL(RuntimeShapeId)) {
        b2Filter filter = b2Shape_GetFilter(RuntimeShapeId);
        filter.categoryBits = CollisionLayer;
        b2Shape_SetFilter(RuntimeShapeId, filter);
    }
}

bool BoxCollider2DComponent::GetCollisionLayerValue(int layerNumber) const
{
    TB_PROFILE_SCOPE_NAME("Tabby::BoxCollider2DComponent::GetCollisionLayerValue");

    TB_CORE_VERIFY_TAGGED(layerNumber >= 1, "Collision layer number must be between 1 and 32 inclusive.");
    TB_CORE_VERIFY_TAGGED(layerNumber <= 32, "Collision layer number must be between 1 and 32 inclusive.");
    return CollisionLayer & (1 << (layerNumber - 1));
}

void BoxCollider2DComponent::SetCollisionMaskValue(int layerNumber, bool value)
{
    TB_PROFILE_SCOPE_NAME("Tabby::BoxCollider2DComponent::SetCollisionMaskValue");

    TB_CORE_VERIFY_TAGGED(layerNumber >= 1, "Collision layer number must be between 1 and 32 inclusive.");
    TB_CORE_VERIFY_TAGGED(layerNumber <= 32, "Collision layer number must be between 1 and 32 inclusive.");
    if (value) {
        CollisionMask |= 1 << (layerNumber - 1);
    } else {
        CollisionMask &= ~(1 << (layerNumber - 1));
    }

    if (B2_IS_NON_NULL(RuntimeShapeId)) {
        b2Filter filter = b2Shape_GetFilter(RuntimeShapeId);
        filter.maskBits = CollisionMask;
        b2Shape_SetFilter(RuntimeShapeId, filter);
    }
}

bool BoxCollider2DComponent::GetCollisionMaskValue(int layerNumber) const
{
    TB_PROFILE_SCOPE_NAME("Tabby::BoxCollider2DComponent::GetCollisionMaskValue");

    TB_CORE_VERIFY_TAGGED(layerNumber >= 1, "Collision layer number must be between 1 and 32 inclusive.");
    TB_CORE_VERIFY_TAGGED(layerNumber <= 32, "Collision layer number must be between 1 and 32 inclusive.");
    return CollisionMask & (1 << (layerNumber - 1));
}

void BoxCollider2DComponent::RefreshShape()
{
    TB_PROFILE_SCOPE_NAME("Tabby::BoxCollider2DComponent::RefreshShape");

    ShapeInfo2D shapeInfo {
        static_cast<ShapeUserData2D*>(b2Shape_GetUserData(RuntimeShapeId))->ShapeEntity,
        ColliderType2D::Circle
    };

    Physics2D::EnqueueShapeUpdate(shapeInfo);
}

// --------------------------------------------------
//
//
//
// ----- CircleCollider2DComponent -----------------------

void CircleCollider2DComponent::SetCollisionLayer(uint32_t layer)
{
    TB_PROFILE_SCOPE_NAME("Tabby::CircleCollider2DComponent::SetCollisionLayer");

    CollisionLayer = layer;
    if (B2_IS_NON_NULL(RuntimeShapeId)) {
        b2Filter filter = b2Shape_GetFilter(RuntimeShapeId);
        filter.categoryBits = layer;
        b2Shape_SetFilter(RuntimeShapeId, filter);
    }
}
uint32_t CircleCollider2DComponent::GetCollisionLayer() const
{
    TB_PROFILE_SCOPE_NAME("Tabby::CircleCollider2DComponent::GetCollisionLayer");

    return CollisionLayer;
}

void CircleCollider2DComponent::SetCollisionMask(uint32_t mask)
{
    TB_PROFILE_SCOPE_NAME("Tabby::CircleCollider2DComponent::SetCollisionMask");

    CollisionMask = mask;
    if (B2_IS_NON_NULL(RuntimeShapeId)) {
        b2Filter filter = b2Shape_GetFilter(RuntimeShapeId);
        filter.maskBits = mask;
        b2Shape_SetFilter(RuntimeShapeId, filter);
    }
}
uint32_t CircleCollider2DComponent::GetCollisionMask() const
{
    TB_PROFILE_SCOPE_NAME("Tabby::CircleCollider2DComponent::GetCollisionMask");

    return CollisionMask;
}

void CircleCollider2DComponent::SetCollisionLayerValue(int layerNumber, bool value)
{
    TB_PROFILE_SCOPE_NAME("Tabby::CircleCollider2DComponent::SetCollisionLayerValue");

    TB_CORE_VERIFY_TAGGED(layerNumber >= 1, "Collision layer number must be between 1 and 32 inclusive.");
    TB_CORE_VERIFY_TAGGED(layerNumber <= 32, "Collision layer number must be between 1 and 32 inclusive.");
    if (value) {
        CollisionLayer |= 1 << (layerNumber - 1);
    } else {
        CollisionLayer &= ~(1 << (layerNumber - 1));
    }

    if (B2_IS_NON_NULL(RuntimeShapeId)) {
        b2Filter filter = b2Shape_GetFilter(RuntimeShapeId);
        filter.categoryBits = CollisionLayer;
        b2Shape_SetFilter(RuntimeShapeId, filter);
    }
}
bool CircleCollider2DComponent::GetCollisionLayerValue(int layerNumber) const
{
    TB_PROFILE_SCOPE_NAME("Tabby::CircleCollider2DComponent::GetCollisionLayerValue");

    TB_CORE_VERIFY_TAGGED(layerNumber >= 1, "Collision layer number must be between 1 and 32 inclusive.");
    TB_CORE_VERIFY_TAGGED(layerNumber <= 32, "Collision layer number must be between 1 and 32 inclusive.");
    return CollisionLayer & (1 << (layerNumber - 1));
}

void CircleCollider2DComponent::SetCollisionMaskValue(int layerNumber, bool value)
{
    TB_PROFILE_SCOPE_NAME("Tabby::CircleCollider2DComponent::SetCollisionMaskValue");

    TB_CORE_VERIFY_TAGGED(layerNumber >= 1, "Collision layer number must be between 1 and 32 inclusive.");
    TB_CORE_VERIFY_TAGGED(layerNumber <= 32, "Collision layer number must be between 1 and 32 inclusive.");
    if (value) {
        CollisionMask |= 1 << (layerNumber - 1);
    } else {
        CollisionMask &= ~(1 << (layerNumber - 1));
    }

    if (B2_IS_NON_NULL(RuntimeShapeId)) {
        b2Filter filter = b2Shape_GetFilter(RuntimeShapeId);
        filter.maskBits = CollisionMask;
        b2Shape_SetFilter(RuntimeShapeId, filter);
    }
}
bool CircleCollider2DComponent::GetCollisionMaskValue(int layerNumber) const
{
    TB_PROFILE_SCOPE_NAME("Tabby::CircleCollider2DComponent::GetCollisionMaskValue");

    TB_CORE_VERIFY_TAGGED(layerNumber >= 1, "Collision layer number must be between 1 and 32 inclusive.");
    TB_CORE_VERIFY_TAGGED(layerNumber <= 32, "Collision layer number must be between 1 and 32 inclusive.");
    return CollisionMask & (1 << (layerNumber - 1));
}

void CircleCollider2DComponent::RefreshShape()
{
    TB_PROFILE_SCOPE_NAME("Tabby::CircleCollider2DComponent::RefreshShape");

    ShapeInfo2D shapeInfo {
        static_cast<ShapeUserData2D*>(b2Shape_GetUserData(RuntimeShapeId))->ShapeEntity,
        ColliderType2D::Circle
    };

    Physics2D::EnqueueShapeUpdate(shapeInfo);
}

// --------------------------------------------------
//
//
//
// ----- CapsuleCollider2DComponent -----------------------

void CapsuleCollider2DComponent::SetCollisionLayer(uint32_t layer)
{
    TB_PROFILE_SCOPE_NAME("Tabby::CapsuleCollider2DComponent::SetCollisionLayer");

    CollisionLayer = layer;
    if (B2_IS_NON_NULL(RuntimeShapeId)) {
        b2Filter filter = b2Shape_GetFilter(RuntimeShapeId);
        filter.categoryBits = layer;
        b2Shape_SetFilter(RuntimeShapeId, filter);
    }
}

uint32_t CapsuleCollider2DComponent::GetCollisionLayer() const
{
    TB_PROFILE_SCOPE_NAME("Tabby::CapsuleCollider2DComponent::GetCollisionLayer");

    return CollisionLayer;
}

void CapsuleCollider2DComponent::SetCollisionMask(uint32_t mask)
{
    TB_PROFILE_SCOPE_NAME("Tabby::CapsuleCollider2DComponent::SetCollisionMask");

    CollisionMask = mask;
    if (B2_IS_NON_NULL(RuntimeShapeId)) {
        b2Filter filter = b2Shape_GetFilter(RuntimeShapeId);
        filter.maskBits = mask;
        b2Shape_SetFilter(RuntimeShapeId, filter);
    }
}

uint32_t CapsuleCollider2DComponent::GetCollisionMask() const
{
    TB_PROFILE_SCOPE_NAME("Tabby::CapsuleCollider2DComponent::GetCollisionMask");

    return CollisionMask;
}

void CapsuleCollider2DComponent::SetCollisionLayerValue(int layerNumber, bool value)
{
    TB_PROFILE_SCOPE_NAME("Tabby::CapsuleCollider2DComponent::SetCollisionLayerValue");

    TB_CORE_VERIFY_TAGGED(layerNumber >= 1, "Collision layer number must be between 1 and 32 inclusive.");
    TB_CORE_VERIFY_TAGGED(layerNumber <= 32, "Collision layer number must be between 1 and 32 inclusive.");
    if (value) {
        CollisionLayer |= 1 << (layerNumber - 1);
    } else {
        CollisionLayer &= ~(1 << (layerNumber - 1));
    }

    if (B2_IS_NON_NULL(RuntimeShapeId)) {
        b2Filter filter = b2Shape_GetFilter(RuntimeShapeId);
        filter.categoryBits = CollisionLayer;
        b2Shape_SetFilter(RuntimeShapeId, filter);
    }
}

bool CapsuleCollider2DComponent::GetCollisionLayerValue(int layerNumber) const
{
    TB_PROFILE_SCOPE_NAME("Tabby::CapsuleCollider2DComponent::GetCollisionLayerValue");

    TB_CORE_VERIFY_TAGGED(layerNumber >= 1, "Collision layer number must be between 1 and 32 inclusive.");
    TB_CORE_VERIFY_TAGGED(layerNumber <= 32, "Collision layer number must be between 1 and 32 inclusive.");
    return CollisionLayer & (1 << (layerNumber - 1));
}

void CapsuleCollider2DComponent::SetCollisionMaskValue(int layerNumber, bool value)
{
    TB_PROFILE_SCOPE_NAME("Tabby::CapsuleCollider2DComponent::SetCollisionMaskValue");

    TB_CORE_VERIFY_TAGGED(layerNumber >= 1, "Collision layer number must be between 1 and 32 inclusive.");
    TB_CORE_VERIFY_TAGGED(layerNumber <= 32, "Collision layer number must be between 1 and 32 inclusive.");
    if (value) {
        CollisionMask |= 1 << (layerNumber - 1);
    } else {
        CollisionMask &= ~(1 << (layerNumber - 1));
    }

    if (B2_IS_NON_NULL(RuntimeShapeId)) {
        b2Filter filter = b2Shape_GetFilter(RuntimeShapeId);
        filter.maskBits = CollisionMask;
        b2Shape_SetFilter(RuntimeShapeId, filter);
    }
}

bool CapsuleCollider2DComponent::GetCollisionMaskValue(int layerNumber) const
{
    TB_PROFILE_SCOPE_NAME("Tabby::CapsuleCollider2DComponent::GetCollisionMaskValue");

    TB_CORE_VERIFY_TAGGED(layerNumber >= 1, "Collision layer number must be between 1 and 32 inclusive.");
    TB_CORE_VERIFY_TAGGED(layerNumber <= 32, "Collision layer number must be between 1 and 32 inclusive.");
    return CollisionMask & (1 << (layerNumber - 1));
}

void CapsuleCollider2DComponent::RefreshShape()
{
    TB_PROFILE_SCOPE_NAME("Tabby::CapsuleCollider2DComponent::RefreshShape");

    ShapeInfo2D shapeInfo {
        static_cast<ShapeUserData2D*>(b2Shape_GetUserData(RuntimeShapeId))->ShapeEntity,
        ColliderType2D::Circle
    };

    Physics2D::EnqueueShapeUpdate(shapeInfo);
}

// --------------------------------------------------
//
//
//
// ----- SegmentCollider2DComponent -----------------------

void SegmentCollider2DComponent::SetCollisionLayer(uint32_t layer)
{
    TB_PROFILE_SCOPE_NAME("Tabby::CapsuleCollider2DComponent::SetCollisionLayer");

    CollisionLayer = layer;
    if (B2_IS_NON_NULL(RuntimeShapeId)) {
        b2Filter filter = b2Shape_GetFilter(RuntimeShapeId);
        filter.categoryBits = layer;
        b2Shape_SetFilter(RuntimeShapeId, filter);
    }
}

uint32_t SegmentCollider2DComponent::GetCollisionLayer() const
{
    TB_PROFILE_SCOPE_NAME("Tabby::CapsuleCollider2DComponent::GetCollisionLayer");

    return CollisionLayer;
}

void SegmentCollider2DComponent::SetCollisionMask(uint32_t mask)
{
    TB_PROFILE_SCOPE_NAME("Tabby::CapsuleCollider2DComponent::SetCollisionMask");

    CollisionMask = mask;
    if (B2_IS_NON_NULL(RuntimeShapeId)) {
        b2Filter filter = b2Shape_GetFilter(RuntimeShapeId);
        filter.maskBits = mask;
        b2Shape_SetFilter(RuntimeShapeId, filter);
    }
}

uint32_t SegmentCollider2DComponent::GetCollisionMask() const
{
    TB_PROFILE_SCOPE_NAME("Tabby::CapsuleCollider2DComponent::GetCollisionMask");

    return CollisionMask;
}

void SegmentCollider2DComponent::SetCollisionLayerValue(int layerNumber, bool value)
{
    TB_PROFILE_SCOPE_NAME("Tabby::CapsuleCollider2DComponent::SetCollisionLayerValue");

    TB_CORE_VERIFY_TAGGED(layerNumber >= 1, "Collision layer number must be between 1 and 32 inclusive.");
    TB_CORE_VERIFY_TAGGED(layerNumber <= 32, "Collision layer number must be between 1 and 32 inclusive.");
    if (value) {
        CollisionLayer |= 1 << (layerNumber - 1);
    } else {
        CollisionLayer &= ~(1 << (layerNumber - 1));
    }

    if (B2_IS_NON_NULL(RuntimeShapeId)) {
        b2Filter filter = b2Shape_GetFilter(RuntimeShapeId);
        filter.categoryBits = CollisionLayer;
        b2Shape_SetFilter(RuntimeShapeId, filter);
    }
}

bool SegmentCollider2DComponent::GetCollisionLayerValue(int layerNumber) const
{
    TB_PROFILE_SCOPE_NAME("Tabby::CapsuleCollider2DComponent::GetCollisionLayerValue");

    TB_CORE_VERIFY_TAGGED(layerNumber >= 1, "Collision layer number must be between 1 and 32 inclusive.");
    TB_CORE_VERIFY_TAGGED(layerNumber <= 32, "Collision layer number must be between 1 and 32 inclusive.");
    return CollisionLayer & (1 << (layerNumber - 1));
}

void SegmentCollider2DComponent::SetCollisionMaskValue(int layerNumber, bool value)
{
    TB_PROFILE_SCOPE_NAME("Tabby::CapsuleCollider2DComponent::SetCollisionMaskValue");

    TB_CORE_VERIFY_TAGGED(layerNumber >= 1, "Collision layer number must be between 1 and 32 inclusive.");
    TB_CORE_VERIFY_TAGGED(layerNumber <= 32, "Collision layer number must be between 1 and 32 inclusive.");
    if (value) {
        CollisionMask |= 1 << (layerNumber - 1);
    } else {
        CollisionMask &= ~(1 << (layerNumber - 1));
    }

    if (B2_IS_NON_NULL(RuntimeShapeId)) {
        b2Filter filter = b2Shape_GetFilter(RuntimeShapeId);
        filter.maskBits = CollisionMask;
        b2Shape_SetFilter(RuntimeShapeId, filter);
    }
}

bool SegmentCollider2DComponent::GetCollisionMaskValue(int layerNumber) const
{
    TB_PROFILE_SCOPE_NAME("Tabby::CapsuleCollider2DComponent::GetCollisionMaskValue");

    TB_CORE_VERIFY_TAGGED(layerNumber >= 1, "Collision layer number must be between 1 and 32 inclusive.");
    TB_CORE_VERIFY_TAGGED(layerNumber <= 32, "Collision layer number must be between 1 and 32 inclusive.");
    return CollisionMask & (1 << (layerNumber - 1));
}

void SegmentCollider2DComponent::RefreshShape()
{
    TB_PROFILE_SCOPE_NAME("Tabby::CapsuleCollider2DComponent::RefreshShape");

    ShapeInfo2D shapeInfo {
        static_cast<ShapeUserData2D*>(b2Shape_GetUserData(RuntimeShapeId))->ShapeEntity,
        ColliderType2D::Segment
    };

    Physics2D::EnqueueShapeUpdate(shapeInfo);
}

}
