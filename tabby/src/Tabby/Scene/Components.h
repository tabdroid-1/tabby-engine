#pragma once

#include <Tabby/Core/UUID.h>
#include <Tabby/Renderer/Camera.h>
#include <Tabby/Renderer/Font.h>

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
    Camera Camera;
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

    uint32_t collisionLayer = 1;
    uint32_t collisionMask = 1;

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

    void SetCollisionLayer(uint32_t layer);
    uint32_t GetCollisionLayer() const;

    void SetCollisionMask(uint32_t mask);
    uint32_t GetCollisionMask() const;

    void SetCollisionLayerValue(int layerNumber, bool value);
    bool GetCollisionLayerValue(int layerNumber) const;

    void SetCollisionMaskValue(int layerNumber, bool value);
    bool GetCollisionMaskValue(int layerNumber) const;

    void RefreshShape();
};

struct CircleCollider2DComponent {
    glm::vec2 Offset = { 0.0f, 0.0f };
    float Radius = 0.5f;

    uint32_t collisionLayer = 1;
    uint32_t collisionMask = 1;

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

    void SetCollisionLayer(uint32_t layer);
    uint32_t GetCollisionLayer() const;

    void SetCollisionMask(uint32_t mask);
    uint32_t GetCollisionMask() const;

    void SetCollisionLayerValue(int layerNumber, bool value);
    bool GetCollisionLayerValue(int layerNumber) const;

    void SetCollisionMaskValue(int layerNumber, bool value);
    bool GetCollisionMaskValue(int layerNumber) const;

    void RefreshShape();
};

struct CapsuleCollider2DComponent {
    glm::vec2 Offset = { 0.0f, 0.0f };
    glm::vec2 center1, center2;
    float Radius = 0.5f;

    uint32_t collisionLayer = 1;
    uint32_t collisionMask = 1;

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

    void SetCollisionLayer(uint32_t layer);
    uint32_t GetCollisionLayer() const;

    void SetCollisionMask(uint32_t mask);
    uint32_t GetCollisionMask() const;

    void SetCollisionLayerValue(int layerNumber, bool value);
    bool GetCollisionLayerValue(int layerNumber) const;

    void SetCollisionMaskValue(int layerNumber, bool value);
    bool GetCollisionMaskValue(int layerNumber) const;

    void RefreshShape();
};

struct SegmentCollider2DComponent {
    glm::vec2 Offset = { 0.0f, 0.0f };
    glm::vec2 point1, point2;

    uint32_t collisionLayer = 1;
    uint32_t collisionMask = 1;

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

    void SetCollisionLayer(uint32_t layer);
    uint32_t GetCollisionLayer() const;

    void SetCollisionMask(uint32_t mask);
    uint32_t GetCollisionMask() const;

    void SetCollisionLayerValue(int layerNumber, bool value);
    bool GetCollisionLayerValue(int layerNumber) const;

    void SetCollisionMaskValue(int layerNumber, bool value);
    bool GetCollisionMaskValue(int layerNumber) const;

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
