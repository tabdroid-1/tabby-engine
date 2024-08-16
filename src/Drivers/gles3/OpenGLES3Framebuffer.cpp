#include "Drivers/gles3/OpenGLES3Framebuffer.h"
#include "Drivers/gles3/GLES3.h"
#include "tbpch.h"

#include <gles3.h>

namespace Tabby {

static const uint32_t s_MaxFramebufferSize = 16384;

namespace Utils {

    static void CreateTextures(uint32_t* outID, uint32_t count)
    {
        TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Framebuffer::Utils::CreateTextures");

        GLES3::GL()->GenTextures(count, outID);
        GLES3::GL()->BindTexture(GL_TEXTURE_2D, *outID);
    }

    static void BindTexture(uint32_t id)
    {
        TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Framebuffer::Utils::BindTexture");

        GLES3::GL()->BindTexture(GL_TEXTURE_2D, id);
    }

    static void AttachColorTexture(uint32_t id, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index)
    {
        TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Framebuffer::Utils::AttachColorTexture");

        GLES3::GL()->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        GLES3::GL()->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        GLES3::GL()->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        GLES3::GL()->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        GLES3::GL()->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        GLES3::GL()->TexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

        GLES3::GL()->FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, id, 0);
    }

    static void AttachDepthTexture(uint32_t id, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
    {
        TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Framebuffer::Utils::AttachDepthTexture");

        GLES3::GL()->TexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT_24_8, 0);

        GLES3::GL()->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        GLES3::GL()->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        GLES3::GL()->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        GLES3::GL()->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        GLES3::GL()->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        uint32_t rboID;
        GLES3::GL()->GenRenderbuffers(1, &rboID);
        GLES3::GL()->BindRenderbuffer(GL_RENDERBUFFER, rboID);
        GLES3::GL()->RenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        GLES3::GL()->FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboID);

        GLES3::GL()->FramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, id, 0);

        GLES3::GL()->DeleteRenderbuffers(1, &rboID);
    }

    static bool IsDepthFormat(FramebufferTextureFormat format)
    {
        switch (format) {
        case FramebufferTextureFormat::DEPTH24STENCIL8:
            return true;
        }

        return false;
    }

    static GLenum TabbyFBTextureFormatToGL(FramebufferTextureFormat format)
    {
        switch (format) {
        case FramebufferTextureFormat::RGBA8:
            return GL_RGBA8;
        case FramebufferTextureFormat::RED_INTEGER:
            return GL_RED_INTEGER;
        }

        TB_CORE_ASSERT(false);
        return 0;
    }

}

OpenGLES3Framebuffer::OpenGLES3Framebuffer(const FramebufferSpecification& spec)
    : m_Specification(spec)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Framebuffer::Constructor");

    for (auto spec : m_Specification.Attachments.Attachments) {
        if (!Utils::IsDepthFormat(spec.TextureFormat))
            m_ColorAttachmentSpecifications.emplace_back(spec);
        else
            m_DepthAttachmentSpecification = spec;
    }

    Invalidate();
}

OpenGLES3Framebuffer::~OpenGLES3Framebuffer()
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Framebuffer::Destructor");

    GLES3::GL()->DeleteFramebuffers(1, &m_RendererID);
    GLES3::GL()->DeleteTextures(1, &m_ColorAttachment);
    GLES3::GL()->DeleteTextures(1, &m_DepthAttachment);
}

void OpenGLES3Framebuffer::Invalidate()
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Framebuffer::Invalidate");

    if (m_RendererID) {
        GLES3::GL()->DeleteFramebuffers(1, &m_RendererID);
        GLES3::GL()->DeleteTextures(1, &m_ColorAttachment);
        GLES3::GL()->DeleteTextures(1, &m_DepthAttachment);

        m_ColorAttachment = 0;
        m_DepthAttachment = 0;
    }

    GLES3::GL()->GenFramebuffers(1, &m_RendererID);
    GLES3::GL()->BindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

    {

        GLES3::GL()->GenTextures(1, &m_ColorAttachment);
        GLES3::GL()->BindTexture(GL_TEXTURE_2D, m_ColorAttachment);
        // GLES3::GL()->TexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA, m_Specification.Width, m_Specification.Height);
        GLES3::GL()->TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Specification.Width, m_Specification.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        GLES3::GL()->FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);

        GLES3::GL()->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        GLES3::GL()->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        // GLES3::GL()->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        GLES3::GL()->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        GLES3::GL()->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        GLES3::GL()->BindTexture(GL_TEXTURE_2D, 0);
    }

    {
        GLES3::GL()->GenTextures(1, &m_DepthAttachment);
        GLES3::GL()->BindTexture(GL_TEXTURE_2D, m_DepthAttachment);
        GLES3::GL()->TexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_Specification.Width, m_Specification.Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        GLES3::GL()->FramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);

        GLES3::GL()->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        GLES3::GL()->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        GLES3::GL()->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        GLES3::GL()->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        GLES3::GL()->BindTexture(GL_TEXTURE_2D, 0);
    }

    GLenum framebufferStatus = GLES3::GL()->CheckFramebufferStatus(GL_FRAMEBUFFER);
    if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
        TB_CORE_ERROR("Framebuffer is incomplete! Status: {0}", framebufferStatus);
    }

    GLES3::GL()->BindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLES3Framebuffer::Bind()
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Framebuffer::Bind");

    GLES3::GL()->BindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
    GLES3::GL()->Viewport(0, 0, m_Specification.Width, m_Specification.Height);
}

void OpenGLES3Framebuffer::Unbind()
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Framebuffer::Unbind");

    GLES3::GL()->BindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLES3Framebuffer::Resize(uint32_t width, uint32_t height)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Framebuffer::Resize");

    if (width == 0 || height == 0 || width > s_MaxFramebufferSize || height > s_MaxFramebufferSize) {
        TB_CORE_WARN("Attempted to rezize framebuffer to {0}, {1}", width, height);
        return;
    }
    m_Specification.Width = width;
    m_Specification.Height = height;

    Invalidate();
}

int OpenGLES3Framebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Framebuffer::ReadPixel");

    // TB_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size());

    GLES3::GL()->ReadBuffer(GL_COLOR_ATTACHMENT0);
    int pixelData;
    GLES3::GL()->ReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
    return pixelData;
}

void OpenGLES3Framebuffer::ClearAttachment(uint32_t attachmentIndex, int value)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Framebuffer::ClearAttachment");

    // TB_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size());

    auto& spec = m_ColorAttachmentSpecifications[0];
    GLES3::GL()->BindTexture(GL_TEXTURE_2D, m_ColorAttachment);
    GLES3::GL()->TexImage2D(GL_TEXTURE_2D, 0, Utils::TabbyFBTextureFormatToGL(spec.TextureFormat), m_Specification.Width, m_Specification.Height, 0, GL_RGBA, GL_INT, &value);
    GLES3::GL()->BindTexture(GL_TEXTURE_2D, 0);
}

}
