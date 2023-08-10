#pragma once

#include "box2d/b2_world.h"
#include <box2d/b2_body.h>
#include <raylib.h>
#include <vector>

class Physics {
public:
    Physics();

    void Load(Vector2 gravity);
    void Load(Vector2 Gravity, int VelocityIterations, int PositionIterations);
    void Update(float dt);
    void Draw(Camera2D& camera);
    void Destroy();

    b2World& GetPhysicsWorld() { return *physicsWorld; }
    const float GetPhysicsWorldScale() { return physicsWorldScale; }

    float GetScreenScale() { }
    void SetScreenScale(float ScreenScale) { screenScale = ScreenScale; }

    Vector2 GetWorldSize() { return worldSize; }
    Vector2 GetWorldMeterSize() { return worldMeterSize; }
    void SetWorldSize(Vector2 WorldSize)
    {
        worldSize = WorldSize;
        worldMeterSize = { pixelsToMetres(worldSize.x), pixelsToMetres(worldSize.y) };
    }

    float GetPixelsPerMeter() { return pixelsPerMeter; }
    void SetPixelsPerMeter(float PixelsPerMeter) { pixelsPerMeter = PixelsPerMeter; }

    float pixelsToMetres(float pixels) { return pixels / pixelsPerMeter; }
    float metresToPixels(float meters) { return meters * pixelsPerMeter; }

public:
    void DrawColliders(float worldScale, Camera2D& camera);
    Vector2 ConvertWorldToScreen(const b2Vec2& worldPosition, const Vector2& windowSize, const float worldScale);

private:
    float screenScale = 1;
    float pixelsPerMeter = 48;
    const float physicsWorldScale = pixelsPerMeter * screenScale;

    Vector2 worldSize = { 24000.0f, 12000.f };
    Vector2 worldMeterSize = { worldSize.x / pixelsPerMeter, worldSize.y / pixelsPerMeter };
    const Vector2 worldScale = { worldMeterSize.x / GetMonitorWidth(GetCurrentMonitor()), worldMeterSize.y / GetMonitorHeight(GetCurrentMonitor()) };

    static b2World* physicsWorld;
    int velocityIterations;
    int positionIterations;
};

// float SCREEN_SCALE = 1;
// float SCREEN_WIDTH = 2560;
// float SCREEN_HEIGHT = 1600;
//
// float PIXELS_PER_METER = 32;
//
// float WORLD_WIDTH = 500;
// float WORLD_HEIGHT = 500;
// float WORLD_SCALE_X = WORLD_WIDTH / SCREEN_WIDTH; // divide world width by screen width to get world scale
// float WORLD_SCALE_Y = WORLD_HEIGHT / SCREEN_HEIGHT; // divide world height by screen height to get world scale
// const float PHYSICS_WORLD_SCALE = PIXELS_PER_METER * SCREEN_SCALE; // get the average of world scale x and y for unuiform scaling
//
// float METRE_WIDTH = WORLD_WIDTH / PIXELS_PER_METER;
// float METRE_HEIGHT = WORLD_HEIGHT / PIXELS_PER_METER;
