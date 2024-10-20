#include <tbpch.h>
#include <Drivers/gl46/OpenGL46Texture.h>
#include <Drivers/GPUProfiler.h>

#include <gl.h>

namespace Tabby {

namespace Utils {

    static GLenum TabbyImageFormatToGLDataFormat(ImageFormat format)
    {
        switch (format) {
        case ImageFormat::RGB8:
            return GL_RGB;
        case ImageFormat::RGBA8:
            return GL_RGBA;
        case ImageFormat::RGBA32_UNORM:
            return GL_RGB32UI;
        default:
            TB_CORE_ASSERT(false);
            return 0;
        }
    }

    static GLenum TabbyImageFormatToGLInternalFormat(ImageFormat format)
    {
        switch (format) {
        case ImageFormat::RGB8:
            return GL_RGB8;
        case ImageFormat::RGBA8:
            return GL_RGBA8;
        case ImageFormat::RGBA32_UNORM:
            return GL_RGB32UI;
        default:
            TB_CORE_ASSERT(false);
            return 0;
        }
    }

}

OpenGL46Texture::OpenGL46Texture(const TextureSpecification& specification, AssetHandle handle, Buffer data)
    : Texture(0)
    , m_Specification(specification)
    , m_Width(m_Specification.Width)
    , m_Height(m_Specification.Height)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL46Texture::Constructor");
    TB_PROFILE_GPU("Tabby::OpenGL46Texture::Constructor");

    Handle = handle;
    m_InternalFormat = Utils::TabbyImageFormatToGLInternalFormat(m_Specification.Format);
    m_DataFormat = Utils::TabbyImageFormatToGLDataFormat(m_Specification.Format);

    glPixelStorei(GL_UNPACK_ALIGNMENT, m_Specification.UnpackAlignment);

    glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
    glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);
    glBindTexture(GL_TEXTURE_2D, m_RendererID);

    if (m_Specification.GenerateMips) {
        glGenerateTextureMipmap(m_RendererID);

        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
    } else {

        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    if (data)
        SetData(data);
}

OpenGL46Texture::~OpenGL46Texture()
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL46Texture::Destructor");
    TB_PROFILE_GPU("Tabby::OpenGL46Texture::Destructor");

    Destroy();
}

void OpenGL46Texture::SetData(Buffer data)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL46Texture::SetData");
    TB_PROFILE_GPU("Tabby::OpenGL46Texture::SetData");

    glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data.Data);

    if (m_Specification.GenerateMips)
        glGenerateTextureMipmap(m_RendererID);
}

void OpenGL46Texture::SetSubData(void* data, uint32_t width, uint32_t height)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL46Texture::SetSubData");
    TB_PROFILE_GPU("Tabby::OpenGL46Texture::SetSubData");

    glTextureSubImage2D(m_RendererID, 0, 0, 0, width, height, m_DataFormat, GL_UNSIGNED_BYTE, data);

    if (m_Specification.GenerateMips)
        glGenerateTextureMipmap(m_RendererID);
}

void OpenGL46Texture::Bind(uint32_t slot) const
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL46Texture::Bind");
    TB_PROFILE_GPU("Tabby::OpenGL46Texture::Bind");

    glBindTextureUnit(slot, m_RendererID);
}

void OpenGL46Texture::Destroy()
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL46Texture::Destroy");
    TB_PROFILE_GPU("Tabby::OpenGL46Texture::Destroy");

    glDeleteTextures(1, &m_RendererID);
}
}
