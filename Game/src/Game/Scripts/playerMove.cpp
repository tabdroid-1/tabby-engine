// #include "playerMove.h"
// #include "ComponentSystem/Component.h"
// #include "ComponentSystem/Components/C_Animation.h"
// #include "ComponentSystem/Components/C_Rigidbody.h"
// #include "ComponentSystem/Components/C_Sprite.h"
// #include "Scene/GameObject.h"
// #include "raylib.h"
//
// PlayerMove::PlayerMove(GameObject* owner)
//     : Component(owner)
// {
// }
//
// void PlayerMove::Load(float speed) { this->playerSpeed = speed; }
//
// void PlayerMove::Update(float dt)
// {
//     input = { 0, 0 };
//     velocity;
//
//     if (IsKeyDown(KEY_W)) {
//         // input.y = -1;
//     }
//     if (IsKeyDown(KEY_S)) {
//         // input.y = 1;
//     }
//     if (IsKeyDown(KEY_D)) {
//         input.x = 1;
//     }
//     if (IsKeyDown(KEY_A)) {
//         input.x = -1;
//     } else {
//         input = { 0, 0 };
//     }
//
//     if (owner->GetComponent<C_Animation>() != nullptr) {
//
//         if (owner->GetComponent<C_Rigidbody>()->GetVelocity().x >= -0.05f && owner->GetComponent<C_Rigidbody>()->GetVelocity().x <= 0.05f) {
//             owner->GetComponent<C_Animation>()->PlayAnimation("idle");
//         } else {
//             owner->GetComponent<C_Animation>()->PlayAnimation("walk");
//         }
//     }
//
//     velocity = { input.x * playerSpeed * dt, input.y * playerSpeed * dt };
//
//     owner->GetComponent<C_Rigidbody>()->SetVelocity(velocity.x, owner->GetComponent<C_Rigidbody>()->GetVelocity().y);
//
//     CheckIfShouldFlip();
// }
//
// void PlayerMove::CheckIfShouldFlip()
// {
//     if (input.x != 0 && input.x != facingDirection) {
//         Flip();
//     }
// }
//
// void PlayerMove::Flip()
// {
//     owner->GetComponent<C_Sprite>()->FlipTextureX();
//     facingDirection *= -1;
// }
#include "Scene/Components.h"
#include "raylib.h"
#include <Game/Scripts/playerMove.h>
#include <cstdio>
#include <iostream>

void PlayerMove::Load(float speed)
{
    playerSpeed = speed;
}

void PlayerMove::Update(float dt)
{

    CheckIfShouldFlip();
}

void PlayerMove::LateUpdate(float dt)
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
    if (playerTransform.Rotation.y == 0) {
        playerTransform.Rotation.y = 180;
    } else {
        playerTransform.Rotation.y = 0;
    }
    facingDirection *= -1;
}

void PlayerMove::Move()
{
    input = { 0, 0 };

    if (IsKeyDown(KEY_X)) {
        GetComponent<Tabby::TransformComponent>().Rotation.y -= 10;
    }
    if (IsKeyDown(KEY_C)) {
        GetComponent<Tabby::TransformComponent>().Rotation.y += 10;
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

    velocity = { input.x * playerSpeed, input.y * playerSpeed };

    auto& playerTransform = GetComponent<Tabby::TransformComponent>();

    if (HasComponent<Tabby::RigidBodyComponent>()) {
        auto& playerRB = GetComponent<Tabby::RigidBodyComponent>();

        // playerRB.SetVelocity({ velocity.x, playerRB.GetVelocity().y });

        GetComponent<Tabby::RigidBodyComponent>().SetVelocity(velocity);

        if (IsKeyDown(KEY_R)) {
            playerRB.SetAngularVelocity(2);
            // playerTransform.Rotation.z += 2;
        }
        if (IsKeyDown(KEY_T)) {
            playerTransform.Rotation.x += 2;
        }
        if (IsKeyDown(KEY_Y)) {
            playerTransform.Rotation.y += 2;
        }
    }
}
