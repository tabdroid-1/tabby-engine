#pragma once
#include <tbpch.h>
#include <Tabby/Debug/Instrumentor.h>
#include <Tabby/Core/UUID.h>

#include <glm/glm.hpp>

namespace Tabby {

typedef uint8_t byte;

using BitMask = uint64_t;
using Flag = uint64_t;
using AssetHandle = UUID;

using IntVector2 = glm::vec<2, int>;
using IntVector3 = glm::vec<3, int>;
using IntVector4 = glm::vec<4, int>;

using UIntVector2 = glm::vec<2, uint32_t>;
using UIntVector3 = glm::vec<3, uint32_t>;
using UIntVector4 = glm::vec<4, uint32_t>;

using Vector2 = glm::vec2;
using Vector3 = glm::vec3;
using Vector4 = glm::vec4;

using Matrix2 = glm::mat2;
using Matrix3 = glm::mat3;
using Matrix4 = glm::mat4;

using RGB32 = glm::u8vec3;
using RGBA32 = glm::u8vec4;

using Quaternion = glm::quat;

struct Sphere {
    Vector3 center;
    float radius;
};

struct Circle {
    Vector2 center;
    float radius;
};

struct AABB {
    Vector3 min, max;
};

struct AABB2D {
    Vector2 min, max;
};

struct Bounds {
    Sphere sphere;
    AABB aabb;
};

struct Plane {
    Vector3 normal;
    float distance;

    Plane()
    {
        normal = { 0.0f, 1.0f, 0.0f };
        distance = 0.0f;
    }

    Plane(const Vector3& p1, const Vector3& norm)
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
        if (Data)
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
