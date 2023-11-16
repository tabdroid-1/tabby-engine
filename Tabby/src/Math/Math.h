#pragma once

#include "raylib.h"

namespace Tabby {
class Math {
public:
    static Vector3 MatrixMultiplyVector(Matrix mat, Vector3 vec)
    {
        Vector3 result;
        result.x = mat.m0 * vec.x + mat.m4 * vec.y + mat.m8 * vec.z + mat.m12;
        result.y = mat.m1 * vec.x + mat.m5 * vec.y + mat.m9 * vec.z + mat.m13;
        result.z = mat.m2 * vec.x + mat.m6 * vec.y + mat.m10 * vec.z + mat.m14;
        return result;
    }
};
}

// Vector2 operator+(const Vector2& a, const Vector2& b)
// {
//     return { a.x + b.x, a.y + b.y };
// }
//
// Vector2 operator-(const Vector2& a, const Vector2& b)
// {
//     return { a.x - b.x, a.y - b.y };
// }
//
// Vector3 operator+(const Vector3& a, const Vector3& b)
// {
//     return { a.x + b.x, a.y + b.y, a.z + b.z };
// }
//
// Vector3 operator*(const Vector3& a, const Vector3& b)
// {
//     return { a.x * b.x, a.y * b.y, a.z * b.z };
// }
//
// Vector3 operator/(const Vector3& a, float b)
// {
//     return { a.x / b, a.y / b, a.z / b };
// }
//
// Vector3 operator-(const Vector3& a, const Vector3& b)
// {
//     return { a.x - b.x, a.y - b.y, a.z - b.z };
// }
//
// Vector3 operator+(const Vector3& a, const Vector2& b)
// {
//     return { a.x + b.x, a.y + b.y };
// }
//
// Vector3 operator-(const Vector3& a, const Vector2& b)
// {
//     return { a.x - b.x, a.y - b.y };
// }
//
// Vector4 operator+(const Vector4& a, const Vector4& b)
// {
//     return { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
// }
//
// Vector4 operator-(const Vector4& a, const Vector4& b)
// {
//     return { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
// }
//
// Vector4 operator+(const Vector4& a, const Vector3& b)
// {
//     return { a.x + b.x, a.y + b.y, a.z + b.z };
// }
//
// Vector4 operator-(const Vector4& a, const Vector3& b)
// {
//     return { a.x - b.x, a.y - b.y, a.z - b.z };
// }
//
// Vector4 operator+(const Vector4& a, const Vector2& b)
// {
//     return { a.x + b.x, a.y + b.y };
// }
//
// Vector4 operator-(const Vector4& a, const Vector2& b)
// {
//     return { a.x - b.x, a.y - b.y };
// }
//
// void BoundEncapsulate(BoundingBox Bound, Tabby::GameObject GameObject)
// {
//     Vector3 halfSize = size / 2.0f;
//
//     Vector3 minPoint = point - halfSize;
//     Vector3 maxPoint = point + halfSize;
//
//     if (minPoint.x < min.x)
//         min.x = minPoint.x;
//     if (minPoint.y < min.y)
//         min.y = minPoint.y;
//     if (minPoint.z < min.z)
//         min.z = minPoint.z;
//
//     if (maxPoint.x > max.x)
//         max.x = maxPoint.x;
//     if (maxPoint.y > max.y)
//         max.y = maxPoint.y;
//     if (maxPoint.z > max.z)
//         max.z = maxPoint.z;
// }
