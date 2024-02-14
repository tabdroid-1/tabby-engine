#include "OpenGL33UniformBuffer.h"
#include "GL33.h"
#include "tbpch.h"

#if !defined(TB_PLATFORM_WEB) || !defined(TB_PLATFORM_ANDROID)
#include <glad/gl33.h>

namespace Tabby {

OpenGL33UniformBuffer::OpenGL33UniformBuffer(uint32_t size, uint32_t binding)
{
    TB_PROFILE_SCOPE();
    GL33::GL()->GenBuffers(1, &m_RendererID);
    GL33::GL()->BindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
    GL33::GL()->BufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
    GL33::GL()->BindBufferBase(GL_UNIFORM_BUFFER, binding, m_RendererID);
}

OpenGL33UniformBuffer::~OpenGL33UniformBuffer()
{
    TB_PROFILE_SCOPE();
    GL33::GL()->DeleteBuffers(1, &m_RendererID);
}

void OpenGL33UniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
{
    TB_PROFILE_SCOPE();
    GL33::GL()->BindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
    GL33::GL()->BufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
}

}
#endif
