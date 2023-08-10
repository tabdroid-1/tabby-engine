#pragma once

#include <iostream>
#include <raylib.h>

#include "ComponentSystem/Components/C_Drawable.h"
#include <ComponentSystem/Component.h>

class C_Sprite : public Component, public C_Drawable {
public:
    C_Sprite(GameObject* owner);

    void Load(Texture2D texture, bool useOwnerSize);
    void Load(const std::string& filePath, bool useOwnerSize);

    void LateUpdate(float deltaTime) override;

    void Draw() override;
    void Delete() override;

    void FlipTextureX();
    void FlipTextureY();

    void SetTextureRect(int x, int y, int width, int height);
    void SetTextureRect(Rectangle& srcRec);

    void SetOrigin(Vector2 Origin) { origin = Origin; }
    void SetColor(Color Tint) { tint = Tint; }
    const Texture2D& GetTexture() const { return texture; }

private:
    Texture2D texture;
    Image tempImage;

    Rectangle srcRec;
    Rectangle destRec;
    Vector2 origin;
    Color tint;

    Vector2 facingDirection = { 1, 1 };
};
