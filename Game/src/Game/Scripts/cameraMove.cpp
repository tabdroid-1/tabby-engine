
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
        input.x = 5 * GetFrameTime();
    }
    if (IsKeyDown(KEY_LEFT) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) {
        input.x = -5 * GetFrameTime();
    }

    if (IsKeyDown(KEY_UP) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_UP)) {
        input.y = 5 * GetFrameTime();
    }
    if (IsKeyDown(KEY_DOWN) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN)) {
        input.y = -5 * GetFrameTime();
    }

    velocity = { input.x * moveSpeed, input.y * moveSpeed };

    auto& cameraTransform = GetComponent<Tabby::TransformComponent>();
    auto& camera = GetComponent<Tabby::CameraComponent>();

    cameraTransform.position.x += velocity.x;
    cameraTransform.position.y += velocity.y;
    // playerRB.SetVelocity({ velocity.x, playerRB.GetVelocity().y });

    // std::cout << "X: " << cameraTransform.Position.x << "  Y: " << cameraTransform.Position.y << "\n";

    // std::cout << GetFPS() << "\n";
    camera.camera.target = { cameraTransform.position.x, cameraTransform.position.y, cameraTransform.position.z - 1 };

    if (IsKeyDown(KEY_T)) {
        cameraTransform.rotation.x += 2;
        // std::cout << cameraTransform.Rotation.x << "\n";
    }
    if (IsKeyDown(KEY_Y)) {
        cameraTransform.rotation.y += 2;
        // std::cout << cameraTransform.Rotation.y << "\n";
    }
}
