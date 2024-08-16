#include "Drivers/gles3/OpenGLES3Buffer.h"
#include "Drivers/gles3/GLES3.h"
#include "tbpch.h"

#include <gles3.h>

namespace Tabby {

/////////////////////////////////////////////////////////////////////////////
// VertexBuffer /////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

OpenGLES3VertexBuffer::OpenGLES3VertexBuffer(uint32_t size)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3VertexBuffer::Constructor");

    GLES3::GL()->GenBuffers(1, &m_RendererID);
    GLES3::GL()->BindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    GLES3::GL()->BufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}

OpenGLES3VertexBuffer::OpenGLES3VertexBuffer(float* vertices, uint32_t size)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3VertexBuffer::Constructor");

    GLES3::GL()->GenBuffers(1, &m_RendererID);
    GLES3::GL()->BindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    GLES3::GL()->BufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

OpenGLES3VertexBuffer::~OpenGLES3VertexBuffer()
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3VertexBuffer::Destructor");

    GLES3::GL()->DeleteBuffers(1, &m_RendererID);
}

void OpenGLES3VertexBuffer::Bind() const
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3VertexBuffer::Bind");

    GLES3::GL()->BindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}

void OpenGLES3VertexBuffer::Unbind() const
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3VertexBuffer::Unbind");

    GLES3::GL()->BindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGLES3VertexBuffer::SetData(const void* data, uint32_t size)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3VertexBuffer::SetData");

    GLES3::GL()->BindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    GLES3::GL()->BufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

void OpenGLES3VertexBuffer::SetSubData(const void* data, uint32_t size, uint32_t offset)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3VertexBuffer::SetSubData");

    GLES3::GL()->BindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    GLES3::GL()->BufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

/////////////////////////////////////////////////////////////////////////////
// IndexBuffer //////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

OpenGLES3IndexBuffer::OpenGLES3IndexBuffer(uint32_t* indices, uint32_t count)
    : m_Count(count)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3IndexBuffer::Constructor");

    GLES3::GL()->GenBuffers(1, &m_RendererID);

    // GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
    //  Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state.
    GLES3::GL()->BindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    GLES3::GL()->BufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
}

OpenGLES3IndexBuffer::~OpenGLES3IndexBuffer()
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3IndexBuffer::Destructor");

    GLES3::GL()->DeleteBuffers(1, &m_RendererID);
}

void OpenGLES3IndexBuffer::Bind() const
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3IndexBuffer::Bind");

    GLES3::GL()->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
}

void OpenGLES3IndexBuffer::Unbind() const
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3IndexBuffer::Unbind");

    GLES3::GL()->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

}
