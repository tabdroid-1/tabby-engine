#pragma once

#include "Tabby/Core/Log.h"
#include "Tabby/Physics/Physics2D.h"
#include "Tabby/Scene/Components.h"
#include "Tabby/Scene/SceneStateMachine.h"
#include "glm/fwd.hpp"
#include <Tabby/Scene/ScriptableEntity.h>

#include "Tabby/Core/Application.h"
#include "Tabby/Core/Input.h"
#include "Tabby/Core/KeyCodes.h"
#include "Tabby/Core/Timestep.h"
#include "Tabby/Scene/Components.h"
#include <cstdio>
#include <iostream>

class PlayerMove : public Tabby::ScriptableEntity {
private:
    Tabby::TransformComponent* playerTransform;
    int facingDirection = 1;
    glm::vec2 input = { 0, 0 };
    glm::vec2 velocity;

    float playerSpeed = 2.5f;

public:
    PlayerMove() { }

    void OnCreate() override
    {
        playerTransform = &GetComponent<Tabby::TransformComponent>();

        Tabby::Entity RigidbodyEntity = Tabby::SceneStateMachine::GetCurrentScene()->CreateEntity("RigidbodyEntity2");
        {

            RigidbodyEntity.GetComponent<Tabby::TransformComponent>().Translation.y = -1.1f;
            auto& spriteComponent = RigidbodyEntity.AddComponent<Tabby::SpriteRendererComponent>();

            // NOTE: Collider components have to be added before rigidbody component
            auto& boxColliderComponent = RigidbodyEntity.AddComponent<Tabby::BoxCollider2DComponent>();
            boxColliderComponent.Size = { 0.19f, 0.24f };

            auto& rigidbodyComponent = RigidbodyEntity.AddComponent<Tabby::Rigidbody2DComponent>();
            rigidbodyComponent.Type = Tabby::Rigidbody2DComponent::BodyType::Dynamic;
        }
    }

    void Update(Tabby::Timestep ts) override
    {

        CheckIfShouldFlip();

        if (Tabby::Input::IsKeyPressed(Tabby::Key::X)) {

            // Tabby::Entity RigidbodyEntity = Tabby::SceneStateMachine::GetCurrentScene()->CreateEntity("RigidbodyEntity2");
            // {
            //
            //     RigidbodyEntity.GetComponent<Tabby::TransformComponent>().Translation.x = 0.25f;
            //     RigidbodyEntity.GetComponent<Tabby::TransformComponent>().Translation.y = 1.25f;
            //
            //     // NOTE: Collider components have to be added before rigidbody component
            //     auto& boxColliderComponent = RigidbodyEntity.AddComponent<Tabby::BoxCollider2DComponent>();
            //     boxColliderComponent.Size = { 0.19f, 0.24f };
            //
            //     auto& rigidbodyComponent = RigidbodyEntity.AddComponent<Tabby::Rigidbody2DComponent>();
            //     rigidbodyComponent.Type = Tabby::Rigidbody2DComponent::BodyType::Dynamic;
            // }

            Tabby::RaycastHit2D hit = Tabby::Physisc2D::Raycast(playerTransform->Translation, { 0.0, -1.0f }, 100.0f, -10, 10);
            TB_INFO("Normal: {0} : {1}", hit.normal.x, hit.normal.y);
            TB_INFO("Point: {0} : {1}", hit.point.x, hit.point.y);
            TB_INFO("Velocity: {0} {1}", hit.rigidbody->GetVelocity().x, hit.rigidbody->GetVelocity().y);
            TB_INFO("Translation: {0} {1}", hit.transform->Translation.x, hit.transform->Translation.y);
            TB_INFO("Distance: {0}", hit.distance);
            TB_INFO("Fraction: {0}", hit.fraction);
            TB_INFO("Tag: {0}", hit.entity.GetComponent<Tabby::TagComponent>().Tag);
        }
    }

    void LateUpdate(Tabby::Timestep ts) override
    {
        Move(ts);
    }

    void CheckIfShouldFlip()
    {
        if (input.x != 0 && input.x != facingDirection) {
            Flip();
        }
    }

    void Flip()
    {

        auto& playerTransform = GetComponent<Tabby::TransformComponent>();
        if (playerTransform.Rotation.y == 0) {

            playerTransform.Rotation.y = 1.0f;
        } else {
            playerTransform.Rotation.y = 0.0f;
        }
        facingDirection *= -1;
    }

    void Move(float dt)
    {
        input = { 0, 0 };

        auto& playerTransform = GetComponent<Tabby::TransformComponent>();
        if (Tabby::Input::IsKeyPressed(Tabby::Key::X)) {
            playerTransform.Rotation.y -= 10 * dt;
        }
        if (Tabby::Input::IsKeyPressed(Tabby::Key::Y)) {
            playerTransform.Rotation.y += 10 * dt;
        }
        if (Tabby::Input::IsKeyPressed(Tabby::Key::D)) {
            input.x = 1;
        }
        if (Tabby::Input::IsKeyPressed(Tabby::Key::A)) {
            input.x = -1;
        }

        if (Tabby::Input::IsKeyPressed(Tabby::Key::W)) {
            input.y = 1;
        }
        if (Tabby::Input::IsKeyPressed(Tabby::Key::S)) {
            input.y = -1;
        }

        if (Tabby::Input::IsKeyPressed(Tabby::Key::Escape)) {
            Tabby::Application::Get().Close();
        }

        velocity = { input.x * playerSpeed, input.y * playerSpeed };

        auto& playerRB = GetComponent<Tabby::Rigidbody2DComponent>();
        if (&playerRB) {

            playerRB.SetVelocity({ velocity.x, playerRB.GetVelocity().y });

            // GetComponent<Tabby::Rigidbody2DComponent>().SetVelocity(velocity);

            if (Tabby::Input::IsKeyPressed(Tabby::Key::R)) {
                playerRB.SetAngularVelocity(2);
                // playerTransform.Rotation.z += 2;
            }
            if (Tabby::Input::IsKeyPressed(Tabby::Key::T)) {
                // playerTransform.Rotation.x += 2;
            }
            if (Tabby::Input::IsKeyPressed(Tabby::Key::Y)) {
                // playerTransform.Rotation.y += 2;
            }
        }
    }

    void OnCollisionEnter(Tabby::ContactCallback contact) override
    {
        TB_INFO("COLLISION ENTER");

        TB_INFO("Velocity: {0} {1}", contact.rigidbody->GetVelocity().x, contact.rigidbody->GetVelocity().y);
        TB_INFO("Translation: {0} {1}\n", contact.transform->Translation.x, contact.transform->Translation.y);
    }
    void OnCollisionExit(Tabby::ContactCallback contact) override
    {
        TB_INFO("COLLISION ExIT");

        TB_INFO("Velocity: {0} {1}", contact.rigidbody->GetVelocity().x, contact.rigidbody->GetVelocity().y);
        TB_INFO("Translation: {0} {1}\n", contact.transform->Translation.x, contact.transform->Translation.y);
    }
    void OnPreSolve(Tabby::ContactCallback contact) override { }
    void OnPostSolve(Tabby::ContactCallback contact) override { }
};
