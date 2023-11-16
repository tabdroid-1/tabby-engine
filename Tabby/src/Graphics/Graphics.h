#pragma once

#include "raylib.h"

namespace Tabby {

typedef enum {
    Back = 0,
    Front = 1,
    Bottom = 2,
    Top = 3,
    Right = 4,
    Left = 5,
    MAX = 6
} FrustumPlanes;

typedef struct
{
    Vector4 Planes[6];
} Frustum;

class Graphics {

public:
    static void DrawSprite(Matrix test, Texture2D texture, Rectangle source, Vector3 position, Vector3 rotation, Vector3 origin, Vector2 size, Color tint);
    // static void DrawSprite2(Matrix test, Texture2D texture, Rectangle source, Vector3 position, Vector3 rotation, Vector3 origin, Vector2 size, Color tint);
    static void Draw3DBillboardRec(Camera camera, Texture2D texture, Rectangle source, Vector3 position, Vector2 size, Color tint);

    static bool IsSphereInFrustum(Camera camera, Vector3 center, float radius);
    // static bool IsSphereInFrustum(Camera Camera, Vector3 Position, float Radius);

private:
    static void ExtractFrustum(Frustum* frustum);
};

}
