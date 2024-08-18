#include <tbpch.h>
#include <Drivers/gles3/OpenGLES3UniformBuffer.h>
#include <Drivers/gles3/GLES3.h>

#include <gles3.h>

namespace Tabby {

OpenGLES3UniformBuffer::OpenGLES3UniformBuffer(uint32_t size, uint32_t binding)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3UniformBuffer::Constructor");

    GLES::gl->GenBuffers(1, &m_RendererID);
    GLES::gl->BindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
    GLES::gl->BufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
    GLES::gl->BindBufferBase(GL_UNIFORM_BUFFER, binding, m_RendererID);
}

OpenGLES3UniformBuffer::~OpenGLES3UniformBuffer()
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3UniformBuffer::Destructor");

    GLES::gl->DeleteBuffers(1, &m_RendererID);
}

void OpenGLES3UniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3UniformBuffer::SetData");

    GLES::gl->BindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
    GLES::gl->BufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
}

}
