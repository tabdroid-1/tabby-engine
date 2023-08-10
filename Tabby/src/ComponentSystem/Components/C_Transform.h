#pragma once

#include "ComponentSystem/Component.h"
#include <raylib.h>
#include <sched.h>

class C_Transform : public Component {
public:
    C_Transform(GameObject* owner);

public:
    void SetPhysicsPosition(float x, float y);
    void SetPhysiscPosition(const Vector2& pos);

    void SetPosition(float x, float y);
    void SetPosition(const Vector2& pos);

    void AddPosition(float x, float y);
    void AddPosition(Vector2 pos);

    void SetPosX(float x);
    void SetPosY(float y);

    void AddPosX(float x);
    void AddPosY(float y);

    const Vector2& GetPosition() const { return position; }

public:
    void SetSize(float sizeX, float sizeY);
    void SetSize(const Vector2& size);

    void AddSize(float sizeX, float sizeY);
    void AddSize(Vector2 size);

    void SetSizeX(float x);
    void SetSizeY(float y);

    void AddSizeX(float x);
    void AddSizeY(float y);

    const Vector2& GetSize() const { return size; }

public:
    void SetScale(float sizeX, float sizeY);
    void SetScale(const Vector2& size);

    void AddScale(float sizeX, float sizeY);
    void AddScale(Vector2 size);

    void SetScaleX(float x);
    void SetScaleY(float y);

    void AddScaleX(float x);
    void AddScaleY(float y);

    const Vector2& GetScale() const { return scale; }

public:
    void SetOrigin(float OriginX, float OriginY);
    void SetOrigin(const Vector2& Origin);

    void AddOrigin(float OriginX, float OriginY);
    void AddOrigin(Vector2 Origin);

    void SetOriginX(float x);
    void SetOriginY(float y);

    void AddOriginX(float x);
    void AddOriginY(float y);

    const Vector2& GetOrigin() const { return origin; }

public:
    void SetRotation(float rotation);
    void AddRotation(float rotation);

    const float& GetRotation() const { return rotation; }

    Vector2 position;

private:
    Vector2 size;
    Vector2 scale;
    Vector2 origin;
    float rotation;

    Rectangle rect;
};
