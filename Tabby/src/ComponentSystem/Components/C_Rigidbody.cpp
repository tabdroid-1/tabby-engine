#include "box2d/b2_fixture.h"
#include "box2d/b2_world.h"
#include "raylib.h"
#include <ComponentSystem/Components/C_Rigidbody.h>
#include <ios>
#include <iostream>

C_Rigidbody::C_Rigidbody(GameObject* owner)
    : Component(owner)
    , offset({ 0.0f, 0.0f })
{
}

void C_Rigidbody::Load(b2World& PhysicsWorld, b2BodyType BodyType, float WorldScale)
{
    worldScale = WorldScale;
    physicsWorld = &PhysicsWorld;

    bodyDef.type = BodyType;
    bodyDef.position.Set(owner->transform->GetPosition().x / worldScale, owner->transform->GetPosition().y / worldScale);
    bodyDef.bullet = true;

    body = physicsWorld->CreateBody(&bodyDef);
    body->SetFixedRotation(false);
}

void C_Rigidbody::Load(b2World& PhysicsWorld, b2BodyType BodyType, bool FixedRotation, float WorldScale)
{
    worldScale = WorldScale;
    physicsWorld = &PhysicsWorld;

    bodyDef.type = BodyType;
    bodyDef.position.Set(owner->transform->GetPosition().x / worldScale, owner->transform->GetPosition().y / worldScale);
    bodyDef.bullet = true;

    body = physicsWorld->CreateBody(&bodyDef);
    body->SetFixedRotation(FixedRotation);
}

void C_Rigidbody::Update(float dt)
{
}

void C_Rigidbody::LateUpdate(float dt)
{
    owner->transform->position = { (body->GetPosition().x + offset.x) * worldScale, (body->GetPosition().y + offset.y) * worldScale };
    owner->transform->SetRotation(GetAngle());
}
void C_Rigidbody::Delete()
{
    physicsWorld->DestroyBody(body);
}

b2Fixture* C_Rigidbody::CreateBoxFixture(Vector2 Size, float Density, float Friction)
{
    b2FixtureDef fixtureDef;
    fixtureDef.density = Density;
    fixtureDef.friction = Friction;

    // Create a box shape for the main body of the capsule
    b2PolygonShape boxShape;
    boxShape.SetAsBox(Size.x, Size.y);
    fixtureDef.shape = &boxShape;

    body->CreateFixture(&fixtureDef);
}

b2Fixture* C_Rigidbody::CreateBoxFixture(Vector2 Size, Vector2 Offset, float Density, float Friction)
{
    offset = Offset;
    owner->transform->SetOrigin(Offset);

    b2FixtureDef fixtureDef;
    fixtureDef.density = Density;
    fixtureDef.friction = Friction;

    // Create a box shape for the main body of the capsule
    b2PolygonShape boxShape;
    boxShape.SetAsBox(Size.x, Size.y);
    fixtureDef.shape = &boxShape;

    body->CreateFixture(&fixtureDef);
}

b2Fixture* C_Rigidbody::CreateCapsuleFixture(Vector2 Size, float Density, float Friction)
{
    b2FixtureDef fixtureDef;
    fixtureDef.density = Density;
    fixtureDef.friction = Friction;

    // Create a box shape for the main body of the capsule
    b2PolygonShape boxShape;
    boxShape.SetAsBox(Size.x / 2.0f, (Size.y - 2.0f * (Size.x / 2)) / 2.0f, b2Vec2(0.0f, 0.0f), 0.0f);
    fixtureDef.shape = &boxShape;
    body->CreateFixture(&fixtureDef);

    // Create a circle shape for the top circle of the capsule
    b2CircleShape topCircleShape;
    topCircleShape.m_radius = Size.x / 2;
    fixtureDef.shape = &topCircleShape;

    // Position the top circle shape at the top of the capsule
    b2Vec2 topPosition(0.0f, Size.y / 2.0f - (Size.x / 2));
    topCircleShape.m_p = topPosition;
    body->CreateFixture(&fixtureDef);

    // Create a circle shape for the bottom circle of the capsule
    b2CircleShape bottomCircleShape;
    bottomCircleShape.m_radius = Size.x / 2;
    fixtureDef.shape = &bottomCircleShape;

    // Position the bottom circle shape at the bottom of the capsule
    b2Vec2 bottomPosition(0.0f, -Size.y / 2.0f + (Size.x / 2));
    bottomCircleShape.m_p = bottomPosition;
    body->CreateFixture(&fixtureDef);
}

b2Fixture* C_Rigidbody::CreateCapsuleFixture(Vector2 Size, Vector2 Offset, float Density, float Friction)
{
    offset = Offset;
    owner->transform->SetOrigin(Offset);

    b2FixtureDef fixtureDef;
    fixtureDef.density = Density;
    fixtureDef.friction = Friction;

    // Create a box shape for the main body of the capsule
    b2PolygonShape boxShape;
    boxShape.SetAsBox(Size.x / 2.0f, (Size.y - 2.0f * (Size.x / 2)) / 2.0f, b2Vec2(0.0f, 0.0f), 0.0f);
    fixtureDef.shape = &boxShape;
    body->CreateFixture(&fixtureDef);

    // Create a circle shape for the top circle of the capsule
    b2CircleShape topCircleShape;
    topCircleShape.m_radius = Size.x / 2;
    fixtureDef.shape = &topCircleShape;

    // Position the top circle shape at the top of the capsule
    b2Vec2 topPosition(0.0f, Size.y / 2.0f - (Size.x / 2));
    topCircleShape.m_p = topPosition;
    body->CreateFixture(&fixtureDef);

    // Create a circle shape for the bottom circle of the capsule
    b2CircleShape bottomCircleShape;
    bottomCircleShape.m_radius = Size.x / 2;
    fixtureDef.shape = &bottomCircleShape;

    // Position the bottom circle shape at the bottom of the capsule
    b2Vec2 bottomPosition(0.0f, -Size.y / 2.0f + (Size.x / 2));
    bottomCircleShape.m_p = bottomPosition;
    body->CreateFixture(&fixtureDef);
}

b2Fixture* C_Rigidbody::CreateCapsuleFixture(Vector2 Size, Vector2 Offset, float TopDensity, float TopFriction, float MidDensity, float MidFriction, float BottomDensity, float BottomFriction)
{
    offset = Offset;
    owner->transform->SetOrigin(Offset);

    b2FixtureDef fixtureDef;
    fixtureDef.density = MidDensity;
    fixtureDef.friction = MidFriction;

    // Create a box shape for the main body of the capsule
    b2PolygonShape boxShape;
    boxShape.SetAsBox(Size.x / 2.0f, (Size.y - 2.0f * (Size.x / 2)) / 2.0f, b2Vec2(0.0f, 0.0f), 0.0f);
    fixtureDef.shape = &boxShape;
    body->CreateFixture(&fixtureDef);

    // Create a circle shape for the top circle of the capsule
    b2CircleShape topCircleShape;
    topCircleShape.m_radius = Size.x / 2;
    fixtureDef.shape = &topCircleShape;
    fixtureDef.density = TopDensity;
    fixtureDef.friction = TopFriction;

    // Position the top circle shape at the top of the capsule
    b2Vec2 topPosition(0.0f, Size.y / 2.0f - (Size.x / 2));
    topCircleShape.m_p = topPosition;
    body->CreateFixture(&fixtureDef);

    // Create a circle shape for the bottom circle of the capsule
    b2CircleShape bottomCircleShape;
    bottomCircleShape.m_radius = Size.x / 2;
    fixtureDef.shape = &bottomCircleShape;
    fixtureDef.density = BottomDensity;
    fixtureDef.friction = BottomFriction;

    // Position the bottom circle shape at the bottom of the capsule
    b2Vec2 bottomPosition(0.0f, -Size.y / 2.0f + (Size.x / 2));
    bottomCircleShape.m_p = bottomPosition;
    body->CreateFixture(&fixtureDef);
}

float C_Rigidbody::GetWorldScale() const
{
    return worldScale;
}

Vector2 C_Rigidbody::GetPosition() const
{
    return { body->GetPosition().x, body->GetPosition().y };
}

Vector2 C_Rigidbody::GetVelocity() const
{
    return { body->GetLinearVelocity().x, body->GetLinearVelocity().y };
}
void C_Rigidbody::SetPosition(Vector2 Position)
{
    // bodyDef.position = b2Vec2(Position.x / worldScale, Position.y / worldScale);
    body->SetTransform(b2Vec2(Position.x, Position.y), body->GetAngle());
}

void C_Rigidbody::SetVelocity(Vector2 velocity)
{
    body->SetLinearVelocity({
        velocity.x,
        velocity.y,
    });
}

void C_Rigidbody::SetVelocity(float velocityX, float velocityY)
{
    body->SetLinearVelocity({
        velocityX,
        velocityY,
    });
}

void C_Rigidbody::SetVelocityX(float velocityX)
{
    body->SetLinearVelocity({
        velocityX,
        body->GetLinearVelocity().y,
    });
}

void C_Rigidbody::SetVelocityY(float velocityY)
{
    body->SetLinearVelocity({
        body->GetLinearVelocity().x,
        velocityY,
    });
}

void C_Rigidbody::AddVelocity(Vector2 velocity)
{
    body->SetLinearVelocity({
        body->GetLinearVelocity().x + velocity.x,
        body->GetLinearVelocity().y + velocity.y,
    });
}

void C_Rigidbody::AddVelocity(float velocityX, float velocityY)
{
    body->SetLinearVelocity({
        body->GetLinearVelocity().x + velocityX,
        body->GetLinearVelocity().y + velocityY,
    });
}

void C_Rigidbody::AddVelocityX(float velocityX)
{
    body->SetLinearVelocity({
        body->GetLinearVelocity().x + velocityX,
        body->GetLinearVelocity().y,
    });
}

void C_Rigidbody::AddVelocityY(float velocityY)
{
    body->SetLinearVelocity({
        body->GetLinearVelocity().x,
        body->GetLinearVelocity().y + velocityY,
    });
}

float C_Rigidbody::GetAngle() const
{
    float angleRadians = body->GetAngle();

    // Convert the angle from radians to degrees
    float angleDegrees = angleRadians * (180.0f / b2_pi);

    // Wrap the angle to the range [0, 360]
    angleDegrees = std::fmod(angleDegrees, 360.0f);

    // If the angle is negative, add 360 to make it positive
    if (angleDegrees < 0.0f) {
        angleDegrees += 360.0f;
    }

    return angleDegrees;
}
void C_Rigidbody::SetAngularVelocity(float AngularVelocity)
{
    body->SetAngularVelocity(AngularVelocity);
}

void C_Rigidbody::AddAngularVelocity(float AngularVelocity)
{
    body->SetAngularVelocity(body->GetAngularVelocity() + AngularVelocity);
}
