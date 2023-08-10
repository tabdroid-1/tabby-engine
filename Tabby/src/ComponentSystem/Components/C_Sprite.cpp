#pragma once

#include <raylib.h>

#include "C_Sprite.h"

#include <Scene/GameObject.h>

C_Sprite::C_Sprite(GameObject* owner)
    : Component(owner)
    , origin(owner->transform->GetOrigin())
    , tint(WHITE)
{
}

void C_Sprite::Load(Texture2D texture, bool useOwnerSize)
{
    tempImage = LoadImageFromTexture(texture);
    this->texture = LoadTextureFromImage(tempImage);

    UnloadImage(tempImage);

    if (useOwnerSize) {
        srcRec = { 0, 0, (float)owner->transform->GetSize().x, (float)owner->transform->GetSize().y };
    } else {
        srcRec = { 0, 0, (float)texture.width, (float)texture.height };
    }
}

void C_Sprite::Load(const std::string& filePath, bool useOwnerSize)
{
    tempImage = LoadImage(filePath.c_str());
    texture = LoadTextureFromImage(tempImage);

    UnloadImage(tempImage);

    if (useOwnerSize) {
        srcRec = { 0, 0, (float)owner->transform->GetSize().x, (float)owner->transform->GetSize().y };
    } else {
        srcRec = { 0, 0, (float)texture.width, (float)texture.height };
    }
}

void C_Sprite::LateUpdate(float dt)
{
}

void C_Sprite::Draw()
{

    origin = {
        (owner->transform->GetSize().x * owner->transform->GetScale().x) / 2,
        (owner->transform->GetSize().y * owner->transform->GetScale().y) / 2,
    };

    // destRec = {
    //     owner->transform->GetPosition().x, owner->transform->GetPosition().y,
    //     owner->transform->GetSize().x * owner->transform->GetScale().x,
    //     owner->transform->GetSize().y * owner->transform->GetScale().y
    // };

    destRec = {
        owner->transform->GetPosition().x, owner->transform->GetPosition().y,
        owner->transform->GetSize().x * owner->transform->GetScale().x * facingDirection.x,
        owner->transform->GetSize().y * owner->transform->GetScale().y * facingDirection.y
    };
    DrawTexturePro(texture, srcRec, destRec, { owner->transform->GetOrigin().x + origin.x, owner->transform->GetOrigin().y + origin.y }, owner->transform->GetRotation(), tint);
}

void C_Sprite::FlipTextureX()
{
    facingDirection.x *= -1;
}

void C_Sprite::FlipTextureY()
{
    facingDirection.y *= -1;
}
void C_Sprite::Delete() { UnloadTexture(texture); }

void C_Sprite::SetTextureRect(int x, int y, int width, int height)
{
    srcRec = { (float)x, (float)y, (float)width, (float)height };
}

void C_Sprite::SetTextureRect(Rectangle& srcRect) { srcRec = srcRect; }
