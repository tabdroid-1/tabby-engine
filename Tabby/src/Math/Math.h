#pragma once

#include "raylib.h"

Vector2 operator+(const Vector2& a, const Vector2& b)
{
    return { a.x + b.x, a.y + b.y };
}

Vector2 operator-(const Vector2& a, const Vector2& b)
{
    return { a.x - b.x, a.y - b.y };
}

Vector3 operator+(const Vector3& a, const Vector3& b)
{
    return { a.x + b.x, a.y + b.y, a.z + b.z };
}

Vector3 operator-(const Vector3& a, const Vector3& b)
{
    return { a.x - b.x, a.y - b.y, a.z - b.z };
}

Vector3 operator+(const Vector3& a, const Vector2& b)
{
    return { a.x + b.x, a.y + b.y };
}

Vector3 operator-(const Vector3& a, const Vector2& b)
{
    return { a.x - b.x, a.y - b.y };
}

Vector4 operator+(const Vector4& a, const Vector4& b)
{
    return { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
}

Vector4 operator-(const Vector4& a, const Vector4& b)
{
    return { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
}

Vector4 operator+(const Vector4& a, const Vector3& b)
{
    return { a.x + b.x, a.y + b.y, a.z + b.z };
}

Vector4 operator-(const Vector4& a, const Vector3& b)
{
    return { a.x - b.x, a.y - b.y, a.z - b.z };
}

Vector4 operator+(const Vector4& a, const Vector2& b)
{
    return { a.x + b.x, a.y + b.y };
}

Vector4 operator-(const Vector4& a, const Vector2& b)
{
    return { a.x - b.x, a.y - b.y };
}
