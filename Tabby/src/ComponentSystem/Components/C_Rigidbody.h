#pragma once

#include "raylib.h"
#include <box2d/b2_world.h>
#include <box2d/box2d.h>
#include <cwchar>
#include <iostream>
#include <memory>
#include <string>

#include <ComponentSystem/Component.h>
#include <Scene/GameObject.h>

class C_Rigidbody : public Component {
public:
    C_Rigidbody(GameObject* owner);

    void Load(b2World& PhysicsWorld, b2BodyType BodyType, float WorldScale);
    void Load(b2World& PhysicsWorld, b2BodyType BodyType, bool FixedRotation, float WorldScale);

    // b2Fixture* CreateCapsuleFixture(Vector2 Size);
    // b2Fixture* CreateCapsuleFixture(Vector2 Size, Vector2 Offset);
    b2Fixture* CreateCapsuleFixture(Vector2 Size, float Density, float Friction);
    b2Fixture* CreateCapsuleFixture(Vector2 Size, Vector2 Offset, float Density, float Friction);
    b2Fixture* CreateCapsuleFixture(Vector2 Size, Vector2 Offset, float TopDensity, float TopFriction, float MidDensity, float MidFriction, float BottomDensity, float BottomFriction);
    // b2Fixture* CreateBoxFixture(Vector2 Size);
    // b2Fixture* CreateBoxFixture(Vector2 Size, Vector2 Offset);
    b2Fixture* CreateBoxFixture(Vector2 Size, float Density, float Friction);
    b2Fixture* CreateBoxFixture(Vector2 Size, Vector2 Offset, float Density, float Friction);

    void Update(float dt) override;
    void LateUpdate(float dt) override;
    void Delete() override;

public:
    float GetWorldScale() const;

    Vector2 GetPosition() const;
    void SetPosition(Vector2 Position);

    Vector2 GetVelocity() const;
    void SetVelocity(Vector2 Velocity);
    void SetVelocity(float VelocityY, float VelocityX);
    void SetVelocityX(float VelocityX);
    void SetVelocityY(float VelocityY);

    void AddVelocity(Vector2 Velocity);
    void AddVelocity(float VelocityY, float VelocityX);
    void AddVelocityX(float VelocityX);
    void AddVelocityY(float VelocityY);

    float GetAngle() const;
    void SetAngularVelocity(float AngularVelocity);
    void AddAngularVelocity(float AngularVelocity);

private:
    b2Body* body;
    b2World* physicsWorld;
    b2BodyDef bodyDef;

    float worldScale;
    Vector2 offset;
    bool allowSleeping = false;
};
