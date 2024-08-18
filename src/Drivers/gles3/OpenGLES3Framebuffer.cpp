#include <tbpch.h>
#include <Drivers/gles3/OpenGLES3Framebuffer.h>
#include <Drivers/gles3/GLES3.h>

#include <gles3.h>

namespace Tabby {

static const uint32_t s_MaxFramebufferSize = 16384;

namespace Utils {

    static void CreateTextures(uint32_t* outID, uint32_t count)
    {
        TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Framebuffer::Utils::CreateTextures");

        GLES::gl->GenTextures(count, outID);
        GLES::gl->BindTexture(GL_TEXTURE_2D, *outID);
    }

    static void BindTexture(uint32_t id)
    {
        TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Framebuffer::Utils::BindTexture");

        GLES::gl->BindTexture(GL_TEXTURE_2D, id);
    }

    static void AttachColorTexture(uint32_t id, GLenum internalFormat, GLenum format, GLenum type, uint32_t width, uint32_t height, int index)
    {
        TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Framebuffer::Utils::AttachColorTexture");
        GLES::gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        GLES::gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        GLES::gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        GLES::gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        GLES::gl->TexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, nullptr);

        GLES::gl->FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, id, 0);
    }

    static void AttachDepthTexture(uint32_t id, GLenum internalFormat, GLenum format, GLenum imageType, GLenum attachmentType, uint32_t width, uint32_t height)
    {
        TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Framebuffer::Utils::AttachDepthTexture");

        GLES::gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        GLES::gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        GLES::gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        GLES::gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        GLES::gl->TexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, imageType, nullptr);
        GLES::gl->FramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, GL_TEXTURE_2D, id, 0);
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

    GLES::gl->DeleteFramebuffers(1, &m_RendererID);
    GLES::gl->DeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
    GLES::gl->DeleteTextures(1, &m_DepthAttachment);
}

void OpenGLES3Framebuffer::Invalidate()
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Framebuffer::Invalidate");

    if (m_RendererID) {
        GLES::gl->DeleteFramebuffers(1, &m_RendererID);
        GLES::gl->DeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
        GLES::gl->DeleteTextures(1, &m_DepthAttachment);

        m_ColorAttachments.clear();
        m_DepthAttachment = 0;
    }

    GLES::gl->GenFramebuffers(1, &m_RendererID);
    GLES::gl->BindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

    bool multisample = m_Specification.Samples > 1;

    // Attachments
    // Helpfull: https://registry.khronos.org/OpenGL-Refpages/es3.0/html/glTexImage2D.xhtml
    if (m_ColorAttachmentSpecifications.size()) {
        m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size());
        Utils::CreateTextures(m_ColorAttachments.data(), m_ColorAttachments.size());

        for (size_t i = 0; i < m_ColorAttachments.size(); i++) {
            Utils::BindTexture(m_ColorAttachments[i]);

            switch (m_ColorAttachmentSpecifications[i].TextureFormat) {
            case FramebufferTextureFormat::RGBA8:
                Utils::AttachColorTexture(m_ColorAttachments[i], GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, m_Specification.Width, m_Specification.Height, i);
                break;
            case FramebufferTextureFormat::RED_INTEGER:
                Utils::AttachColorTexture(m_ColorAttachments[i], GL_R32I, GL_RED_INTEGER, GL_INT, m_Specification.Width, m_Specification.Height, i);
                break;
            }
        }
    }

    if (m_DepthAttachmentSpecification.TextureFormat != FramebufferTextureFormat::None) {
        Utils::CreateTextures(&m_DepthAttachment, 1);
        Utils::BindTexture(m_DepthAttachment);
        switch (m_DepthAttachmentSpecification.TextureFormat) {
        case FramebufferTextureFormat::DEPTH24STENCIL8:
            Utils::AttachDepthTexture(m_DepthAttachment, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, GL_DEPTH_STENCIL_ATTACHMENT, m_Specification.Width, m_Specification.Height);
            break;
        }
    }

    if (m_ColorAttachments.size() > 0) {
        TB_CORE_ASSERT(m_ColorAttachments.size() <= 4);
        GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
        GLES::gl->DrawBuffers(m_ColorAttachments.size(), buffers);
    } else {
        // Only depth-pass
        GLES::gl->DrawBuffers(0, GL_NONE);
    }

    std::string message = "Framebuffer is incomplete! Error code: ";

    GLenum status = GLES::gl->CheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        message += std::to_string(GLES::gl->CheckFramebufferStatus(GL_FRAMEBUFFER));
        message += "(";
        switch (status) {
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            message += "Incomplete attachment";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            message += "Missing attachment";
            break;
        default:
            message += "Unknown error";
            break;
        }
        message += ")";
    }

    TB_CORE_ASSERT_TAGGED(GLES::gl->CheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, message);
    GLES::gl->BindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLES3Framebuffer::Bind()
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Framebuffer::Bind");

    GLES::gl->BindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
    GLES::gl->Viewport(0, 0, m_Specification.Width, m_Specification.Height);
}

void OpenGLES3Framebuffer::Unbind()
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Framebuffer::Unbind");

    GLES::gl->BindFramebuffer(GL_FRAMEBUFFER, 0);
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

    TB_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size());

    GLES::gl->ReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
    int pixelData;
    GLES::gl->ReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
    return pixelData;
}

void OpenGLES3Framebuffer::ClearAttachment(uint32_t attachmentIndex, int value)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Framebuffer::ClearAttachment");

    TB_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size());

    auto& spec = m_ColorAttachmentSpecifications[attachmentIndex];
    GLES::gl->BindTexture(GL_TEXTURE_2D, m_ColorAttachments[attachmentIndex]);
    GLES::gl->TexImage2D(GL_TEXTURE_2D, 0, Utils::TabbyFBTextureFormatToGL(spec.TextureFormat), m_Specification.Width, m_Specification.Height, 0, GL_RGBA, GL_INT, &value);
    GLES::gl->BindTexture(GL_TEXTURE_2D, 0);
}
}
