#include "Drivers/OpenGL33/OpenGL33Framebuffer.h"
#include "tbpch.h"

#include <glad/gl33.h>

namespace Tabby {

static const uint32_t s_MaxFramebufferSize = 8192;

OpenGL33Framebuffer::OpenGL33Framebuffer(const FramebufferSpecification& spec)
    : m_Specification(spec)
{
    Invalidate();
}

OpenGL33Framebuffer::~OpenGL33Framebuffer()
{
    glDeleteFramebuffers(1, &m_RendererID);
    glDeleteTextures(1, &m_ColorAttachment);
    glDeleteTextures(1, &m_DepthAttachment);
}

void OpenGL33Framebuffer::Invalidate()
{
    if (m_RendererID) {
        glDeleteFramebuffers(1, &m_RendererID);
        glDeleteTextures(1, &m_ColorAttachment);
        glDeleteTextures(1, &m_DepthAttachment);
    }

    glGenFramebuffers(1, &m_RendererID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

    // glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachment);

    glGenTextures(1, &m_RendererID);
    glBindTexture(GL_TEXTURE_2D, m_RendererID);

    glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Specification.Width, m_Specification.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);

    // glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment);

    glGenTextures(1, &m_DepthAttachment);
    glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);

    glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);

    // glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);

    TB_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGL33Framebuffer::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
    glViewport(0, 0, m_Specification.Width, m_Specification.Height);
}

void OpenGL33Framebuffer::Unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGL33Framebuffer::Resize(uint32_t width, uint32_t height)
{
    if (width == 0 || height == 0 || width > s_MaxFramebufferSize || height > s_MaxFramebufferSize) {
        TB_CORE_WARN("Attempted to rezize framebuffer to {0}, {1}", width, height);
        return;
    }
    m_Specification.Width = width;
    m_Specification.Height = height;

    Invalidate();
}

}
