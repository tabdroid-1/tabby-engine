
#include "Scene/Components.h"
#include "raylib.h"
#include <Game/Scripts/cameraMove.h>
#include <cstdio>
#include <iostream>

void CameraMove::Load(float speed)
{
    moveSpeed = speed;
}

void CameraMove::Update()
{
}

void CameraMove::LateUpdate()
{
    Move();
}

void CameraMove::Move()
{
    input = { 0, 0 };

    // if (IsKeyDown(KEY_RIGHT)) {
    //     input.x = 5 * GetFrameTime();
    // }
    // if (IsKeyDown(KEY_LEFT)) {
    //     input.x = -5 * GetFrameTime();
    // }
    //
    // if (IsKeyDown(KEY_UP)) {
    //     input.y = 5 * GetFrameTime();
    // }
    // if (IsKeyDown(KEY_DOWN)) {
    //     input.y = -5 * GetFrameTime();
    // }

    if (IsKeyDown(KEY_RIGHT) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) {
        input.x = 7.5 * GetFrameTime();
    }
    if (IsKeyDown(KEY_LEFT) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) {
        input.x = -7.5 * GetFrameTime();
    }

    if (IsKeyDown(KEY_UP) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_UP)) {
        input.y = 7.5 * GetFrameTime();
    }
    if (IsKeyDown(KEY_DOWN) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN)) {
        input.y = -7.5 * GetFrameTime();
    }

    velocity = { input.x * moveSpeed, input.y * moveSpeed };

    auto& cameraTransform = GetComponent<Tabby::TransformComponent>();
    auto& camera = GetComponent<Tabby::CameraComponent>();

    camera.Camera.target = { cameraTransform.Position.x, cameraTransform.Position.y, cameraTransform.Position.z - 1 };

    cameraTransform.Position.x += velocity.x;
    cameraTransform.Position.y += velocity.y;
    // playerRB.SetVelocity({ velocity.x, playerRB.GetVelocity().y });

    // std::cout << "X: " << cameraTransform.Position.x << "  Y: " << cameraTransform.Position.y << "\n";

    // std::cout << GetFPS() << "\n";

    if (IsKeyDown(KEY_T)) {
        cameraTransform.Rotation.x += 2;
        // std::cout << cameraTransform.Rotation.x << "\n";
    }
    if (IsKeyDown(KEY_Y)) {
        cameraTransform.Rotation.y += 2;
        // std::cout << cameraTransform.Rotation.y << "\n";
    }
}
