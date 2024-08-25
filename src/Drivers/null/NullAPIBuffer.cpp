#include <tbpch.h>
#include <Drivers/null/NullAPIBuffer.h>

namespace Tabby {

/////////////////////////////////////////////////////////////////////////////
// VertexBuffer /////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

NullAPIVertexBuffer::NullAPIVertexBuffer(uint32_t size)
{
}

NullAPIVertexBuffer::NullAPIVertexBuffer(float* vertices, uint32_t size)
{
}

NullAPIVertexBuffer::~NullAPIVertexBuffer()
{
}

void NullAPIVertexBuffer::Bind() const
{
}

void NullAPIVertexBuffer::Unbind() const
{
}

void NullAPIVertexBuffer::SetData(const void* data, uint32_t size)
{
}

void NullAPIVertexBuffer::SetSubData(const void* data, uint32_t size, uint32_t offset)
{
}

/////////////////////////////////////////////////////////////////////////////
// IndexBuffer //////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

NullAPIIndexBuffer::NullAPIIndexBuffer(uint32_t* indices, uint32_t count)
{
}

NullAPIIndexBuffer::~NullAPIIndexBuffer()
{
}

void NullAPIIndexBuffer::Bind() const
{
}

void NullAPIIndexBuffer::Unbind() const
{
}

}
