#include "playerMove.h"
#include "ComponentSystem/Component.h"
#include "ComponentSystem/Components/C_Animation.h"
#include "ComponentSystem/Components/C_Rigidbody.h"
#include "ComponentSystem/Components/C_Sprite.h"
#include "Scene/GameObject.h"
#include "raylib.h"

PlayerMove::PlayerMove(GameObject* owner)
    : Component(owner)
{
}

void PlayerMove::Load(float speed) { this->playerSpeed = speed; }

void PlayerMove::Update(float dt)
{
    input = { 0, 0 };
    velocity;

    if (IsKeyDown(KEY_W)) {
        // input.y = -1;
    }
    if (IsKeyDown(KEY_S)) {
        // input.y = 1;
    }
    if (IsKeyDown(KEY_D)) {
        input.x = 1;
    }
    if (IsKeyDown(KEY_A)) {
        input.x = -1;
    } else {
        input = { 0, 0 };
    }

    if (owner->GetComponent<C_Animation>() != nullptr) {

        if (owner->GetComponent<C_Rigidbody>()->GetVelocity().x >= -0.05f && owner->GetComponent<C_Rigidbody>()->GetVelocity().x <= 0.05f) {
            owner->GetComponent<C_Animation>()->PlayAnimation("idle");
        } else {
            owner->GetComponent<C_Animation>()->PlayAnimation("walk");
        }
    }

    velocity = { input.x * playerSpeed * dt, input.y * playerSpeed * dt };

    owner->GetComponent<C_Rigidbody>()->SetVelocity(velocity.x, owner->GetComponent<C_Rigidbody>()->GetVelocity().y);

    CheckIfShouldFlip();
}

void PlayerMove::CheckIfShouldFlip()
{
    if (input.x != 0 && input.x != facingDirection) {
        Flip();
    }
}

void PlayerMove::Flip()
{
    owner->GetComponent<C_Sprite>()->FlipTextureX();
    facingDirection *= -1;
}
