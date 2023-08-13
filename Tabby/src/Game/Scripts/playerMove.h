#pragma once

#include <Scene/ScriptableGameObject.h>
#include <raylib.h>

class PlayerMove : public Tabby::ScriptableEntity {
public:
    // PlayerMove(GameObject* owner);

    void Load(float speed);
    void Update(float deltaTime) override;

    void CheckIfShouldFlip();
    void Flip();

private:
    int facingDirection = 1;
    Vector2 input = { 0, 0 };
    Vector2 velocity;

    float playerSpeed = 100;
};
