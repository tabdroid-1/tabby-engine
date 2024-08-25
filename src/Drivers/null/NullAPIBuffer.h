#pragma once

#include <Tabby/Renderer/Buffer.h>

namespace Tabby {

class NullAPIVertexBuffer : public VertexBuffer {
public:
    NullAPIVertexBuffer(uint32_t size);
    NullAPIVertexBuffer(float* vertices, uint32_t size);
    virtual ~NullAPIVertexBuffer();

    virtual void Bind() const override;
    virtual void Unbind() const override;

    virtual void SetData(const void* data, uint32_t size) override;
    virtual void SetSubData(const void* data, uint32_t size, uint32_t offset) override;

    virtual const BufferLayout& GetLayout() const override { return m_Layout; }
    virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }

private:
    uint32_t m_RendererID;
    BufferLayout m_Layout;
};

class NullAPIIndexBuffer : public IndexBuffer {
public:
    NullAPIIndexBuffer(uint32_t* indices, uint32_t count);
    virtual ~NullAPIIndexBuffer();

    virtual void Bind() const override;
    virtual void Unbind() const override;

    virtual uint32_t GetCount() const override { return 0; }
};

}
