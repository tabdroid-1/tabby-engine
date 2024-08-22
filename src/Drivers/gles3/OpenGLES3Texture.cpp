#include <tbpch.h>
#include <Drivers/gles3/OpenGLES3Texture.h>
#include <Drivers/gles3/GLES3.h>

#include <stb_image.h>
#include <gles3.h>

namespace Tabby {

namespace Utils {

    static GLenum TabbyImageFormatToGLDataFormat(ImageFormat format)
    {
        switch (format) {
        case ImageFormat::RGB8:
            return GL_RGB;
        case ImageFormat::RGBA8:
            return GL_RGBA;
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
        default:
            TB_CORE_ASSERT(false);
            return 0;
        }
    }
}

OpenGLES3Texture::OpenGLES3Texture(const TextureSpecification& specification, AssetHandle handle, Buffer data)
    : Texture(0)
    , m_Specification(specification)
    , m_Width(m_Specification.Width)
    , m_Height(m_Specification.Height)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Texture::Constructor");

    Handle = handle;
    m_InternalFormat = Utils::TabbyImageFormatToGLInternalFormat(m_Specification.Format);
    m_DataFormat = Utils::TabbyImageFormatToGLDataFormat(m_Specification.Format);

    GLES::gl->PixelStorei(GL_UNPACK_ALIGNMENT, m_Specification.UnpackAlignment);

    GLES::gl->GenTextures(1, &m_RendererID);
    GLES::gl->BindTexture(GL_TEXTURE_2D, m_RendererID);

    GLES::gl->TexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    if (m_Specification.GenerateMips) {

        GLES::gl->GenerateMipmap(GL_TEXTURE_2D);

        GLES::gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        GLES::gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        GLES::gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        GLES::gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    } else {
        GLES::gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        GLES::gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        GLES::gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        GLES::gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    if (data)
        SetData(data);
}

OpenGLES3Texture::~OpenGLES3Texture()
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Texture::Destructor");

    Destroy();
}

void OpenGLES3Texture::SetData(Buffer data)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Texture::SetData");

    // uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
    // TB_CORE_ASSERT_TAGGED(data.Size == m_Width * m_Height * bpp, "Data must be the entire texture!");

    GLES::gl->BindTexture(GL_TEXTURE_2D, m_RendererID);
    GLES::gl->TexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data.Data);

    if (m_Specification.GenerateMips)
        GLES::gl->GenerateMipmap(GL_TEXTURE_2D);
}

void OpenGLES3Texture::SetSubData(void* data, uint32_t width, uint32_t height)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Texture::SetSubData");

    GLES::gl->BindTexture(GL_TEXTURE_2D, m_RendererID);
    GLES::gl->TexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, m_DataFormat, GL_UNSIGNED_BYTE, data);

    if (m_Specification.GenerateMips)
        GLES::gl->GenerateMipmap(GL_TEXTURE_2D);
}

void OpenGLES3Texture::Bind(uint32_t slot) const
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Texture::Bind");

    GLES::gl->ActiveTexture(GL_TEXTURE0 + slot);
    GLES::gl->BindTexture(GL_TEXTURE_2D, m_RendererID);
}

void OpenGLES3Texture::Destroy()
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Texture::Destroy");

    GLES::gl->DeleteTextures(1, &m_RendererID);
}
}
