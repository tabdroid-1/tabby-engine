#include <tbpch.h>
#include <Drivers/gl33/OpenGL33Framebuffer.h>
#include <Drivers/GPUProfiler.h>

#include <gl.h>

namespace Tabby {

static const uint32_t s_MaxFramebufferSize = 16384;

namespace Utils {

    static GLenum TextureTarget(bool multisampled)
    {
        return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
    }

    static void CreateTextures(bool multisampled, uint32_t* outID, uint32_t count)
    {
        TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33Framebuffer::Utils::CreateTextures");
        TB_PROFILE_GPU("Tabby::OpenGL33Framebuffer::Utils::CreateTextures");

        glGenTextures(count, outID);
        glBindTexture(TextureTarget(multisampled), *outID);
    }

    static void BindTexture(bool multisampled, uint32_t id)
    {
        TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33Framebuffer::Utils::BindTexture");
        TB_PROFILE_GPU("Tabby::OpenGL33Framebuffer::Utils::BindTexture");

        glBindTexture(TextureTarget(multisampled), id);
    }

    static void AttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index)
    {
        TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33Framebuffer::Utils::AttachColorTexture");
        TB_PROFILE_GPU("Tabby::OpenGL33Framebuffer::Utils::AttachColorTexture");

        bool multisampled = samples > 1;
        if (multisampled) {
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
        } else {
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
    }

    static void AttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
    {
        TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33Framebuffer::Utils::AttachDepthTexture");
        TB_PROFILE_GPU("Tabby::OpenGL33Framebuffer::Utils::AttachDepthTexture");

        bool multisampled = samples > 1;
        if (multisampled) {
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
        } else {
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

        glFramebufferTexture(GL_FRAMEBUFFER, attachmentType, id, 0);
    }

    static bool IsDepthFormat(FramebufferTextureFormat format)
    {
        switch (format) {
        case FramebufferTextureFormat::DEPTH24STENCIL8:
            return true;
        default:
            return false;
        }
    }

    static GLenum TabbyFBTextureFormatToGL(FramebufferTextureFormat format)
    {
        switch (format) {
        case FramebufferTextureFormat::RGBA8:
            return GL_RGBA8;
        case FramebufferTextureFormat::RED_INTEGER:
            return GL_RED_INTEGER;
        default:
            TB_CORE_ASSERT(false);
            return 0;
        }
    }

}

OpenGL33Framebuffer::OpenGL33Framebuffer(const FramebufferSpecification& spec)
    : m_Specification(spec)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33Framebuffer::Constructor");
    TB_PROFILE_GPU("Tabby::OpenGL33Framebuffer::Constructor");

    for (auto spec : m_Specification.Attachments.Attachments) {
        if (!Utils::IsDepthFormat(spec.TextureFormat))
            m_ColorAttachmentSpecifications.emplace_back(spec);
        else
            m_DepthAttachmentSpecification = spec;
    }

    Invalidate();
}

OpenGL33Framebuffer::~OpenGL33Framebuffer()
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33Framebuffer::Destructor");
    TB_PROFILE_GPU("Tabby::OpenGL33Framebuffer::Destructor");

    glDeleteFramebuffers(1, &m_RendererID);
    glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
    glDeleteTextures(1, &m_DepthAttachment);
}

void OpenGL33Framebuffer::Invalidate()
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33Framebuffer::Invalidate");
    TB_PROFILE_GPU("Tabby::OpenGL33Framebuffer::Invalidate");

    if (m_RendererID) {
        glDeleteFramebuffers(1, &m_RendererID);
        glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
        glDeleteTextures(1, &m_DepthAttachment);

        m_ColorAttachments.clear();
        m_DepthAttachment = 0;
    }

    glGenFramebuffers(1, &m_RendererID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

    bool multisample = m_Specification.Samples > 1;

    // Attachments
    if (m_ColorAttachmentSpecifications.size()) {
        m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size());
        Utils::CreateTextures(multisample, m_ColorAttachments.data(), m_ColorAttachments.size());

        for (size_t i = 0; i < m_ColorAttachments.size(); i++) {
            Utils::BindTexture(multisample, m_ColorAttachments[i]);

            switch (m_ColorAttachmentSpecifications[i].TextureFormat) {
            case FramebufferTextureFormat::RGBA8:
                Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, GL_RGBA8, GL_RGBA, m_Specification.Width, m_Specification.Height, i);
                break;
            case FramebufferTextureFormat::RED_INTEGER:
                Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, GL_R32I, GL_RED_INTEGER, m_Specification.Width, m_Specification.Height, i);
                break;
            default:
                break;
            }
        }
    }

    if (m_DepthAttachmentSpecification.TextureFormat != FramebufferTextureFormat::None) {
        Utils::CreateTextures(multisample, &m_DepthAttachment, 1);
        Utils::BindTexture(multisample, m_DepthAttachment);
        switch (m_DepthAttachmentSpecification.TextureFormat) {
        case FramebufferTextureFormat::DEPTH24STENCIL8:
            Utils::AttachDepthTexture(m_DepthAttachment, m_Specification.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_Specification.Width, m_Specification.Height);
            break;
        default:
            break;
        }
    }

    if (m_ColorAttachments.size() > 0) {
        TB_CORE_ASSERT(m_ColorAttachments.size() <= 4);
        GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
        glDrawBuffers(m_ColorAttachments.size(), buffers);
    } else if (m_ColorAttachments.empty()) {
        // Only depth-pass
        glDrawBuffers(0, GL_NONE);
    }

    TB_CORE_ASSERT_TAGGED(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGL33Framebuffer::Bind()
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33Framebuffer::Bind");
    TB_PROFILE_GPU("Tabby::OpenGL33Framebuffer::Bind");

    glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
    glViewport(0, 0, m_Specification.Width, m_Specification.Height);
}

void OpenGL33Framebuffer::Unbind()
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33Framebuffer::Unbind");
    TB_PROFILE_GPU("Tabby::OpenGL33Framebuffer::Unbind");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGL33Framebuffer::Resize(uint32_t width, uint32_t height)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33Framebuffer::Resize");
    TB_PROFILE_GPU("Tabby::OpenGL33Framebuffer::Resize");

    if (width == 0 || height == 0 || width > s_MaxFramebufferSize || height > s_MaxFramebufferSize) {
        TB_CORE_WARN("Attempted to rezize framebuffer to {0}, {1}", width, height);
        return;
    }
    m_Specification.Width = width;
    m_Specification.Height = height;

    Invalidate();
}

int OpenGL33Framebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33Framebuffer::ReadPixel");
    TB_PROFILE_GPU("Tabby::OpenGL33Framebuffer::ReadPixel");

    TB_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size());

    glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
    int pixelData;
    glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
    return pixelData;
}

void OpenGL33Framebuffer::ClearAttachment(uint32_t attachmentIndex, int value)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33Framebuffer::ClearAttachment");
    TB_PROFILE_GPU("Tabby::OpenGL33Framebuffer::ClearAttachment");

    TB_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size());

    auto& spec = m_ColorAttachmentSpecifications[attachmentIndex];
    glBindTexture(GL_TEXTURE_2D, m_ColorAttachments[attachmentIndex]);
    glTexImage2D(GL_TEXTURE_2D, 0, Utils::TabbyFBTextureFormatToGL(spec.TextureFormat), m_Specification.Width, m_Specification.Height, 0, GL_RGBA, GL_INT, &value);
    glBindTexture(GL_TEXTURE_2D, 0);
}
}
