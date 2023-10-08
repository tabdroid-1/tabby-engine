#pragma once

#include "raylib.h"

namespace Tabby {

class Graphics {

public:
    static void DrawSprite(Matrix test, Texture2D texture, Rectangle source, Vector3 position, Vector3 rotation, Vector2 size, Color tint);
    static void Draw3DBillboardRec(Camera camera, Texture2D texture, Rectangle source, Vector3 position, Vector2 size, Color tint);

    static void TransformCamera(int mode, Vector3 position, Vector3 rotation);
};

}
