#pragma once

#include "box2d/b2_body.h"
#include <Core/Animation.h>
#include <Scene/ScriptableGameObject.h>
#include <raylib.h>
#include <raymath.h>

namespace Tabby {

struct IDComponent {
    UUID ID;
    IDComponent() = default;
    IDComponent(const IDComponent&) = default;
};

struct TagComponent {
    std::string Tag = "UnTagged";
    std::string Layer = "Default";
    bool IsPersistent = false;

    TagComponent() = default;
    TagComponent(const TagComponent&) = default;
    TagComponent(const std::string& tag)
        : Tag(tag)
    {
    }
    TagComponent(const std::string& tag, const std::string& layer)
        : Tag(tag)
        , Layer(layer)
    {
    }
    TagComponent(const std::string& tag, const std::string& layer, bool isPersistent)
        : Tag(tag)
        , Layer(layer)
        , IsPersistent(isPersistent)
    {
    }
};

struct TransformComponent {
    Vector3 Position = { 0.0f, 0.0f };
    Vector3 Scale = { 1.0f, 1.0f, 1.0f };
    Vector3 Rotation = { 0.0f, 0.0f, 0.0f };
    Vector3 Size = { 32.0f, 32.0f, 32.0f };
    Vector3 Origin = { 1.0f, 1.0f, 1.0f };

    TransformComponent() = default;
    TransformComponent(const TransformComponent&) = default;
    TransformComponent(const Vector3& position)
        : Position(position)
    {
    }

    Matrix GetTransform() const
    {
        Matrix mat = MatrixIdentity();

        mat = MatrixMultiply(mat, MatrixTranslate(Position.x, Position.y, Position.z));
        mat = MatrixMultiply(mat, MatrixRotate(Vector3 { 1.0f, 0.0f, 0.0f }, Rotation.x));
        mat = MatrixMultiply(mat, MatrixRotate(Vector3 { 0.0f, 1.0f, 0.0f }, Rotation.y));
        mat = MatrixMultiply(mat, MatrixRotate(Vector3 { 0.0f, 0.0f, 1.0f }, Rotation.z));
        mat = MatrixMultiply(mat, MatrixScale(Scale.x, Scale.y, Scale.y));

        return mat;
    }
};

struct SpriteRendererComponent {
    Color Tint = WHITE;
    Texture2D Texture;
    Rectangle srcRec = { 0, 0, 50, 50 };
    Rectangle destRec;
    Vector2 origin;

    SpriteRendererComponent() = default;
    SpriteRendererComponent(const SpriteRendererComponent&) = default;
    SpriteRendererComponent(const Color tint)
        : Tint(tint)
    {
    }

    void FlipTextureX()
    {
        Image image = LoadImageFromTexture(Texture);
        ImageFlipHorizontal(&image);
        UpdateTexture(Texture, image.data);
    }

    void FlipTextureY()
    {
        Image image = LoadImageFromTexture(Texture);
        ImageFlipVertical(&image);
        UpdateTexture(Texture, image.data);
    }
};

struct AnimationComponent {

    std::shared_ptr<SpriteRendererComponent> sprite;
    std::map<std::string, std::shared_ptr<Animation>> animations;
    std::pair<std::string, std::shared_ptr<Animation>> currentAnimation;
    Texture currentTexture;

    AnimationComponent() = default;
    AnimationComponent(const AnimationComponent&) = default;

    void AddAnimation(std::string Name, std::shared_ptr<Animation> Animation)
    {
        auto inserted = animations.insert(std::make_pair(Name, Animation));

        if (currentAnimation.first == "null") {
            PlayAnimation(Name);
        }
    }
    void PlayAnimation(const std::string& Name)
    {
        if (currentAnimation.first == Name) {
            return;
        }

        auto animation = animations.find(Name);
        if (animation != animations.end()) {
            currentAnimation.first = animation->first;
            currentAnimation.second = animation->second;

            // owner->GetComponent<C_Sprite>()->Load(animation->second->GetCurrentFrame()->texturePath, true);
            currentTexture = LoadTexture(animation->second->GetCurrentFrame()->texturePath.c_str());
            currentAnimation.second->Reset();
        }
    }
    const std::string& GetAnimationName() const
    {
        return currentAnimation.first;
    }
};

struct CameraComponent {
    // TODO: Experiment with adding second camera for 3D to be able to toggle between perspective and orthographic camera view;
    Camera2D camera = { 0 };
    bool isMainCamera;

    bool FixedAspectRatio = false;

    // CameraComponent() = default;
    CameraComponent()
    {
        camera.target = (Vector2) { 0.0f, 0.0f };
        camera.offset = (Vector2) { 0, 0 };
        camera.zoom = 1.0f;
        camera.rotation = 0.0f;
    };
    CameraComponent(const CameraComponent&) = default;

    const Camera2D& GetCamera() { return camera; }
    void SetRotation(float rotation) { camera.rotation = rotation; }
    void SetZoom(float zoom) { camera.zoom = zoom; }
    void SetOffest(Vector2 offset) { camera.offset = offset; }
};

struct RigidBodyComponent {
    enum class BodyType {
        Static = 0,
        Kinematic = 1,
        Dynamic = 2
    };
    BodyType Type = BodyType::Dynamic;
    bool FixedRotation = false;
    b2Body* RuntimeBody = nullptr;

    RigidBodyComponent() = default;
    RigidBodyComponent(const RigidBodyComponent&) = default;

    void SetVelocity(Vector2 velocity)
    {
        RuntimeBody->SetLinearVelocity({ velocity.x, velocity.y });
    }

    void SetAngularVelocity(float velocity)
    {
        RuntimeBody->SetAngularVelocity(velocity);
    }

    Vector2 GetVelocity() const
    {
        return { RuntimeBody->GetLinearVelocity().x, RuntimeBody->GetLinearVelocity().y };
    }

    float GetAngularVelocity() const
    {
        return RuntimeBody->GetAngularVelocity();
    }

    float GetAngle() const
    {
        float angleRadians = RuntimeBody->GetAngle();

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
};

struct BoxCollider2DComponent {
    Vector2 Offset = { 0.0f, 0.0f };
    Vector2 Size = { 0.5f, 0.5f };

    float Density = 1.0f;
    float Friction = 0.5f;
    float Bounce = 0.0f, BounceThreshold = 0.0f;

    void* RuntimeFixture = nullptr;
    BoxCollider2DComponent() = default;
    BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
};

struct CapsuleCollider2DComponent {
    Vector2 Offset = { 0.0f, 0.0f };
    Vector2 Size = { 0.5f, 0.5f };

    float Density = 1.0f;
    float Friction = 0.5f;
    float Bounce = 0.5f, BounceThreshold = 0.5f;

    void* RuntimeFixture = nullptr;
    CapsuleCollider2DComponent() = default;
    CapsuleCollider2DComponent(const CapsuleCollider2DComponent&) = default;
};

struct NativeScriptComponent {
    ScriptableEntity* Instance = nullptr;

    ScriptableEntity* (*InstantiateScript)();
    void (*DestroyScript)(NativeScriptComponent*);

    template <typename T>
    void Bind()
    {
        InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
        DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
    }
};
}
