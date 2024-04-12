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

struct Frustum {
    Plane planes[6] = {}; // top, bottom, right, left, far, near planes
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

// template <typename T = float32>
// struct vec2 {
//     union {
//         T values[2];
//         struct {
//             union {
//                 T x, r;
//             };
//             union {
//                 T y, g;
//             };
//         };
//     };
//
//     vec2() = default;
//
//     vec2(T value)
//     {
//         x = value;
//         y = value;
//     }
//
//     vec2(const T& x, const T& y)
//     {
//         this->x = x;
//         this->y = y;
//     }
//
//     vec2(const glm::vec<2, T, glm::qualifier::defaultp>& other)
//     {
//         x = other.x;
//         y = other.y;
//     }
//
//     template <typename T>
//     explicit constexpr operator vec2<T>() const { return { static_cast<T>(x), static_cast<T>(y) }; }
//     vec2 operator+(const vec2& other) { return { x + other.x, y + other.y }; }
//     vec2 operator-(const vec2& other) { return { x - other.x, y - other.y }; }
//     vec2& operator+=(const vec2& other)
//     {
//         this = this + other;
//         return *this;
//     }
//     vec2& operator-=(const vec2& other)
//     {
//         this = this - other;
//         return *this;
//     }
//
//     operator glm::vec<2, T, glm::defaultp>()
//     {
//         return glm::vec<2, T, glm::defaultp>(x, y);
//     }
// };

// template <typename T = float32>
// struct vec3 {
//     union {
//         T x, r;
//     };
//     union {
//         T y, g;
//     };
//     union {
//         T z, b;
//     };
//
//     vec3() = default;
//
//     vec3(T value)
//     {
//         x = value;
//         y = value;
//         z = value;
//     }
//
//     vec3(const T& x, const T& y, const T& z)
//     {
//         this->x = x;
//         this->y = y;
//         this->z = z;
//     }
//
//     vec3(const glm::vec<3, T, glm::qualifier::defaultp>& other)
//     {
//         x = other.x;
//         y = other.y;
//     }
//
//     template <typename T>
//     explicit constexpr operator vec3<T>() const { return { T(x), T(y), T(z) }; }
//     vec3 operator+(const vec3& other) { return { x + other.x, y + other.y, z + other.z }; }
//     vec3 operator-(const vec3& other) { return { x - other.x, y - other.y, z - other.z }; }
//     vec3& operator+=(const vec3& other)
//     {
//         this = this + other;
//         return *this;
//     }
//     vec3& operator-=(const vec3& other)
//     {
//         this = this - other;
//         return *this;
//     }
//
//     operator glm::vec<3, T, glm::defaultp>()
//     {
//         return glm::vec<3, T, glm::defaultp>(x, y, z);
//     }
// };

// template <typename T = float32>
// struct vec4 {
//     union {
//         T x, r;
//     };
//     union {
//         T y, g;
//     };
//     union {
//         T z, b;
//     };
//     union {
//         T w, a;
//     };
//
//     vec4() = default;
//
//     vec4(T value)
//     {
//         x = value;
//         y = value;
//         z = value;
//         w = value;
//     }
//
//     vec4(const T& x, const T& y, const T& z, const T& w)
//     {
//         this->x = x;
//         this->y = y;
//         this->z = z;
//         this->w = w;
//     }
//
//     vec4(const glm::vec<4, T, glm::qualifier::defaultp>& other)
//     {
//         x = other.x;
//         y = other.y;
//         z = other.z;
//         w = other.w;
//     }
//
//     template <typename T>
//     explicit constexpr operator vec4<T>() const { return { (T)x, (T)y, T(z), (T)w }; }
//
//     vec4 operator+(const vec4& other) { return { x + other.x, y + other.y, z + other.z, w + other.w }; }
//     vec4 operator-(const vec4& other) { return { x - other.x, y - other.y, z - other.z, w - other.w }; }
//     vec4& operator+=(const vec4& other)
//     {
//         this = this + other;
//         return *this;
//     }
//     vec4& operator-=(const vec4& other)
//     {
//         this = this - other;
//         return *this;
//     }
//
//     operator glm::vec<4, T, glm::defaultp>()
//     {
//         return glm::vec<4, T, glm::defaultp>(x, y, z, w);
//     }
// };

// using ivec2 = vec2<int32>;
// using ivec3 = vec3<int32>;
// using ivec4 = vec4<int32>;
//
// using uvec2 = vec2<uint32>;
// using uvec3 = vec3<uint32>;
// using uvec4 = vec4<uint32>;
//
// using fvec2 = vec2<float32>;
// using fvec3 = vec3<float32>;
// using fvec4 = vec4<float32>;

}
