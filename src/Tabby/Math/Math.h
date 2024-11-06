#pragma once
#include <Tabby/Foundation/Types.h>

namespace Tabby {

/*template <typename T = float>*/
/*struct Vector2 {*/
/*    T x;*/
/*    T y;*/
/**/
/*    Vector2()*/
/*        : x(0)*/
/*        , y(0)*/
/*    {*/
/*    }*/
/**/
/*    Vector2(T val)*/
/*        : x(val)*/
/*        , y(val)*/
/*    {*/
/*    }*/
/**/
/*    Vector2(T x, T y)*/
/*        : x(x)*/
/*        , y(y)*/
/*    {*/
/*    }*/
/**/
/*    Vector2(const Vector2& vec)*/
/*        : x(vec.x)*/
/*        , y(vec.y)*/
/*    {*/
/*    }*/
/*};*/
/**/
/*template <typename T = float>*/
/*struct Vector3 {*/
/*    T x;*/
/*    T y;*/
/*    T z;*/
/**/
/*    Vector3()*/
/*        : x(0)*/
/*        , y(0)*/
/*        , z(0)*/
/*    {*/
/*    }*/
/**/
/*    Vector3(T val)*/
/*        : x(val)*/
/*        , y(val)*/
/*        , z(val)*/
/*    {*/
/*    }*/
/**/
/*    Vector3(T x, T y, T z)*/
/*        : x(x)*/
/*        , y(y)*/
/*        , z(z)*/
/*    {*/
/*    }*/
/**/
/*    Vector3(const Vector3& vec)*/
/*        : x(vec.x)*/
/*        , y(vec.y)*/
/*        , z(vec.z)*/
/*    {*/
/*    }*/
/**/
/*    Vector3(const Vector2<T>& vec)*/
/*        : x(vec.x)*/
/*        , y(vec.y)*/
/*        , z(0)*/
/*    {*/
/*    }*/
/*};*/
/**/
/*template <typename T = float>*/
/*struct Vector4 {*/
/*    T x;*/
/*    T y;*/
/*    T z;*/
/*    T w;*/
/**/
/*    Vector4()*/
/*        : x(0)*/
/*        , y(0)*/
/*        , z(0)*/
/*        , w(0)*/
/*    {*/
/*    }*/
/**/
/*    Vector4(T val)*/
/*        : x(val)*/
/*        , y(val)*/
/*        , z(val)*/
/*        , w(val)*/
/*    {*/
/*    }*/
/**/
/*    Vector4(T x, T y, T z, T w)*/
/*        : x(x)*/
/*        , y(y)*/
/*        , z(z)*/
/*        , w(w)*/
/*    {*/
/*    }*/
/**/
/*    Vector4(const Vector4& vec)*/
/*        : x(vec.x)*/
/*        , y(vec.y)*/
/*        , z(vec.z)*/
/*        , w(vec.w)*/
/*    {*/
/*    }*/
/**/
/*    Vector4(const Vector3& vec)*/
/*        : x(vec.x)*/
/*        , y(vec.y)*/
/*        , z(vec.z)*/
/*        , w(0)*/
/*    {*/
/*    }*/
/**/
/*    Vector4(const Vector2& vec)*/
/*        : x(vec.x)*/
/*        , y(vec.y)*/
/*        , z(0)*/
/*        , w(0)*/
/*    {*/
/*    }*/
/*};*/
/**/
/*template <typename T = float>*/
/*struct Matrix2 {*/
/*    T mat[4];*/
/**/
/*    Matrix2()*/
/*        : mat { 1, 0, 0, 1 }*/
/*    {*/
/*    }*/
/**/
/*    Matrix2(T diagonal)*/
/*        : mat { diagonal, 0, 0, diagonal }*/
/*    {*/
/*    }*/
/**/
/*    Matrix2(T m00, T m01, T m10, T m11)*/
/*        : mat { m00, m01, m10, m11 }*/
/*    {*/
/*    }*/
/*};*/
/**/
/*template <typename T = float>*/
/*struct Matrix3 {*/
/*    T mat[9];*/
/**/
/*    Matrix3()*/
/*        : mat { 1, 0, 0, 0, 1, 0, 0, 0, 1 }*/
/*    {*/
/*    }*/
/**/
/*    Matrix3(T diagonal)*/
/*        : mat { diagonal, 0, 0, 0, diagonal, 0, 0, 0, diagonal }*/
/*    {*/
/*    }*/
/**/
/*    Matrix3(T m00, T m01, T m02,*/
/*        T m10, T m11, T m12,*/
/*        T m20, T m21, T m22)*/
/*        : mat { m00, m01, m02, m10, m11, m12, m20, m21, m22 }*/
/*    {*/
/*    }*/
/*};*/
/**/
/*template <typename T = float>*/
/*struct Matrix4 {*/
/*    T mat[16];*/
/**/
/*    Matrix4()*/
/*        : mat { 1, 0, 0, 0,*/
/*            0, 1, 0, 0,*/
/*            0, 0, 1, 0,*/
/*            0, 0, 0, 1 }*/
/*    {*/
/*    }*/
/**/
/*    Matrix4(T diagonal)*/
/*        : mat { diagonal, 0, 0, 0,*/
/*            0, diagonal, 0, 0,*/
/*            0, 0, diagonal, 0,*/
/*            0, 0, 0, diagonal }*/
/*    {*/
/*    }*/
/**/
/*    Matrix4(T m00, T m01, T m02, T m03,*/
/*        T m10, T m11, T m12, T m13,*/
/*        T m20, T m21, T m22, T m23,*/
/*        T m30, T m31, T m32, T m33)*/
/*        : mat { m00, m01, m02, m03,*/
/*            m10, m11, m12, m13,*/
/*            m20, m21, m22, m23,*/
/*            m30, m31, m32, m33 }*/
/*    {*/
/*    }*/
/*};*/
/**/
/*struct Quaternion {*/
/*    float x, y, z, w;*/
/**/
/*    Quaternion()*/
/*        : x(0.0f)*/
/*        , y(0.0f)*/
/*        , z(0.0f)*/
/*        , w(1.0f)*/
/*    {*/
/*    }*/
/**/
/*    Quaternion(float x, float y, float z, float w)*/
/*        : x(x)*/
/*        , y(y)*/
/*        , z(z)*/
/*        , w(w)*/
/*    {*/
/*    }*/
/**/
/*    Quaternion(const Vector3& axis, float angle)*/
/*    {*/
/*        float half_angle = angle * 0.5f;*/
/*        float s = std::sin(half_angle);*/
/*        x = axis.x * s;*/
/*        y = axis.y * s;*/
/*        z = axis.z * s;*/
/*        w = std::cos(half_angle);*/
/*    }*/
/*};*/

namespace Math {
    const float PI = 3.14159265358979323846f;
    const float DEG2RAD = (PI / 180.0f);
    const float RAD2DEG = (180.0f / PI);
    const float EPSILON = 0.000001f;

    template <typename T>
    inline bool IsPowerOf2(T value)
    {
        if (!std::is_integral_v<T>)
            return false;

        bool result = value && ((value & (value - 1)) == 0);
        return result;
    }

    inline float Abs(float f)
    {
        if (f > 0)
            return f;
        else
            return f * -1;
    }

    inline float Cross(const Vector2& a, const Vector2& b)
    {
        return a.x * b.y - a.y * b.x;
    }

    inline Vector3 Cross(const Vector3& a, const Vector3& b)
    {
        return Vector3 {
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        };
    }

    inline float Dot(const Vector2& a, const Vector2& b)
    {
        return a.x * b.x + a.y * b.y;
    }

    inline float Dot(const Vector3& a, const Vector3& b)
    {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    inline float Dot(const Vector4& a, const Vector4& b)
    {
        return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    }

    inline void Normalize(Vector2& v)
    {
        float len = std::sqrt(v.x * v.x + v.y * v.y);

        if (len > EPSILON) {
            float invLen = 1.0f / len;
            v.x *= invLen;
            v.y *= invLen;
        }
    }

    inline void Normalize(Vector3& v)
    {
        float len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);

        if (len > EPSILON) {
            float invLen = 1.0f / len;
            v.x *= invLen;
            v.y *= invLen;
            v.z *= invLen;
        }
    }

    inline void Normalize(Vector4& v)
    {
        float len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);

        if (len > EPSILON) {
            float invLen = 1.0f / len;
            v.x *= invLen;
            v.y *= invLen;
            v.z *= invLen;
            v.w *= invLen;
        }
    }

}
}
