#include "OpenGLES3UniformBuffer.h"
#include "GLES3.h"
#include "tbpch.h"

#include <gles3.h>

namespace Tabby {

OpenGLES3UniformBuffer::OpenGLES3UniformBuffer(uint32_t size, uint32_t binding)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3UniformBuffer::Constructor");

    GLES3::GL()->GenBuffers(1, &m_RendererID);
    GLES3::GL()->BindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
    GLES3::GL()->BufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
    GLES3::GL()->BindBufferBase(GL_UNIFORM_BUFFER, binding, m_RendererID);
}

OpenGLES3UniformBuffer::~OpenGLES3UniformBuffer()
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3UniformBuffer::Destructor");

    GLES3::GL()->DeleteBuffers(1, &m_RendererID);
}

void OpenGLES3UniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3UniformBuffer::SetData");

    GLES3::GL()->BindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
    GLES3::GL()->BufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
}

}
