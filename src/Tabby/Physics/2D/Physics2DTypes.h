#pragma once
#include <Tabby/World/Entity.h>

#include <box2d/box2d.h>

namespace Tabby {

struct TransformComponent;
struct Rigidbody2DComponent;

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
    Circle,
    Capsule,
    Polygon,
    Segment
};

struct ShapeUserData2D {
    Entity ShapeEntity;
    Entity BodyEntity;
    ColliderType2D ColliderType;
};

struct BodyUserData2D {
    Entity BodyEntity;
    std::unordered_map<UUID, Entity> ShapeEntities;
};

struct Collision {

    // Entity collided with
    Entity CollidedEntity;
    // Entity transform collided with
    TransformComponent* CollidedEntityTransform;
    // Entity Rigidbody2DComponent collided with
    Rigidbody2DComponent* CollidedEntityRigidbody;

    // Collider data that collided which is part of this body (i hope you get it)
    ShapeUserData2D HostColliderData;
    // Collider data of the entiti we collided with
    ShapeUserData2D CollidedEntitysColliderData;
};

struct BodyInfo2D {
    Entity BodyEntity;
};

struct ShapeInfo2D {
    Entity ShapeEntity;
    ColliderType2D ColliderType;
};

struct RaycastHit2D {
    RaycastFilter2D RaycastFilter;
    Entity HitEntity;
    TransformComponent* Transform;
    Rigidbody2DComponent* Rigidbody;
    Vector2 Origin = { 0.0f, 0.0f };
    Vector2 Point = { 0.0f, 0.0f };
    Vector2 Normal = { 0.0f, 0.0f };
    float Distance;
    float Fraction;

    operator bool() { return HitEntity.Valid(); }
};

}
