#pragma once

#include "SceneCamera.h"
#include "Tabby/Core/UUID.h"
#include "Tabby/Renderer/Font.h"

#include <box2d/box2d.h>
#include "entt/entt.hpp"

namespace Tabby {

struct IDComponent {
    UUID ID;
    bool IsPersistent = false;

    IDComponent() = default;
    IDComponent(const IDComponent&) = default;
};

struct TagComponent {
    std::string Tag;

    TagComponent() = default;
    TagComponent(const TagComponent&) = default;
    TagComponent(const std::string& tag)
        : Tag(tag)
    {
    }
};

struct HierarchyNodeComponent {
    std::pair<UUID, entt::entity> Parent;
    std::vector<std::pair<UUID, entt::entity>> Children;

    HierarchyNodeComponent() = default;
};

struct TransformComponent {
    glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
    glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
    glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

    glm::vec3 LocalTranslation = { 0.0f, 0.0f, 0.0f };
    glm::vec3 LocalRotation = { 0.0f, 0.0f, 0.0f };
    glm::vec3 LocalScale = { 1.0f, 1.0f, 1.0f };

    TransformComponent() = default;
    TransformComponent(const TransformComponent&) = default;
    TransformComponent(const glm::vec3& translation)
        : Translation(translation)
    {
    }

    glm::mat4 GetTransform() const;
    glm::mat4 GetLocalTransform() const;
    void ApplyTransform(const glm::mat4& transform);
};

struct SpriteRendererComponent {
    glm::vec4 Color { 1.0f, 1.0f, 1.0f, 1.0f };
    AssetHandle Texture;
    int renderOrder = 0;

    int hFrames = 1;
    int vFrames = 1;
    int xFrame = 0;
    int yFrame = 0;
    float TilingFactor = 1.0f;

    SpriteRendererComponent() = default;
    SpriteRendererComponent(const SpriteRendererComponent&) = default;
    SpriteRendererComponent(const glm::vec4& color)
        : Color(color)
    {
    }
};

struct CircleRendererComponent {
    glm::vec4 Color { 1.0f, 1.0f, 1.0f, 1.0f };
    float Thickness = 1.0f;
    float Fade = 0.005f;

    int renderOrder = 0;

    CircleRendererComponent() = default;
    CircleRendererComponent(const CircleRendererComponent&) = default;
};

struct CameraComponent {
    SceneCamera Camera;
    bool Primary = true; // TODO: think about moving to Scene
    bool FixedAspectRatio = false;

    CameraComponent() = default;
    CameraComponent(const CameraComponent&) = default;
};

// class AudioSource;

struct SoundComponent {
    // Shared<AudioSource> Sound;
    float Gain = 1.0f;
    bool Playing = false;
    bool Loop = false;

    SoundComponent() = default;
    SoundComponent(const SoundComponent&) = default;
};

// Forward declaration
class ScriptableEntity;

struct NativeScriptComponent {
    ScriptableEntity* Instance = nullptr;

    ScriptableEntity* (*InstantiateScript)();
    void (*DestroyScript)(NativeScriptComponent*);

    template <typename T>
    void Bind()
    {
        InstantiateScript = []() {
            return static_cast<ScriptableEntity*>(new T());
        };
        DestroyScript = [](NativeScriptComponent* nsc) {
            delete nsc->Instance;
            nsc->Instance = nullptr;
        };
    }
};

// Physics

struct Rigidbody2DComponent {
    enum class BodyType { Static = 0,
        Dynamic,
        Kinematic };
    BodyType Type = BodyType::Dynamic;
    bool FixedRotation = false;

    // Storage for runtime
    b2BodyId RuntimeBodyId = b2_nullBodyId;
    bool queuedForInitialization;

    Rigidbody2DComponent() = default;
    Rigidbody2DComponent(const Rigidbody2DComponent&) = default;

    void SetFixedRotation(bool enable);
    void SetVelocity(glm::vec2 velocity);
    void SetAngularVelocity(float velocity);

    glm::vec2 GetVelocity() const;
    glm::vec2 GetPosition() const;
    float GetAngularVelocity() const;
    float GetAngle() const;
};

struct BoxCollider2DComponent {

    glm::vec2 Offset = { 0.0f, 0.0f };
    glm::vec2 Size = { 0.5f, 0.5f };
    float angle = 0.0f;

    // TODO(Yan): move into physics material in the future maybe
    float Density = 1.0f;
    float Friction = 0.5f;
    float Restitution = 0.0f;
    float RestitutionThreshold = 0.5f;

    /// A sensor shape collects contact information but never generates a
    /// collision response.
    bool isSensor = false;

    /// Enable sensor events for this shape. Only applies to kinematic and dynamic
    /// bodies. Ignored for sensors.
    bool enableSensorEvents = false;

    /// Enable contact events for this shape. Only applies to kinematic and
    /// dynamic bodies. Ignored for sensors.
    bool enableContactEvents = false;

    /// Enable pre-solve contact events for this shape. Only applies to dynamic
    /// bodies. These are expensive
    ///    and must be carefully handled due to multi-threading. Ignored for
    ///    sensors.
    bool enablePreSolveEvents = false;

    // Storage for runtime
    b2ShapeId RuntimeShapeId = b2_nullShapeId;
    b2WorldId worldId = b2_nullWorldId;
    bool queuedForInitialization;

    BoxCollider2DComponent() = default;
    BoxCollider2DComponent(const BoxCollider2DComponent&) = default;

    void RefreshShape();
};

struct CircleCollider2DComponent {
    glm::vec2 Offset = { 0.0f, 0.0f };
    float Radius = 0.5f;

    // TODO(Yan): move into physics material in the future maybe
    float Density = 1.0f;
    float Friction = 0.5f;
    float Restitution = 0.0f;
    float RestitutionThreshold = 0.5f;

    /// A sensor shape collects contact information but never generates a
    /// collision response.
    bool isSensor = false;

    /// Enable sensor events for this shape. Only applies to kinematic and dynamic
    /// bodies. Ignored for sensors.
    bool enableSensorEvents = false;

    /// Enable contact events for this shape. Only applies to kinematic and
    /// dynamic bodies. Ignored for sensors.
    bool enableContactEvents = false;

    /// Enable pre-solve contact events for this shape. Only applies to dynamic
    /// bodies. These are expensive
    ///    and must be carefully handled due to multi-threading. Ignored for
    ///    sensors.
    bool enablePreSolveEvents = false;

    // Storage for runtime
    b2ShapeId RuntimeShapeId = b2_nullShapeId;
    b2WorldId worldId = b2_nullWorldId;
    bool queuedForInitialization;

    CircleCollider2DComponent() = default;
    CircleCollider2DComponent(const CircleCollider2DComponent&) = default;

    void RefreshShape();
};

struct CapsuleCollider2DComponent {
    glm::vec2 Offset = { 0.0f, 0.0f };
    glm::vec2 center1, center2;
    float Radius = 0.5f;

    // TODO(Yan): move into physics material in the future maybe
    float Density = 1.0f;
    float Friction = 0.5f;
    float Restitution = 0.0f;
    float RestitutionThreshold = 0.5f;

    /// A sensor shape collects contact information but never generates a
    /// collision response.
    bool isSensor = false;

    /// Enable sensor events for this shape. Only applies to kinematic and dynamic
    /// bodies. Ignored for sensors.
    bool enableSensorEvents = false;

    /// Enable contact events for this shape. Only applies to kinematic and
    /// dynamic bodies. Ignored for sensors.
    bool enableContactEvents = false;

    /// Enable pre-solve contact events for this shape. Only applies to dynamic
    /// bodies. These are expensive
    ///    and must be carefully handled due to multi-threading. Ignored for
    ///    sensors.
    bool enablePreSolveEvents = false;

    // Storage for runtime
    b2ShapeId RuntimeShapeId = b2_nullShapeId;
    b2WorldId worldId = b2_nullWorldId;
    bool queuedForInitialization;

    CapsuleCollider2DComponent() = default;
    CapsuleCollider2DComponent(const CapsuleCollider2DComponent&) = default;

    void RefreshShape();
};

struct SegmentCollider2DComponent {
    glm::vec2 Offset = { 0.0f, 0.0f };
    glm::vec2 point1, point2;

    // TODO(Yan): move into physics material in the future maybe
    float Density = 1.0f;
    float Friction = 0.5f;
    float Restitution = 0.0f;
    float RestitutionThreshold = 0.5f;

    /// A sensor shape collects contact information but never generates a
    /// collision response.
    bool isSensor = false;

    /// Enable sensor events for this shape. Only applies to kinematic and dynamic
    /// bodies. Ignored for sensors.
    bool enableSensorEvents = false;

    /// Enable contact events for this shape. Only applies to kinematic and
    /// dynamic bodies. Ignored for sensors.
    bool enableContactEvents = false;

    /// Enable pre-solve contact events for this shape. Only applies to dynamic
    /// bodies. These are expensive
    ///    and must be carefully handled due to multi-threading. Ignored for
    ///    sensors.
    bool enablePreSolveEvents = false;

    // Storage for runtime
    b2ShapeId RuntimeShapeId = b2_nullShapeId;
    b2WorldId worldId = b2_nullWorldId;
    bool queuedForInitialization;

    SegmentCollider2DComponent() = default;
    SegmentCollider2DComponent(const SegmentCollider2DComponent&) = default;

    void RefreshShape();
};

struct TextComponent {
    std::string TextString;

    Shared<Font> font = Font::GetDefault();
    // AssetHandle Font;
    glm::vec4 Color { 1.0f };
    float Kerning = 0.0f;
    float LineSpacing = 0.0f;

    int renderOrder = 0;
};

template <typename... Component>
struct ComponentGroup { };

using AllComponents = ComponentGroup<TransformComponent, SpriteRendererComponent,
    CircleRendererComponent, CameraComponent, SoundComponent, HierarchyNodeComponent, NativeScriptComponent,
    Rigidbody2DComponent, BoxCollider2DComponent, CircleCollider2DComponent, CapsuleCollider2DComponent, TextComponent>;

} // namespace Tabby

// #pragma once
//
// #include "SceneCamera.h"
// #include "Tabby/Core/UUID.h"
// #include "Tabby/Renderer/Font.h"
//
// #include <box2d/box2d.h>
// #include "entt/entt.hpp"
// #include "glm/fwd.hpp"
// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include <Tabby/Math/Math.h>
//
// #define GLM_ENABLE_EXPERIMENTAL
// #include <glm/gtx/matrix_decompose.hpp>
// #include <glm/gtx/quaternion.hpp>
//
// namespace Tabby {
//
// struct IDComponent {
//     UUID ID;
//     bool IsPersistent = false;
//
//     IDComponent() = default;
//     IDComponent(const IDComponent&) = default;
// };
//
// struct TagComponent {
//     std::string Tag;
//
//     TagComponent() = default;
//     TagComponent(const TagComponent&) = default;
//     TagComponent(const std::string& tag)
//         : Tag(tag)
//     {
//     }
// };
//
// struct HierarchyNodeComponent {
//     std::pair<UUID, entt::entity> Parent;
//     std::vector<std::pair<UUID, entt::entity>> Children;
//
//     HierarchyNodeComponent() = default;
// };
//
// struct TransformComponent {
//     glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
//     glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
//     glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };
//
//     glm::vec3 LocalTranslation = { 0.0f, 0.0f, 0.0f };
//     glm::vec3 LocalRotation = { 0.0f, 0.0f, 0.0f };
//     glm::vec3 LocalScale = { 1.0f, 1.0f, 1.0f };
//
//     TransformComponent() = default;
//     TransformComponent(const TransformComponent&) = default;
//     TransformComponent(const glm::vec3& translation)
//         : Translation(translation)
//     {
//     }
//
//     glm::mat4 GetTransform() const
//     {
//         glm::mat4 rotation = glm::toMat4(glm::quat(glm::radians(Rotation)));
//
//         return glm::translate(glm::mat4(1.0f), Translation) * rotation * glm::scale(glm::mat4(1.0f), Scale);
//     }
//
//     glm::mat4 GetLocalTransform() const
//     {
//         glm::mat4 rotation = glm::toMat4(glm::quat(glm::radians(LocalRotation)));
//
//         return glm::translate(glm::mat4(1.0f), LocalTranslation) * rotation * glm::scale(glm::mat4(1.0f), LocalScale);
//     }
//
//     void ApplyTransform(const glm::mat4& transform)
//     {
//
//         using namespace glm;
//         using T = float;
//
//         mat4 LocalMatrix(transform);
//
//         // Normalize the matrix.
//         if (epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), epsilon<T>()))
//             return;
//
//         // First, isolate perspective.  This is the messiest.
//         if (epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) || epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) || epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>())) {
//             // Clear the perspective partition
//             LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
//             LocalMatrix[3][3] = static_cast<T>(1);
//         }
//
//         // Next take care of translation (easy).
//         Translation = vec3(LocalMatrix[3]);
//         LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);
//
//         vec3 Row[3], Pdum3;
//
//         // Now get scale and shear.
//         for (length_t i = 0; i < 3; ++i)
//             for (length_t j = 0; j < 3; ++j)
//                 Row[i][j] = LocalMatrix[i][j];
//
//         // Compute X scale factor and normalize first row.
//         Scale.x = length(Row[0]);
//         Row[0] = detail::scale(Row[0], static_cast<T>(1));
//         Scale.y = length(Row[1]);
//         Row[1] = detail::scale(Row[1], static_cast<T>(1));
//         Scale.z = length(Row[2]);
//         Row[2] = detail::scale(Row[2], static_cast<T>(1));
//
//         // At this point, the matrix (in rows[]) is orthonormal.
//         // Check for a coordinate system flip.  If the determinant
//         // is -1, then negate the matrix and the scaling factors.
// #if 0
//         		Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
//         		if (dot(Row[0], Pdum3) < 0)
//         		{
//         			for (length_t i = 0; i < 3; i++)
//         			{
//         				scale[i] *= static_cast<T>(-1);
//         				Row[i] *= static_cast<T>(-1);
//         			}
//         		}
// #endif
//
//         Rotation.y = asin(-Row[0][2]);
//         if (cos(Rotation.y) != 0) {
//             Rotation.x = atan2(Row[1][2], Row[2][2]);
//             Rotation.z = atan2(Row[0][1], Row[0][0]);
//         } else {
//             Rotation.x = atan2(-Row[2][0], Row[1][1]);
//             Rotation.z = 0;
//         }
//
//         Rotation.x *= Math::RAD2DEG;
//         Rotation.y *= Math::RAD2DEG;
//         Rotation.z *= Math::RAD2DEG;
//     }
// };
//
// struct SpriteRendererComponent {
//     glm::vec4 Color { 1.0f, 1.0f, 1.0f, 1.0f };
//     AssetHandle Texture;
//     int renderOrder = 0;
//
//     int hFrames = 1;
//     int vFrames = 1;
//     int xFrame = 0;
//     int yFrame = 0;
//     float TilingFactor = 1.0f;
//
//     SpriteRendererComponent() = default;
//     SpriteRendererComponent(const SpriteRendererComponent&) = default;
//     SpriteRendererComponent(const glm::vec4& color)
//         : Color(color)
//     {
//     }
// };
//
// struct CircleRendererComponent {
//     glm::vec4 Color { 1.0f, 1.0f, 1.0f, 1.0f };
//     float Thickness = 1.0f;
//     float Fade = 0.005f;
//
//     int renderOrder = 0;
//
//     CircleRendererComponent() = default;
//     CircleRendererComponent(const CircleRendererComponent&) = default;
// };
//
// struct CameraComponent {
//     SceneCamera Camera;
//     bool Primary = true; // TODO: think about moving to Scene
//     bool FixedAspectRatio = false;
//
//     CameraComponent() = default;
//     CameraComponent(const CameraComponent&) = default;
// };
//
// // class AudioSource;
//
// struct SoundComponent {
//     // Shared<AudioSource> Sound;
//     float Gain = 1.0f;
//     bool Playing = false;
//     bool Loop = false;
//
//     SoundComponent() = default;
//     SoundComponent(const SoundComponent&) = default;
// };
//
// // Forward declaration
// class ScriptableEntity;
//
// struct NativeScriptComponent {
//     ScriptableEntity* Instance = nullptr;
//
//     ScriptableEntity* (*InstantiateScript)();
//     void (*DestroyScript)(NativeScriptComponent*);
//
//     template <typename T>
//     void Bind()
//     {
//         InstantiateScript = []() {
//             return static_cast<ScriptableEntity*>(new T());
//         };
//         DestroyScript = [](NativeScriptComponent* nsc) {
//             delete nsc->Instance;
//             nsc->Instance = nullptr;
//         };
//     }
// };
//
// // Physics
//
// struct Rigidbody2DComponent {
//     enum class BodyType { Static = 0,
//         Dynamic,
//         Kinematic };
//     BodyType Type = BodyType::Dynamic;
//     bool FixedRotation = false;
//
//     // Storage for runtime
//     b2BodyId RuntimeBodyId = b2_nullBodyId;
//     bool queuedForInitialization;
//
//     Rigidbody2DComponent() = default;
//     Rigidbody2DComponent(const Rigidbody2DComponent&) = default;
//
//     // void SetType(BodyType bodyType)
//     // {
//     //
//     //     b2BodyType type;
//     //     switch (bodyType) {
//     //     case BodyType::Static:
//     //         type = b2_staticBody;
//     //         break;
//     //     case BodyType::Dynamic:
//     //         type = b2_dynamicBody;
//     //         break;
//     //     case BodyType::Kinematic:
//     //         type = b2_kinematicBody;
//     //         break;
//     //     default:
//     //         type = b2_dynamicBody;
//     //         break;
//     //     }
//     //
//     //     static_cast<b2Body*>(RuntimeBody)->SetType(type);
//     //     Type = bodyType;
//     // }
//
//     // void SetFixedRotation(bool enable)
//     // {
//     //     static_cast<b2Body*>(RuntimeBody)->SetFixedRotation(enable);
//     //     FixedRotation = enable;
//     // }
//
//     void SetVelocity(glm::vec2 velocity)
//     {
//         if (B2_IS_NON_NULL(RuntimeBodyId))
//             b2Body_SetLinearVelocity(RuntimeBodyId, { velocity.x, velocity.y });
//         // static_cast<b2Body*>(RuntimeBody)->SetLinearVelocity({ velocity.x,
//         // velocity.y });
//     }
//
//     void SetAngularVelocity(float velocity)
//     {
//         if (B2_IS_NON_NULL(RuntimeBodyId))
//             b2Body_SetAngularVelocity(RuntimeBodyId, velocity);
//         // static_cast<b2Body*>(RuntimeBody)->SetAngularVelocity(velocity);
//     }
//
//     glm::vec2 GetVelocity() const
//     {
//         if (B2_IS_NON_NULL(RuntimeBodyId)) {
//
//             b2Vec2 velocity = b2Body_GetLinearVelocity(RuntimeBodyId);
//             return { velocity.x, velocity.y };
//         } else
//             return { 0.0f, 0.0f };
//     }
//
//     float GetAngularVelocity() const
//     {
//         if (B2_IS_NON_NULL(RuntimeBodyId))
//             return b2Body_GetAngularVelocity(RuntimeBodyId);
//         else
//             return 0.0f;
//     }
//
//     float GetAngle() const
//     {
//         float angleRadians;
//
//         if (B2_IS_NON_NULL(RuntimeBodyId))
//             angleRadians = b2Body_GetAngle(RuntimeBodyId);
//
//         // Convert the angle from radians to degrees
//         float angleDegrees = angleRadians * (180.0f / b2_pi);
//
//         // Wrap the angle to the range [0, 360]
//         angleDegrees = std::fmod(angleDegrees, 360.0f);
//
//         // If the angle is negative, add 360 to make it positive
//         if (angleDegrees < 0.0f) {
//             angleDegrees += 360.0f;
//         }
//
//         return angleDegrees;
//     }
// };
//
// struct BoxCollider2DComponent {
//     std::pair<UUID, entt::entity> AtachedRigidbody2DEntity;
//
//     glm::vec2 Offset = { 0.0f, 0.0f };
//     glm::vec2 Size = { 0.5f, 0.5f };
//     float angle = 0.0f;
//
//     // TODO(Yan): move into physics material in the future maybe
//     float Density = 1.0f;
//     float Friction = 0.5f;
//     float Restitution = 0.0f;
//     float RestitutionThreshold = 0.5f;
//
//     /// A sensor shape collects contact information but never generates a
//     /// collision response.
//     bool isSensor = false;
//
//     /// Enable sensor events for this shape. Only applies to kinematic and dynamic
//     /// bodies. Ignored for sensors.
//     bool enableSensorEvents = false;
//
//     /// Enable contact events for this shape. Only applies to kinematic and
//     /// dynamic bodies. Ignored for sensors.
//     bool enableContactEvents = false;
//
//     /// Enable pre-solve contact events for this shape. Only applies to dynamic
//     /// bodies. These are expensive
//     ///    and must be carefully handled due to multi-threading. Ignored for
//     ///    sensors.
//     bool enablePreSolveEvents = false;
//
//     // Storage for runtime
//     b2ShapeId RuntimeShapeId = b2_nullShapeId;
//     bool queuedForInitialization;
//
//     BoxCollider2DComponent() = default;
//     BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
// };
//
// struct CircleCollider2DComponent {
//     std::pair<UUID, entt::entity> AtachedRigidbody2DEntity;
//
//     glm::vec2 Offset = { 0.0f, 0.0f };
//     float Radius = 0.5f;
//
//     // TODO(Yan): move into physics material in the future maybe
//     float Density = 1.0f;
//     float Friction = 0.5f;
//     float Restitution = 0.0f;
//     float RestitutionThreshold = 0.5f;
//
//     /// A sensor shape collects contact information but never generates a
//     /// collision response.
//     bool isSensor = false;
//
//     /// Enable sensor events for this shape. Only applies to kinematic and dynamic
//     /// bodies. Ignored for sensors.
//     bool enableSensorEvents = false;
//
//     /// Enable contact events for this shape. Only applies to kinematic and
//     /// dynamic bodies. Ignored for sensors.
//     bool enableContactEvents = false;
//
//     /// Enable pre-solve contact events for this shape. Only applies to dynamic
//     /// bodies. These are expensive
//     ///    and must be carefully handled due to multi-threading. Ignored for
//     ///    sensors.
//     bool enablePreSolveEvents = false;
//
//     // Storage for runtime
//     b2ShapeId RuntimeShapeId = b2_nullShapeId;
//     bool queuedForInitialization;
//
//     CircleCollider2DComponent() = default;
//     CircleCollider2DComponent(const CircleCollider2DComponent&) = default;
// };
//
// struct CapsuleCollider2DComponent {
//     std::pair<UUID, entt::entity> AtachedRigidbody2DEntity;
//
//     glm::vec2 center1, center2;
//     float Radius = 0.5f;
//
//     // TODO(Yan): move into physics material in the future maybe
//     float Density = 1.0f;
//     float Friction = 0.5f;
//     float Restitution = 0.0f;
//     float RestitutionThreshold = 0.5f;
//
//     /// A sensor shape collects contact information but never generates a
//     /// collision response.
//     bool isSensor = false;
//
//     /// Enable sensor events for this shape. Only applies to kinematic and dynamic
//     /// bodies. Ignored for sensors.
//     bool enableSensorEvents = false;
//
//     /// Enable contact events for this shape. Only applies to kinematic and
//     /// dynamic bodies. Ignored for sensors.
//     bool enableContactEvents = false;
//
//     /// Enable pre-solve contact events for this shape. Only applies to dynamic
//     /// bodies. These are expensive
//     ///    and must be carefully handled due to multi-threading. Ignored for
//     ///    sensors.
//     bool enablePreSolveEvents = false;
//
//     // Storage for runtime
//     b2ShapeId RuntimeShapeId = b2_nullShapeId;
//     bool queuedForInitialization;
//
//     CapsuleCollider2DComponent() = default;
//     CapsuleCollider2DComponent(const CapsuleCollider2DComponent&) = default;
// };
//
// struct TextComponent {
//     std::string TextString;
//
//     Shared<Font> font = Font::GetDefault();
//     // AssetHandle Font;
//     glm::vec4 Color { 1.0f };
//     float Kerning = 0.0f;
//     float LineSpacing = 0.0f;
//
//     int renderOrder = 0;
// };
//
// template <typename... Component>
// struct ComponentGroup { };
//
// using AllComponents = ComponentGroup<TransformComponent, SpriteRendererComponent,
//     CircleRendererComponent, CameraComponent, SoundComponent, HierarchyNodeComponent, NativeScriptComponent,
//     Rigidbody2DComponent, BoxCollider2DComponent, CircleCollider2DComponent, CapsuleCollider2DComponent, TextComponent>;
//
// } // namespace Tabby
