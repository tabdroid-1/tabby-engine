#pragma once

#include <Tabby/Foundation/Types.h>

namespace Tabby {

/*
 *  @brief Represents a buffer used for rendering, e.g. vertex, index or storage buffers.
 */

enum class ShaderBufferFlags : BitMask {
    VERTEX_RATE = BIT(0),
    INSTANCE_RATE = BIT(1),
    INDEX_TYPE_UINT8 = BIT(2),
    INDEX_TYPE_UINT16 = BIT(3),
    INDEX_TYPE_UINT32 = BIT(4),
    CREATE_STAGING_BUFFER = BIT(5),
};

enum class ShaderBufferUsage : uint32_t {
    VERTEX_BUFFER,
    INDEX_BUFFER,
    UNIFORM_BUFFER,
    STORAGE_BUFFER,
    STAGING_BUFFER,
    SHADER_DEVICE_ADDRESS,
    INDIRECT_PARAMS
};

enum class ShaderBufferMemoryUsage {
    READ_BACK,
    COHERENT_WRITE,
    NO_HOST_ACCESS
};

enum class ShaderBufferMemoryHeap {
    DEVICE, // buffer is allocated in VRAM
    HOST // buffer is allocated in system RAM
};

struct ShaderBufferSpecification {
    uint64_t size;
    BitMask flags;
    ShaderBufferUsage buffer_usage;
    ShaderBufferMemoryUsage memory_usage;
    ShaderBufferMemoryHeap heap;
    std::string debug_name;
};

class ShaderBuffer {
public:
    static Shared<ShaderBuffer> Create(const ShaderBufferSpecification& spec, Buffer data = Buffer());
    static Shared<ShaderBuffer> Create(const ShaderBufferSpecification& spec, void* data, uint64_t data_size);

    virtual ~ShaderBuffer() {};

    virtual void Destroy() = 0;

    virtual ShaderBufferSpecification GetSpecification() const = 0;

    virtual void UploadData(uint64_t offset, void* data, uint64_t data_size) = 0;
    virtual void UploadData(uint64_t offset, Buffer data) = 0;
};

}
