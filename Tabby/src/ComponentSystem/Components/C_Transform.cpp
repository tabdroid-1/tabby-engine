#include "C_Transform.h"
#include "ComponentSystem/Components/C_Rigidbody.h"
#include <Scene/GameObject.h>

C_Transform::C_Transform(GameObject* owner)
    : Component(owner)
    , position((Vector2) { 0.0f, 0.0f })
    , size((Vector2) { 100.0f, 100.0f })
    , scale((Vector2) { 1.0f, 1.0f })
    , origin((Vector2) { 0.0f, 0.0f })
    , rotation(0.0f)
{
}

// =================== Postion

// void C_Transform::SetPhysicsPosition(float x, float y)
// {
//     position.x = x;
//     position.y = y;
// }
// void C_Transform::SetPhysiscPosition(const Vector2& pos)
// {
//     position = pos;
// }

void C_Transform::SetPosition(float x, float y)
{

    if (owner->GetComponent<C_Rigidbody>() != nullptr) {
        owner->GetComponent<C_Rigidbody>()->SetPosition({ x / owner->GetComponent<C_Rigidbody>()->GetWorldScale(), y / owner->GetComponent<C_Rigidbody>()->GetWorldScale() });
    } else {
        position.x = x;
        position.y = y;
    }
}
void C_Transform::SetPosition(const Vector2& pos)
{

    if (owner->GetComponent<C_Rigidbody>() != nullptr) {
        owner->GetComponent<C_Rigidbody>()->SetPosition({ pos.x / owner->GetComponent<C_Rigidbody>()->GetWorldScale(), pos.y / owner->GetComponent<C_Rigidbody>()->GetWorldScale() });
    } else {
        position = pos;
    }
}

void C_Transform::AddPosition(float x, float y)
{

    // bodyDef.position.Set(owner->transform->GetPosition().x / worldScale, owner->transform->GetPosition().y / worldScale);

    if (owner->GetComponent<C_Rigidbody>() != nullptr) {
        owner->GetComponent<C_Rigidbody>()->SetPosition({ (owner->GetComponent<C_Rigidbody>()->GetPosition().x + x) / owner->GetComponent<C_Rigidbody>()->GetWorldScale(), (owner->GetComponent<C_Rigidbody>()->GetPosition().y + y) / owner->GetComponent<C_Rigidbody>()->GetWorldScale() });
    } else {
        position.x += x;
        position.y += y;
    }
}
void C_Transform::AddPosition(Vector2 pos)
{

    if (owner->GetComponent<C_Rigidbody>() != nullptr) {
        owner->GetComponent<C_Rigidbody>()->SetPosition({ (owner->GetComponent<C_Rigidbody>()->GetPosition().x + pos.x) / owner->GetComponent<C_Rigidbody>()->GetWorldScale(), (owner->GetComponent<C_Rigidbody>()->GetPosition().y + pos.y) / owner->GetComponent<C_Rigidbody>()->GetWorldScale() });
    } else {
        position.x += pos.x;
        position.y += pos.y;
    }
}

void C_Transform::SetPosX(float x)
{
    if (owner->GetComponent<C_Rigidbody>() != nullptr) {
        owner->GetComponent<C_Rigidbody>()->SetPosition({ x / owner->GetComponent<C_Rigidbody>()->GetWorldScale(), owner->GetComponent<C_Rigidbody>()->GetPosition().y });
    } else {
        position.x = x;
    }
}
void C_Transform::SetPosY(float y)
{
    if (owner->GetComponent<C_Rigidbody>() != nullptr) {
        owner->GetComponent<C_Rigidbody>()->SetPosition({ owner->GetComponent<C_Rigidbody>()->GetPosition().x, y / owner->GetComponent<C_Rigidbody>()->GetWorldScale() });
    } else {
        position.y = y;
    }
}

void C_Transform::AddPosX(float x)
{

    if (owner->GetComponent<C_Rigidbody>() != nullptr) {
        owner->GetComponent<C_Rigidbody>()->SetPosition({ owner->GetComponent<C_Rigidbody>()->GetPosition().x + (x / owner->GetComponent<C_Rigidbody>()->GetWorldScale()), owner->GetComponent<C_Rigidbody>()->GetPosition().y });
    } else {
        position.x += x;
    }
}
void C_Transform::AddPosY(float y)
{

    if (owner->GetComponent<C_Rigidbody>() != nullptr) {
        owner->GetComponent<C_Rigidbody>()->SetPosition({ owner->GetComponent<C_Rigidbody>()->GetPosition().x, owner->GetComponent<C_Rigidbody>()->GetPosition().y + (y / owner->GetComponent<C_Rigidbody>()->GetWorldScale()) });
    } else {
        position.y += y;
    }
}

// ==================== Size

void C_Transform::SetSize(float x, float y)
{
    size.x = x;
    size.y = y;
}
void C_Transform::SetSize(const Vector2& size) { this->size = size; }

void C_Transform::AddSize(float x, float y)
{
    size.x += x;
    size.y += y;
}
void C_Transform::AddSize(Vector2 size)
{

    this->size.x = size.x;
    this->size.y = size.y;
}

void C_Transform::SetSizeX(float x) { size.x = x; }
void C_Transform::SetSizeY(float y) { size.y = y; }

void C_Transform::AddSizeX(float x) { size.x += x; }
void C_Transform::AddSizeY(float y) { size.y += y; }

// ==================== Scale

void C_Transform::SetScale(float x, float y)
{
    scale.x = x;
    scale.y = y;
}
void C_Transform::SetScale(const Vector2& scale) { this->scale = scale; }

void C_Transform::AddScale(float x, float y)
{
    scale.x += x;
    scale.y += y;
}
void C_Transform::AddScale(Vector2 scale)
{

    this->scale.x = scale.x;
    this->scale.y = scale.y;
}

void C_Transform::SetScaleX(float x) { scale.x = x; }
void C_Transform::SetScaleY(float y) { scale.y = y; }

void C_Transform::AddScaleX(float x) { scale.x += x; }
void C_Transform::AddScaleY(float y) { scale.y += y; }

// ==================== Origin

void C_Transform::SetOrigin(float x, float y)
{
    origin.x = x;
    origin.y = y;
}
void C_Transform::SetOrigin(const Vector2& origin) { this->origin = origin; }

void C_Transform::AddOrigin(float x, float y)
{
    origin.x += x;
    origin.y += y;
}
void C_Transform::AddOrigin(Vector2 origin)
{

    this->origin.x = origin.x;
    this->origin.y = origin.y;
}

void C_Transform::SetOriginX(float x) { origin.x = x; }
void C_Transform::SetOriginY(float y) { origin.y = y; }

void C_Transform::AddOriginX(float x) { origin.x += x; }
void C_Transform::AddOriginY(float y) { origin.y += y; }
// ==================== Rotation

void C_Transform::SetRotation(float rotation) { this->rotation = rotation; }

void C_Transform::AddRotation(float rotation) { this->rotation += rotation; }
