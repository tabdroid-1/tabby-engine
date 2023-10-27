#pragma once

#include "raylib.h"

namespace Tabby {

class Graphics {

public:
    static void DrawSprite(Matrix test, Texture2D texture, Rectangle source, Vector3 position, Vector3 rotation, Vector3 origin, Vector2 size, Color tint);
    // static void DrawSprite2(Matrix test, Texture2D texture, Rectangle source, Vector3 position, Vector3 rotation, Vector3 origin, Vector2 size, Color tint);
    static void Draw3DBillboardRec(Camera camera, Texture2D texture, Rectangle source, Vector3 position, Vector2 size, Color tint);

    static bool IsSphereInFrustrum(Camera Camera, Vector3 Position, float Radius);
};

}
