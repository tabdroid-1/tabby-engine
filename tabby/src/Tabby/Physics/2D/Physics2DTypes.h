#pragma once

#include <Tabby/Scene/Components.h>
#include <Tabby/Scene/Entity.h>

namespace Tabby {

namespace Utils {

    inline b2BodyType Rigidbody2DTypeToBox2DBody(Rigidbody2DComponent::BodyType bodyType)
    {
        switch (bodyType) {
        case Rigidbody2DComponent::BodyType::Static:
            return b2_staticBody;
        case Rigidbody2DComponent::BodyType::Dynamic:
            return b2_dynamicBody;
        case Rigidbody2DComponent::BodyType::Kinematic:
            return b2_kinematicBody;
        }

        TB_CORE_ASSERT(false, "Unknown body type");
        return b2_staticBody;
    }

    inline Rigidbody2DComponent::BodyType Rigidbody2DTypeFromBox2DBody(b2BodyType bodyType)
    {
        switch (bodyType) {
        case b2_staticBody:
            return Rigidbody2DComponent::BodyType::Static;
        case b2_dynamicBody:
            return Rigidbody2DComponent::BodyType::Dynamic;
        case b2_kinematicBody:
            return Rigidbody2DComponent::BodyType::Kinematic;
        }

        TB_CORE_ASSERT(false, "Unknown body type");
        return Rigidbody2DComponent::BodyType::Static;
    }

}

struct RaycastFilter2D {
public:
    RaycastFilter2D()
        : collisionLayer(0x00000001)
        , collisionMask(0xFFFFFFFF)
    {
    }

    RaycastFilter2D(uint32_t layer, uint32_t mask)
        : collisionLayer(layer)
        , collisionMask(mask)
    {
    }

    void SetCollisionLayer(uint32_t layer);
    uint32_t GetCollisionLayer() const;

    void SetCollisionMask(uint32_t layer);
    uint32_t GetCollisionMask() const;

    void SetLayerValue(int layerNumber, bool value);
    bool GetLayerValue(int layerNumber) const;

    void SetMaskValue(int layerNumber, bool value);
    bool GetMaskValue(int layerNumber) const;

private:
    uint32_t collisionLayer = 1;
    uint32_t collisionMask = 1;
};

enum ColliderType2D : uint8_t {
    Box = 0,
    Circle = 1,
    Capsule = 2,
    Segment = 3
};

struct ShapeUserData2D {
    Entity shapeEntity;
    Entity bodyEntity;
    ColliderType2D colliderType;
};

struct BodyUserData2D {
    Entity bodyEntity;
    std::unordered_map<UUID, Entity> shapeEntities;
};

struct ContactCallback {
    Entity entity;
    Tabby::TransformComponent* transform;
    Tabby::Rigidbody2DComponent* rigidbody;
};

struct BodyInfo2D {
    Entity entity;
};

struct ShapeInfo2D {
    Entity entity;
    ColliderType2D colliderType;
};

struct RaycastHit2D {
    RaycastFilter2D RaycastFilter;
    Entity entity;
    Tabby::TransformComponent* transform;
    Tabby::Rigidbody2DComponent* rigidbody;
    glm::vec2 origin = { 0.0f, 0.0f };
    glm::vec2 point = { 0.0f, 0.0f };
    glm::vec2 normal = { 0.0f, 0.0f };
    float distance;
    float fraction;

    operator bool() { return entity.Valid(); }
};

}
