#include "Drivers/gl33/OpenGL33Buffer.h"
#include "Drivers/gl33/GL33.h"
#include "tbpch.h"

#if !defined(TB_PLATFORM_WEB) || !defined(TB_PLATFORM_ANDROID)
#include <glad/gl33.h>

namespace Tabby {

/////////////////////////////////////////////////////////////////////////////
// VertexBuffer /////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

OpenGL33VertexBuffer::OpenGL33VertexBuffer(uint32_t size)
{
    TB_PROFILE_SCOPE_NAME("(VertexBuffer) Generate");

    GL33::GL()->GenBuffers(1, &m_RendererID);
    GL33::GL()->BindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    GL33::GL()->BufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}

OpenGL33VertexBuffer::OpenGL33VertexBuffer(float* vertices, uint32_t size)
{
    TB_PROFILE_SCOPE_NAME("(VertexBuffer) Generate");

    GL33::GL()->GenBuffers(1, &m_RendererID);
    GL33::GL()->BindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    GL33::GL()->BufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

OpenGL33VertexBuffer::~OpenGL33VertexBuffer()
{
    TB_PROFILE_SCOPE_NAME("(VertexBuffer) Delete");

    GL33::GL()->DeleteBuffers(1, &m_RendererID);
}

void OpenGL33VertexBuffer::Bind() const
{
    TB_PROFILE_SCOPE_NAME("(VertexBuffer) Bind");

    GL33::GL()->BindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}

void OpenGL33VertexBuffer::Unbind() const
{
    TB_PROFILE_SCOPE_NAME("(VertexBuffer) Unbind");

    GL33::GL()->BindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGL33VertexBuffer::SetData(const void* data, uint32_t size)
{
    TB_PROFILE_SCOPE_NAME("(VertexBuffer) Set Data");

    GL33::GL()->BindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    GL33::GL()->BufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

/////////////////////////////////////////////////////////////////////////////
// IndexBuffer //////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

OpenGL33IndexBuffer::OpenGL33IndexBuffer(uint32_t* indices, uint32_t count)
    : m_Count(count)
{
    TB_PROFILE_SCOPE_NAME("(Index Buffer) Generate");

    GL33::GL()->GenBuffers(1, &m_RendererID);

    GL33::GL()->BindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    GL33::GL()->BufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
}

OpenGL33IndexBuffer::~OpenGL33IndexBuffer()
{
    TB_PROFILE_SCOPE_NAME("(Index Buffer) Delete");

    GL33::GL()->DeleteBuffers(1, &m_RendererID);
}

void OpenGL33IndexBuffer::Bind() const
{
    TB_PROFILE_SCOPE_NAME("(Index Buffer) Bind");

    GL33::GL()->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
}

void OpenGL33IndexBuffer::Unbind() const
{
    TB_PROFILE_SCOPE_NAME("(Index Buffer) Unbind");

    GL33::GL()->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

}
#endif
