#include "OpenGL33UniformBuffer.h"
#include "tbpch.h"

#include <glad/gl33.h>

namespace Tabby {

OpenGL33UniformBuffer::OpenGL33UniformBuffer(uint32_t size, uint32_t binding)
{
    glGenBuffers(1, &m_RendererID);
    glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
    glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_RendererID);
    // glGenBuffers(1, &m_RendererID);
    // glNamedBufferData(m_RendererID, size, nullptr, GL_DYNAMIC_DRAW); // TODO: investigate usage hint
    // glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_RendererID);
}

OpenGL33UniformBuffer::~OpenGL33UniformBuffer()
{
    glDeleteBuffers(1, &m_RendererID);
}

void OpenGL33UniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
{
    // glBufferSubData(m_RendererID, offset, size, data);

    glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);

    // Update the buffer data
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
    // glNamedBufferSubData(m_RendererID, offset, size, data);
}

}
