#include <Drivers/gl46/OpenGL46Buffer.h>
#include <Drivers/GPUProfiler.h>

#include "tbpch.h"

#include <gl.h>

namespace Tabby {

/////////////////////////////////////////////////////////////////////////////
// VertexBuffer /////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

OpenGL46VertexBuffer::OpenGL46VertexBuffer(uint32_t size)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL46VertexBuffer::Constructor");
    TB_PROFILE_GPU("Tabby::OpenGL46VertexBuffer::Constructor");

    glCreateBuffers(1, &m_RendererID);
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}

OpenGL46VertexBuffer::OpenGL46VertexBuffer(float* vertices, uint32_t size)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL46VertexBuffer::Constructor");
    TB_PROFILE_GPU("Tabby::OpenGL46VertexBuffer::Constructor");

    glCreateBuffers(1, &m_RendererID);
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

OpenGL46VertexBuffer::~OpenGL46VertexBuffer()
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL46VertexBuffer::Destructor");
    TB_PROFILE_GPU("Tabby::OpenGL46VertexBuffer::Destructor");

    glDeleteBuffers(1, &m_RendererID);
}

void OpenGL46VertexBuffer::Bind() const
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL46VertexBuffer::Bind");
    TB_PROFILE_GPU("Tabby::OpenGL46VertexBuffer::Bind");

    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}

void OpenGL46VertexBuffer::Unbind() const
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL46VertexBuffer::Unbind");
    TB_PROFILE_GPU("Tabby::OpenGL46VertexBuffer::Unbind");

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGL46VertexBuffer::SetData(const void* data, uint32_t size)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL46VertexBuffer::SetData");
    TB_PROFILE_GPU("Tabby::OpenGL46VertexBuffer::SetData");

    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

void OpenGL46VertexBuffer::SetSubData(const void* data, uint32_t size, uint32_t offset)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL46VertexBuffer::SetSubData");
    TB_PROFILE_GPU("Tabby::OpenGL46VertexBuffer::SetSubData");

    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

/////////////////////////////////////////////////////////////////////////////
// IndexBuffer //////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

OpenGL46IndexBuffer::OpenGL46IndexBuffer(uint32_t* indices, uint32_t count)
    : m_Count(count)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL46IndexBuffer::Constructor");
    TB_PROFILE_GPU("Tabby::OpenGL46IndexBuffer::Constructor");

    glCreateBuffers(1, &m_RendererID);

    // GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
    // Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state.
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
}

OpenGL46IndexBuffer::~OpenGL46IndexBuffer()
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL46IndexBuffer::Destructor");
    TB_PROFILE_GPU("Tabby::OpenGL46IndexBuffer::Destructor");

    glDeleteBuffers(1, &m_RendererID);
}

void OpenGL46IndexBuffer::Bind() const
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL46IndexBuffer::Bind");
    TB_PROFILE_GPU("Tabby::OpenGL46IndexBuffer::Bind");

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
}

void OpenGL46IndexBuffer::Unbind() const
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL46IndexBuffer::Unbind");
    TB_PROFILE_GPU("Tabby::OpenGL46IndexBuffer::Unbind");

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

}
