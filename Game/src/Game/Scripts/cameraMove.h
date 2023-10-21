#pragma once

#include <Scene/ScriptableGameObject.h>
#include <raylib.h>

class CameraMove : public Tabby::ScriptableEntity {
public:
    void Load(float speed);
    void Update(float deltaTime) override;
    void LateUpdate(float deltaTime) override;

    void Move();

private:
    int facingDirection = 1;
    Vector2 input = { 0, 0 };
    Vector3 velocity;

    float moveSpeed = 0.5;
};
