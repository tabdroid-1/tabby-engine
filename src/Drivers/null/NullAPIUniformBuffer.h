#pragma once

#include <Tabby/Renderer/UniformBuffer.h>

namespace Tabby {

class NullAPIUniformBuffer : public UniformBuffer {
public:
    NullAPIUniformBuffer(uint32_t size, uint32_t binding);
    virtual ~NullAPIUniformBuffer();

    virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;
};
}
