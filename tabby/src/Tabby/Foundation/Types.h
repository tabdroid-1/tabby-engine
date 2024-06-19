#pragma once

#include <Tabby/Core/UUID.h>
#include <glm/glm.hpp>

namespace Tabby {

typedef uint8_t byte;
typedef float float32;
typedef double float64;

using BitMask = uint64_t;
using Flag = uint64_t;
using AssetHandle = UUID;

struct MiscData {
    byte* data;
    uint32_t size;
};

using RGB32 = glm::u8vec3;
using RGBA32 = glm::u8vec4;

struct Sphere {
    glm::vec3 center;
    float32 radius;
};

struct AABB {
    glm::vec3 min, max;
};

struct AABB_2D {
    glm::vec2 min, max;
};

struct Bounds {
    Sphere sphere;
    AABB aabb;
};

struct Plane {
    glm::vec3 normal;
    float32 distance;

    Plane()
    {
        normal = { 0.0f, 1.0f, 0.0f };
        distance = 0.0f;
    }

    Plane(const glm::vec3& p1, const glm::vec3& norm)
        : normal(glm::normalize(norm))
        , distance(glm::dot(normal, p1))
    {
    }
};

template <typename T = float>
struct vec2 {
    union {
        T values[2];
        struct {
            union {
                T x, r;
            };
            union {
                T y, g;
            };
        };
    };

    vec2() = default;

    vec2(T value)
    {
        x = value;
        y = value;
    }

    vec2(const T& x, const T& y)
    {
        this->x = x;
        this->y = y;
    }

    vec2(const glm::vec<2, T, glm::qualifier::defaultp>& other)
    {
        x = other.x;
        y = other.y;
    }

    // template <typename T>
    explicit constexpr operator vec2<T>() const { return { (T)x, (T)y }; }
    vec2 operator+(const vec2& other) { return { x + other.x, y + other.y }; }
    vec2 operator-(const vec2& other) { return { x - other.x, y - other.y }; }

    vec2& operator+=(const vec2& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    vec2& operator-=(const vec2& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    operator glm::vec<2, T, glm::defaultp>()
    {
        return glm::vec<2, T, glm::defaultp>(x, y);
    }
};

template <typename T = float>
struct vec3 {
    union {
        T x, r;
    };
    union {
        T y, g;
    };
    union {
        T z, b;
    };

    vec3() = default;

    vec3(T value)
    {
        x = value;
        y = value;
        z = value;
    }

    vec3(const T& x, const T& y, const T& z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    vec3(const glm::vec<3, T, glm::qualifier::defaultp>& other)
    {
        x = other.x;
        y = other.y;
    }

    // template <typename T>
    explicit constexpr operator vec3<T>() const { return { T(x), T(y), T(z) }; }
    vec3 operator+(const vec3& other) const { return { x + other.x, y + other.y, z + other.z }; }
    vec3 operator-(const vec3& other) const { return { x - other.x, y - other.y, z - other.z }; }

    vec3& operator+=(const vec3& other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    vec3& operator-=(const vec3& other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    operator glm::vec<3, T, glm::defaultp>()
    {
        return glm::vec<3, T, glm::defaultp>(x, y, z);
    }
};

template <typename T = float>
struct vec4 {
    union {
        T x, r;
    };
    union {
        T y, g;
    };
    union {
        T z, b;
    };
    union {
        T w, a;
    };

    vec4() = default;

    vec4(T value)
    {
        x = value;
        y = value;
        z = value;
        w = value;
    }

    vec4(const T& x, const T& y, const T& z, const T& w)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }

    vec4(const glm::vec<4, T, glm::qualifier::defaultp>& other)
    {
        x = other.x;
        y = other.y;
        z = other.z;
        w = other.w;
    }

    // template <typename T>
    explicit constexpr operator vec4<T>() const { return { (T)x, (T)y, T(z), (T)w }; }

    vec4 operator+(const vec4& other) { return { x + other.x, y + other.y, z + other.z, w + other.w }; }
    vec4 operator-(const vec4& other) { return { x - other.x, y - other.y, z - other.z, w - other.w }; }

    vec4& operator+=(const vec4& other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        w -= other.w;
        return *this;
    }

    vec4& operator-=(const vec4& other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        w -= other.w;
        return *this;
    }

    operator glm::vec<4, T, glm::defaultp>()
    {
        return glm::vec<4, T, glm::defaultp>(x, y, z, w);
    }
};

using IntVector2 = vec2<int32_t>;
using IntVector3 = vec3<int32_t>;
using IntVector4 = vec4<int32_t>;

using UIntVector2 = vec2<uint32_t>;
using UIntVector3 = vec3<uint32_t>;
using UIntVector4 = vec4<uint32_t>;

using Vector2 = vec2<float>;
using Vector3 = vec3<float>;
using Vector4 = vec4<float>;

class Frustum {
public:
    Frustum() { }

    // m = ProjectionMatrix * ViewMatrix
    Frustum(const glm::mat4& ProjectionMatrix, const glm::mat4& transform)
    {
        glm::mat4 m = ProjectionMatrix * glm::inverse(transform);
        m = glm::transpose(m);
        m_planes[Left] = m[3] + m[0];
        m_planes[Right] = m[3] - m[0];
        m_planes[Bottom] = m[3] + m[1];
        m_planes[Top] = m[3] - m[1];
        m_planes[Near] = m[3] + m[2];
        m_planes[Far] = m[3] - m[2];

        glm::vec3 crosses[Combinations] = {
            glm::cross(glm::vec3(m_planes[Left]), glm::vec3(m_planes[Right])),
            glm::cross(glm::vec3(m_planes[Left]), glm::vec3(m_planes[Bottom])),
            glm::cross(glm::vec3(m_planes[Left]), glm::vec3(m_planes[Top])),
            glm::cross(glm::vec3(m_planes[Left]), glm::vec3(m_planes[Near])),
            glm::cross(glm::vec3(m_planes[Left]), glm::vec3(m_planes[Far])),
            glm::cross(glm::vec3(m_planes[Right]), glm::vec3(m_planes[Bottom])),
            glm::cross(glm::vec3(m_planes[Right]), glm::vec3(m_planes[Top])),
            glm::cross(glm::vec3(m_planes[Right]), glm::vec3(m_planes[Near])),
            glm::cross(glm::vec3(m_planes[Right]), glm::vec3(m_planes[Far])),
            glm::cross(glm::vec3(m_planes[Bottom]), glm::vec3(m_planes[Top])),
            glm::cross(glm::vec3(m_planes[Bottom]), glm::vec3(m_planes[Near])),
            glm::cross(glm::vec3(m_planes[Bottom]), glm::vec3(m_planes[Far])),
            glm::cross(glm::vec3(m_planes[Top]), glm::vec3(m_planes[Near])),
            glm::cross(glm::vec3(m_planes[Top]), glm::vec3(m_planes[Far])),
            glm::cross(glm::vec3(m_planes[Near]), glm::vec3(m_planes[Far]))
        };

        m_points[0] = intersection<Left, Bottom, Near>(crosses);
        m_points[1] = intersection<Left, Top, Near>(crosses);
        m_points[2] = intersection<Right, Bottom, Near>(crosses);
        m_points[3] = intersection<Right, Top, Near>(crosses);
        m_points[4] = intersection<Left, Bottom, Far>(crosses);
        m_points[5] = intersection<Left, Top, Far>(crosses);
        m_points[6] = intersection<Right, Bottom, Far>(crosses);
        m_points[7] = intersection<Right, Top, Far>(crosses);
    }

    bool IsBoxVisible(const glm::vec3& minp, const glm::vec3& maxp) const
    {
        // check box outside/inside of frustum
        for (int i = 0; i < Count; i++) {
            // clang-format off
            if ((glm::dot(m_planes[i], glm::vec4(minp.x, minp.y, minp.z, 1.0f)) < 0.0) &&
                (glm::dot(m_planes[i], glm::vec4(maxp.x, minp.y, minp.z, 1.0f)) < 0.0) &&
                (glm::dot(m_planes[i], glm::vec4(minp.x, maxp.y, minp.z, 1.0f)) < 0.0) &&
                (glm::dot(m_planes[i], glm::vec4(maxp.x, maxp.y, minp.z, 1.0f)) < 0.0) &&
                (glm::dot(m_planes[i], glm::vec4(minp.x, minp.y, maxp.z, 1.0f)) < 0.0) &&
                (glm::dot(m_planes[i], glm::vec4(maxp.x, minp.y, maxp.z, 1.0f)) < 0.0) &&
                (glm::dot(m_planes[i], glm::vec4(minp.x, maxp.y, maxp.z, 1.0f)) < 0.0) &&
                (glm::dot(m_planes[i], glm::vec4(maxp.x, maxp.y, maxp.z, 1.0f)) < 0.0))
            {
                return false;
            }
            // clang-format on
        }

        // check frustum outside/inside box
        // clang-format off
        int out;
        out = 0; for (int i = 0; i<8; i++) out += ((m_points[i].x > maxp.x) ? 1 : 0); if (out == 8) return false;
        out = 0; for (int i = 0; i<8; i++) out += ((m_points[i].x < minp.x) ? 1 : 0); if (out == 8) return false;
        out = 0; for (int i = 0; i<8; i++) out += ((m_points[i].y > maxp.y) ? 1 : 0); if (out == 8) return false;
        out = 0; for (int i = 0; i<8; i++) out += ((m_points[i].y < minp.y) ? 1 : 0); if (out == 8) return false;
        out = 0; for (int i = 0; i<8; i++) out += ((m_points[i].z > maxp.z) ? 1 : 0); if (out == 8) return false;
        out = 0; for (int i = 0; i<8; i++) out += ((m_points[i].z < minp.z) ? 1 : 0); if (out == 8) return false;
        // clang-format on

        return true;
    }

    glm::vec3* GetPoints() { return m_points; }

private:
    enum Planes {
        Left = 0,
        Right,
        Bottom,
        Top,
        Near,
        Far,
        Count,
        Combinations = Count * (Count - 1) / 2
    };

    template <Planes i, Planes j>
    struct ij2k {
        enum { k = i * (9 - i) / 2 + j - 1 };
    };

    // template <Planes a, Planes b, Planes c>
    // glm::vec3 intersection(const glm::vec3* crosses) const;
    template <Frustum::Planes a, Frustum::Planes b, Frustum::Planes c>
    inline glm::vec3 intersection(const glm::vec3* crosses) const
    {
        float D = glm::dot(glm::vec3(m_planes[a]), crosses[ij2k<b, c>::k]);
        glm::vec3 res = glm::mat3(crosses[ij2k<b, c>::k], -crosses[ij2k<a, c>::k], crosses[ij2k<a, b>::k]) * glm::vec3(m_planes[a].w, m_planes[b].w, m_planes[c].w);
        return res * (-1.0f / D);
    }

    glm::vec4 m_planes[Count];
    glm::vec3 m_points[8];
};

// Macro table is usually being iterated, so I use an array of pairs and not a map, because map
// fits better for random access
using ShaderMacroTable = std::map<std::string, std::string>;

// Non-owning raw buffer class
struct Buffer {
    uint8_t* Data = nullptr;
    uint64_t Size = 0;

    Buffer() = default;

    Buffer(uint64_t size)
    {
        Allocate(size);
    }

    Buffer(const void* data, uint64_t size)
        : Data((uint8_t*)data)
        , Size(size)
    {
    }

    Buffer(const Buffer&) = default;

    static Buffer Copy(Buffer other)
    {
        Buffer result(other.Size);
        memcpy(result.Data, other.Data, other.Size);
        return result;
    }

    void Allocate(uint64_t size)
    {
        Release();

        Data = (uint8_t*)malloc(size);
        Size = size;
    }

    void Release()
    {
        free(Data);
        Data = nullptr;
        Size = 0;
    }

    template <typename T>
    T* As()
    {
        return (T*)Data;
    }

    operator bool() const
    {
        return (bool)Data;
    }
};

struct ScopedBuffer {
    ScopedBuffer(Buffer buffer)
        : m_Buffer(buffer)
    {
    }

    ScopedBuffer(uint64_t size)
        : m_Buffer(size)
    {
    }

    ~ScopedBuffer()
    {
        m_Buffer.Release();
    }

    uint8_t* Data() { return m_Buffer.Data; }
    uint64_t Size() { return m_Buffer.Size; }

    template <typename T>
    T* As()
    {
        return m_Buffer.As<T>();
    }

    operator bool() const { return m_Buffer; }

private:
    Buffer m_Buffer;
};

}

// #pragma once
//
// #include <Tabby/Core/UUID.h>
// #include <glm/glm.hpp>
//
// namespace Tabby {
//
// typedef uint8_t byte;
// typedef float float32;
// typedef double float64;
//
// using BitMask = uint64_t;
// using Flag = uint64_t;
// using AssetHandle = UUID;
//
// struct MiscData {
//     byte* data;
//     uint32_t size;
// };
//
// using RGB32 = glm::u8vec3;
// using RGBA32 = glm::u8vec4;
//
// struct Sphere {
//     glm::vec3 center;
//     float32 radius;
// };
//
// struct AABB {
//     glm::vec3 min, max;
// };
//
// struct AABB_2D {
//     glm::vec2 min, max;
// };
//
// struct Bounds {
//     Sphere sphere;
//     AABB aabb;
// };
//
// struct Plane {
//     glm::vec3 normal;
//     float32 distance;
//
//     Plane()
//     {
//         normal = { 0.0f, 1.0f, 0.0f };
//         distance = 0.0f;
//     }
//
//     Plane(const glm::vec3& p1, const glm::vec3& norm)
//         : normal(glm::normalize(norm))
//         , distance(glm::dot(normal, p1))
//     {
//     }
// };
//
// class Frustum {
// public:
//     Frustum() { }
//
//     // m = ProjectionMatrix * ViewMatrix
//     Frustum(const glm::mat4& ProjectionMatrix, const glm::mat4& transform)
//     {
//         glm::mat4 m = ProjectionMatrix * glm::inverse(transform);
//         m = glm::transpose(m);
//         m_planes[Left] = m[3] + m[0];
//         m_planes[Right] = m[3] - m[0];
//         m_planes[Bottom] = m[3] + m[1];
//         m_planes[Top] = m[3] - m[1];
//         m_planes[Near] = m[3] + m[2];
//         m_planes[Far] = m[3] - m[2];
//
//         glm::vec3 crosses[Combinations] = {
//             glm::cross(glm::vec3(m_planes[Left]), glm::vec3(m_planes[Right])),
//             glm::cross(glm::vec3(m_planes[Left]), glm::vec3(m_planes[Bottom])),
//             glm::cross(glm::vec3(m_planes[Left]), glm::vec3(m_planes[Top])),
//             glm::cross(glm::vec3(m_planes[Left]), glm::vec3(m_planes[Near])),
//             glm::cross(glm::vec3(m_planes[Left]), glm::vec3(m_planes[Far])),
//             glm::cross(glm::vec3(m_planes[Right]), glm::vec3(m_planes[Bottom])),
//             glm::cross(glm::vec3(m_planes[Right]), glm::vec3(m_planes[Top])),
//             glm::cross(glm::vec3(m_planes[Right]), glm::vec3(m_planes[Near])),
//             glm::cross(glm::vec3(m_planes[Right]), glm::vec3(m_planes[Far])),
//             glm::cross(glm::vec3(m_planes[Bottom]), glm::vec3(m_planes[Top])),
//             glm::cross(glm::vec3(m_planes[Bottom]), glm::vec3(m_planes[Near])),
//             glm::cross(glm::vec3(m_planes[Bottom]), glm::vec3(m_planes[Far])),
//             glm::cross(glm::vec3(m_planes[Top]), glm::vec3(m_planes[Near])),
//             glm::cross(glm::vec3(m_planes[Top]), glm::vec3(m_planes[Far])),
//             glm::cross(glm::vec3(m_planes[Near]), glm::vec3(m_planes[Far]))
//         };
//
//         m_points[0] = intersection<Left, Bottom, Near>(crosses);
//         m_points[1] = intersection<Left, Top, Near>(crosses);
//         m_points[2] = intersection<Right, Bottom, Near>(crosses);
//         m_points[3] = intersection<Right, Top, Near>(crosses);
//         m_points[4] = intersection<Left, Bottom, Far>(crosses);
//         m_points[5] = intersection<Left, Top, Far>(crosses);
//         m_points[6] = intersection<Right, Bottom, Far>(crosses);
//         m_points[7] = intersection<Right, Top, Far>(crosses);
//     }
//
//     bool IsBoxVisible(const glm::vec3& minp, const glm::vec3& maxp) const
//     {
//         // check box outside/inside of frustum
//         for (int i = 0; i < Count; i++) {
//             // clang-format off
//             if ((glm::dot(m_planes[i], glm::vec4(minp.x, minp.y, minp.z, 1.0f)) < 0.0) &&
//                 (glm::dot(m_planes[i], glm::vec4(maxp.x, minp.y, minp.z, 1.0f)) < 0.0) &&
//                 (glm::dot(m_planes[i], glm::vec4(minp.x, maxp.y, minp.z, 1.0f)) < 0.0) &&
//                 (glm::dot(m_planes[i], glm::vec4(maxp.x, maxp.y, minp.z, 1.0f)) < 0.0) &&
//                 (glm::dot(m_planes[i], glm::vec4(minp.x, minp.y, maxp.z, 1.0f)) < 0.0) &&
//                 (glm::dot(m_planes[i], glm::vec4(maxp.x, minp.y, maxp.z, 1.0f)) < 0.0) &&
//                 (glm::dot(m_planes[i], glm::vec4(minp.x, maxp.y, maxp.z, 1.0f)) < 0.0) &&
//                 (glm::dot(m_planes[i], glm::vec4(maxp.x, maxp.y, maxp.z, 1.0f)) < 0.0))
//             {
//                 return false;
//             }
//             // clang-format on
//         }
//
//         // check frustum outside/inside box
//         // clang-format off
//         int out;
//         out = 0; for (int i = 0; i<8; i++) out += ((m_points[i].x > maxp.x) ? 1 : 0); if (out == 8) return false;
//         out = 0; for (int i = 0; i<8; i++) out += ((m_points[i].x < minp.x) ? 1 : 0); if (out == 8) return false;
//         out = 0; for (int i = 0; i<8; i++) out += ((m_points[i].y > maxp.y) ? 1 : 0); if (out == 8) return false;
//         out = 0; for (int i = 0; i<8; i++) out += ((m_points[i].y < minp.y) ? 1 : 0); if (out == 8) return false;
//         out = 0; for (int i = 0; i<8; i++) out += ((m_points[i].z > maxp.z) ? 1 : 0); if (out == 8) return false;
//         out = 0; for (int i = 0; i<8; i++) out += ((m_points[i].z < minp.z) ? 1 : 0); if (out == 8) return false;
//         // clang-format on
//
//         return true;
//     }
//
//     glm::vec3* GetPoints() { return m_points; }
//
// private:
//     enum Planes {
//         Left = 0,
//         Right,
//         Bottom,
//         Top,
//         Near,
//         Far,
//         Count,
//         Combinations = Count * (Count - 1) / 2
//     };
//
//     template <Planes i, Planes j>
//     struct ij2k {
//         enum { k = i * (9 - i) / 2 + j - 1 };
//     };
//
//     // template <Planes a, Planes b, Planes c>
//     // glm::vec3 intersection(const glm::vec3* crosses) const;
//     template <Frustum::Planes a, Frustum::Planes b, Frustum::Planes c>
//     inline glm::vec3 intersection(const glm::vec3* crosses) const
//     {
//         float D = glm::dot(glm::vec3(m_planes[a]), crosses[ij2k<b, c>::k]);
//         glm::vec3 res = glm::mat3(crosses[ij2k<b, c>::k], -crosses[ij2k<a, c>::k], crosses[ij2k<a, b>::k]) * glm::vec3(m_planes[a].w, m_planes[b].w, m_planes[c].w);
//         return res * (-1.0f / D);
//     }
//
//     glm::vec4 m_planes[Count];
//     glm::vec3 m_points[8];
// };
//
// // Macro table is usually being iterated, so I use an array of pairs and not a map, because map
// // fits better for random access
// using ShaderMacroTable = std::map<std::string, std::string>;
//
// // Non-owning raw buffer class
// struct Buffer {
//     uint8_t* Data = nullptr;
//     uint64_t Size = 0;
//
//     Buffer() = default;
//
//     Buffer(uint64_t size)
//     {
//         Allocate(size);
//     }
//
//     Buffer(const void* data, uint64_t size)
//         : Data((uint8_t*)data)
//         , Size(size)
//     {
//     }
//
//     Buffer(const Buffer&) = default;
//
//     static Buffer Copy(Buffer other)
//     {
//         Buffer result(other.Size);
//         memcpy(result.Data, other.Data, other.Size);
//         return result;
//     }
//
//     void Allocate(uint64_t size)
//     {
//         Release();
//
//         Data = (uint8_t*)malloc(size);
//         Size = size;
//     }
//
//     void Release()
//     {
//         free(Data);
//         Data = nullptr;
//         Size = 0;
//     }
//
//     template <typename T>
//     T* As()
//     {
//         return (T*)Data;
//     }
//
//     operator bool() const
//     {
//         return (bool)Data;
//     }
// };
//
// struct ScopedBuffer {
//     ScopedBuffer(Buffer buffer)
//         : m_Buffer(buffer)
//     {
//     }
//
//     ScopedBuffer(uint64_t size)
//         : m_Buffer(size)
//     {
//     }
//
//     ~ScopedBuffer()
//     {
//         m_Buffer.Release();
//     }
//
//     uint8_t* Data() { return m_Buffer.Data; }
//     uint64_t Size() { return m_Buffer.Size; }
//
//     template <typename T>
//     T* As()
//     {
//         return m_Buffer.As<T>();
//     }
//
//     operator bool() const { return m_Buffer; }
//
// private:
//     Buffer m_Buffer;
// };
//
// // template <typename T = float32>
// // struct vec2 {
// //     union {
// //         T values[2];
// //         struct {
// //             union {
// //                 T x, r;
// //             };
// //             union {
// //                 T y, g;
// //             };
// //         };
// //     };
// //
// //     vec2() = default;
// //
// //     vec2(T value)
// //     {
// //         x = value;
// //         y = value;
// //     }
// //
// //     vec2(const T& x, const T& y)
// //     {
// //         this->x = x;
// //         this->y = y;
// //     }
// //
// //     vec2(const glm::vec<2, T, glm::qualifier::defaultp>& other)
// //     {
// //         x = other.x;
// //         y = other.y;
// //     }
// //
// //     template <typename T>
// //     explicit constexpr operator vec2<T>() const { return { static_cast<T>(x), static_cast<T>(y) }; }
// //     vec2 operator+(const vec2& other) { return { x + other.x, y + other.y }; }
// //     vec2 operator-(const vec2& other) { return { x - other.x, y - other.y }; }
// //     vec2& operator+=(const vec2& other)
// //     {
// //         this = this + other;
// //         return *this;
// //     }
// //     vec2& operator-=(const vec2& other)
// //     {
// //         this = this - other;
// //         return *this;
// //     }
// //
// //     operator glm::vec<2, T, glm::defaultp>()
// //     {
// //         return glm::vec<2, T, glm::defaultp>(x, y);
// //     }
// // };
//
// // template <typename T = float32>
// // struct vec3 {
// //     union {
// //         T x, r;
// //     };
// //     union {
// //         T y, g;
// //     };
// //     union {
// //         T z, b;
// //     };
// //
// //     vec3() = default;
// //
// //     vec3(T value)
// //     {
// //         x = value;
// //         y = value;
// //         z = value;
// //     }
// //
// //     vec3(const T& x, const T& y, const T& z)
// //     {
// //         this->x = x;
// //         this->y = y;
// //         this->z = z;
// //     }
// //
// //     vec3(const glm::vec<3, T, glm::qualifier::defaultp>& other)
// //     {
// //         x = other.x;
// //         y = other.y;
// //     }
// //
// //     template <typename T>
// //     explicit constexpr operator vec3<T>() const { return { T(x), T(y), T(z) }; }
// //     vec3 operator+(const vec3& other) { return { x + other.x, y + other.y, z + other.z }; }
// //     vec3 operator-(const vec3& other) { return { x - other.x, y - other.y, z - other.z }; }
// //     vec3& operator+=(const vec3& other)
// //     {
// //         this = this + other;
// //         return *this;
// //     }
// //     vec3& operator-=(const vec3& other)
// //     {
// //         this = this - other;
// //         return *this;
// //     }
// //
// //     operator glm::vec<3, T, glm::defaultp>()
// //     {
// //         return glm::vec<3, T, glm::defaultp>(x, y, z);
// //     }
// // };
//
// // template <typename T = float32>
// // struct vec4 {
// //     union {
// //         T x, r;
// //     };
// //     union {
// //         T y, g;
// //     };
// //     union {
// //         T z, b;
// //     };
// //     union {
// //         T w, a;
// //     };
// //
// //     vec4() = default;
// //
// //     vec4(T value)
// //     {
// //         x = value;
// //         y = value;
// //         z = value;
// //         w = value;
// //     }
// //
// //     vec4(const T& x, const T& y, const T& z, const T& w)
// //     {
// //         this->x = x;
// //         this->y = y;
// //         this->z = z;
// //         this->w = w;
// //     }
// //
// //     vec4(const glm::vec<4, T, glm::qualifier::defaultp>& other)
// //     {
// //         x = other.x;
// //         y = other.y;
// //         z = other.z;
// //         w = other.w;
// //     }
// //
// //     template <typename T>
// //     explicit constexpr operator vec4<T>() const { return { (T)x, (T)y, T(z), (T)w }; }
// //
// //     vec4 operator+(const vec4& other) { return { x + other.x, y + other.y, z + other.z, w + other.w }; }
// //     vec4 operator-(const vec4& other) { return { x - other.x, y - other.y, z - other.z, w - other.w }; }
// //     vec4& operator+=(const vec4& other)
// //     {
// //         this = this + other;
// //         return *this;
// //     }
// //     vec4& operator-=(const vec4& other)
// //     {
// //         this = this - other;
// //         return *this;
// //     }
// //
// //     operator glm::vec<4, T, glm::defaultp>()
// //     {
// //         return glm::vec<4, T, glm::defaultp>(x, y, z, w);
// //     }
// // };
//
// // using ivec2 = vec2<int32>;
// // using ivec3 = vec3<int32>;
// // using ivec4 = vec4<int32>;
// //
// // using uvec2 = vec2<uint32>;
// // using uvec3 = vec3<uint32>;
// // using uvec4 = vec4<uint32>;
// //
// // using fvec2 = vec2<float32>;
// // using fvec3 = vec3<float32>;
// // using fvec4 = vec4<float32>;
// }
