#include "OpenGL33UniformBuffer.h"
#include "GL33.h"
#include "tbpch.h"

#include <glad/gl33.h>

namespace Tabby {

OpenGL33UniformBuffer::OpenGL33UniformBuffer(uint32_t size, uint32_t binding)
{
    GL33::GL()->GenBuffers(1, &m_RendererID);
    GL33::GL()->BindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
    GL33::GL()->BufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
    GL33::GL()->BindBufferBase(GL_UNIFORM_BUFFER, binding, m_RendererID);
    // glGenBuffers(1, &m_RendererID);
    // glNamedBufferData(m_RendererID, size, nullptr, GL_DYNAMIC_DRAW); // TODO: investigate usage hint
    // glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_RendererID);
}

OpenGL33UniformBuffer::~OpenGL33UniformBuffer()
{
    GL33::GL()->DeleteBuffers(1, &m_RendererID);
}

void OpenGL33UniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
{
    // glBufferSubData(m_RendererID, offset, size, data);

    GL33::GL()->BindBuffer(GL_UNIFORM_BUFFER, m_RendererID);

    // Update the buffer data
    GL33::GL()->BufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
    // glNamedBufferSubData(m_RendererID, offset, size, data);
}

}
