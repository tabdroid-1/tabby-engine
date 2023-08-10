#pragma once

#include <iostream>
#include <map>
#include <raylib.h>

#include "ComponentSystem/Component.h"

class C_Camera : public Component {
public:
    C_Camera(GameObject* owner);

    void Load();
    void Load(Vector2 offset);

    void Update(float deltaTime) override;
    void Draw() override;

public:
    void SetRotation(float rotation) { camera.rotation = rotation; }
    void SetZoom(float zoom) { camera.zoom = zoom; }
    void SetOffest(Vector2 offset) { camera.offset = offset; }

    const Camera2D& GetCamera() { return camera; }
    void UpdateCamera(std::map<std::string, Camera2D>& cameraHashMap, std::string name);

private:
    Camera2D camera = { 0 };
};
