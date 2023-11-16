#pragma once
#include <raylib.h>

namespace Tabby {

struct CameraComponent;

class CameraTools {

public:
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

    static bool IsSphereInFrustum(Frustum frustum, Vector3 Position, float radius);
    static bool IsAABBoxInFrustum(Frustum frustum, Vector3 min, Vector3 max);
    static bool PointInFrustum(Frustum frustum, Vector3 position);
    // static void ExtractFrustum(Frustum* frustum);
    static Frustum ExtractFrustum();

private:
    static void NormalizePlane(Vector4* plane);
    static float DistanceToPlane(const Vector4* plane, const Vector3* position);

private:
};

}
