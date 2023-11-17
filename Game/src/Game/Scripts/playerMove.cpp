#include "Core/Application.h"
#include "Physics/Physics.h"
#include "Scene/Components.h"
#include "raylib.h"
#include <Game/Scripts/playerMove.h>
#include <cstdio>
#include <iostream>

void PlayerMove::Load(float speed)
{
    playerSpeed = speed;
}

void PlayerMove::Update()
{

    CheckIfShouldFlip();
}

void PlayerMove::LateUpdate()
{
    Move();
}

void PlayerMove::CheckIfShouldFlip()
{

    if (input.x != 0 && input.x != facingDirection) {
        Flip();
    }
}

void PlayerMove::Flip()
{
    auto& playerTransform = GetComponent<Tabby::TransformComponent>();
    if (playerTransform.rotation.y == 0) {
        playerTransform.rotation.y = 180;
    } else {
        playerTransform.rotation.y = 0;
    }
    facingDirection *= -1;
}

void PlayerMove::Move()
{
    input = { 0, 0 };

    if (IsKeyDown(KEY_X)) {
        GetComponent<Tabby::TransformComponent>().rotation.y -= 10;
    }
    if (IsKeyDown(KEY_C)) {
        GetComponent<Tabby::TransformComponent>().rotation.y += 10;
    }
    if (IsKeyDown(KEY_D)) {
        input.x = 1;
    }
    if (IsKeyDown(KEY_A)) {
        input.x = -1;
    }

    if (IsKeyDown(KEY_W)) {
        input.y = 1;
    }
    if (IsKeyDown(KEY_S)) {
        input.y = -1;
    }

    if (IsKeyDown(KEY_ESCAPE)) {
        Tabby::Application::CloseApplication();
    }

    if (IsKeyDown(KEY_E)) {
        Tabby::Physics::SetGravity({ 0.0f, -9.81f });
    }

    velocity = { input.x * playerSpeed, input.y * playerSpeed };

    auto& playerTransform = GetComponent<Tabby::TransformComponent>();

    if (HasComponent<Tabby::RigidBodyComponent>()) {
        auto& playerRB = GetComponent<Tabby::RigidBodyComponent>();

        playerRB.SetVelocity({ velocity.x, playerRB.GetVelocity().y });

        // GetComponent<Tabby::RigidBodyComponent>().SetVelocity(velocity);

        if (IsKeyDown(KEY_R)) {
            // playerRB.SetAngularVelocity(2);
            // playerTransform.Rotation.z += 2;
        }
        if (IsKeyDown(KEY_T)) {
            playerTransform.rotation.x += 2;
        }
        if (IsKeyDown(KEY_Y)) {
            playerTransform.rotation.y += 2;
        }
    }
}
