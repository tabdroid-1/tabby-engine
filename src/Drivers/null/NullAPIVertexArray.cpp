#include <tbpch.h>
#include <Drivers/null/NullAPIVertexArray.h>

namespace Tabby {

NullAPIVertexArray::NullAPIVertexArray()
{
}

NullAPIVertexArray::~NullAPIVertexArray()
{
}

void NullAPIVertexArray::Bind() const
{
}

void NullAPIVertexArray::Unbind() const
{
}

void NullAPIVertexArray::AddVertexBuffer(const Shared<VertexBuffer>& vertexBuffer)
{
}

void NullAPIVertexArray::SetIndexBuffer(const Shared<IndexBuffer>& indexBuffer)
{
}

}
