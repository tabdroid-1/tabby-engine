#pragma once

#include "Tabby/Scene/Components.h"
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
    }

    void Update(Tabby::Timestep ts) override
    {

        CheckIfShouldFlip();
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

        // auto& playerTransform = GetComponent<Tabby::TransformComponent>();

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
};
