
#include "Scene/Components.h"
#include "raylib.h"
#include <Game/Scripts/cameraMove.h>
#include <cstdio>
#include <iostream>

void CameraMove::Load(float speed)
{
    moveSpeed = speed;
}

void CameraMove::Update(float dt)
{
}

void CameraMove::LateUpdate(float dt)
{
    Move();
}

void CameraMove::Move()
{
    input = { 0, 0 };

    if (IsKeyDown(KEY_RIGHT)) {
        input.x = 1;
    }
    if (IsKeyDown(KEY_LEFT)) {
        input.x = -1;
    }

    if (IsKeyDown(KEY_UP)) {
        input.y = 1;
    }
    if (IsKeyDown(KEY_DOWN)) {
        input.y = -1;
    }

    velocity = { input.x * moveSpeed, input.y * moveSpeed };

    auto& cameraTransform = GetComponent<Tabby::TransformComponent>();
    auto& camera = GetComponent<Tabby::CameraComponent>();

    cameraTransform.Position.x += velocity.x;
    cameraTransform.Position.y += velocity.y;
    // playerRB.SetVelocity({ velocity.x, playerRB.GetVelocity().y });

    // std::cout << "X: " << cameraTransform.Position.x << "  Y: " << cameraTransform.Position.y << "\n";

    camera.camera.target = { cameraTransform.Position.x, cameraTransform.Position.y, cameraTransform.Position.z - 1 };

    if (IsKeyDown(KEY_T)) {
        cameraTransform.Rotation.x += 2;
        // std::cout << cameraTransform.Rotation.x << "\n";
    }
    if (IsKeyDown(KEY_Y)) {
        cameraTransform.Rotation.y += 2;
        // std::cout << cameraTransform.Rotation.y << "\n";
    }
}
