#pragma once

#include <cwchar>
#include <raylib.h>
#include <raymath.h>
#include <rcamera.h>

#include "C_Camera.h"

#include "Scene/GameObject.h"

C_Camera::C_Camera(GameObject* owner)
    : Component(owner)
{
}

void C_Camera::Load()
{
    camera.target = (Vector2) { owner->transform->GetPosition().x, owner->transform->GetPosition().y };
    camera.offset = (Vector2) { 0, 0 };
    camera.zoom = 1.0f;
    camera.rotation = 0.0f;
}

void C_Camera::Load(Vector2 offset)
{
    camera.target = (Vector2) { owner->transform->GetPosition().x, owner->transform->GetPosition().y };
    camera.offset = offset;
    camera.zoom = 1.0f;
    camera.rotation = 0.0f;
}

void C_Camera::Update(float dt)
{
    camera.target = { owner->transform->GetPosition().x,
        owner->transform->GetPosition().y };
}

void C_Camera::Draw()
{
}

void C_Camera::UpdateCamera(std::map<std::string, Camera2D>& cameraHashMap, std::string name)
{
    cameraHashMap.find(name)->second = GetCamera();
}
