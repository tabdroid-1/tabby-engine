#include "tbpch.h"

#include <Drivers/gl46/OpenGL46UniformBuffer.h>
#include <Drivers/GPUProfiler.h>

#include <gl.h>

namespace Tabby {

OpenGL46UniformBuffer::OpenGL46UniformBuffer(uint32_t size, uint32_t binding)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL46UniformBuffer::Constructor");
    TB_PROFILE_GPU("Tabby::OpenGL46UniformBuffer::Constructor");

    glCreateBuffers(1, &m_RendererID);
    glNamedBufferData(m_RendererID, size, nullptr, GL_DYNAMIC_DRAW); // TODO: investigate usage hint
    glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_RendererID);
}

OpenGL46UniformBuffer::~OpenGL46UniformBuffer()
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL46UniformBuffer::Destructor");
    TB_PROFILE_GPU("Tabby::OpenGL46UniformBuffer::Destructor");

    glDeleteBuffers(1, &m_RendererID);
}

void OpenGL46UniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL46UniformBuffer::SetData");
    TB_PROFILE_GPU("Tabby::OpenGL46UniformBuffer::SetData");

    glNamedBufferSubData(m_RendererID, offset, size, data);
}

}
