#pragma once

#include "box2d/b2_body.h"
#include <Graphics/Animation.h>
#include <Scene/Scene.h>
#include <Scene/ScriptableGameObject.h>
#include <raylib.h>
#include <raymath.h>
#include <vector>

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
    Vector3 position = { 0.0f, 0.0f };
    Vector3 scale = { 1.0f, 1.0f, 1.0f };
    Vector3 rotation = { 0.0f, 0.0f, 0.0f };
    Vector3 localPosition = { 0.0f, 0.0f };
    Vector3 localScale = { 1.0f, 1.0f, 1.0f };
    Vector3 localRotation = { 0.0f, 0.0f, 0.0f };

    entt::entity parent { entt::null };
    std::vector<entt::entity> children;

    TransformComponent() = default;
    TransformComponent(const TransformComponent&) = default;
    TransformComponent(const Vector3& position)
        : position(position)
    {
    }

    Matrix GetTransform() const
    {
        Matrix mat = MatrixIdentity();

        mat = MatrixMultiply(mat, MatrixTranslate(position.x, position.y, position.z));
        mat = MatrixMultiply(mat, MatrixRotate(Vector3 { 1.0f, 0.0f, 0.0f }, rotation.x * DEG2RAD));
        mat = MatrixMultiply(mat, MatrixRotate(Vector3 { 0.0f, 1.0f, 0.0f }, rotation.y * DEG2RAD));
        mat = MatrixMultiply(mat, MatrixRotate(Vector3 { 0.0f, 0.0f, 1.0f }, rotation.z * DEG2RAD));
        mat = MatrixMultiply(mat, MatrixScale(scale.x, scale.y, scale.y));

        return mat;
    }
};

struct SpriteRendererComponent {
    Color Tint = WHITE;
    Texture2D Texture;
    Rectangle srcRec = { 0, 0, 100, 100 };
    Rectangle destRec;
    Vector2 origin;

    SpriteRendererComponent() = default;
    SpriteRendererComponent(const SpriteRendererComponent&) = default;
    SpriteRendererComponent(const Color tint)
        : Tint(tint)
    {
    }

    void SetTexture(const char* path)
    {
        Texture = LoadTexture(path);
        srcRec = { 0.0, 0.0, (float)Texture.width, (float)Texture.height };
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
    Camera camera = { 0 };
    int cameraMode = CAMERA_FIRST_PERSON;
    bool isMainCamera;
    bool debugCameraMovement = false;

    bool FixedAspectRatio = false;

    // CameraComponent() = default;
    CameraComponent()
    {
        camera = { 0 };
        camera.position = (Vector3) { 0.0f, 0.0f, 0.0f }; // Camera position
        camera.target = (Vector3) { 0.0f, 0.0f, 0.0f }; // Camera looking at point
        camera.up = (Vector3) { 0.0f, 1.0f, 0.0f }; // Camera up vector (rotation towards target)
        camera.fovy = 60.0f; // Camera field-of-view Y
        camera.projection = CAMERA_PERSPECTIVE; // Camera projection type
    };
    CameraComponent(const CameraComponent&) = default;
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
