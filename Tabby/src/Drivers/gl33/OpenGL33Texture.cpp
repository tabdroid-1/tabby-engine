#include "Drivers/gl33/OpenGL33Texture.h"
#include "Drivers/gl33/GL33.h"
#include "tbpch.h"
#include <Tabby/Core/Log.h>
#include <stb_image.h>

namespace Tabby {

namespace Utils {

    static GLenum TabbyImageFormatToGLDataFormat(ImageFormat format)
    {
        switch (format) {
        case ImageFormat::RGB8:
            return GL_RGB;
        case ImageFormat::RGBA8:
            return GL_RGBA;
        }

        TB_CORE_ASSERT(false);
        return 0;
    }

    static GLenum TabbyImageFormatToGLInternalFormat(ImageFormat format)
    {
        switch (format) {
        case ImageFormat::RGB8:
            return GL_RGB8;
        case ImageFormat::RGBA8:
            return GL_RGBA8;
        }

        TB_CORE_ASSERT(false);
        return 0;
    }

}

OpenGL33Texture2D::OpenGL33Texture2D(const TextureSpecification& specification)
    : m_Specification(specification)
    , m_Width(m_Specification.Width)
    , m_Height(m_Specification.Height)
{
    TB_PROFILE_SCOPE_NAME("(Texture) Generate (spec)");

    m_InternalFormat = Utils::TabbyImageFormatToGLInternalFormat(m_Specification.Format);
    m_DataFormat = Utils::TabbyImageFormatToGLDataFormat(m_Specification.Format);

    GL33::GL()->GenTextures(1, &m_RendererID);
    GL33::GL()->BindTexture(GL_TEXTURE_2D, m_RendererID);

    GL33::GL()->TexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    GL33::GL()->PixelStorei(GL_UNPACK_ALIGNMENT, specification.UnpackAlignment);

    GL33::GL()->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    GL33::GL()->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GL33::GL()->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    GL33::GL()->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

OpenGL33Texture2D::OpenGL33Texture2D(const std::string& path)
    : m_Path(path)
{
    TB_PROFILE_SCOPE_NAME("(Texture) Generate (path)");

    int width, height, channels;
    stbi_set_flip_vertically_on_load(1);
    stbi_uc* data = nullptr;
    {
        TB_PROFILE_SCOPE_NAME("stbi_load - OpenGL33Texture2D::OpenGL33Texture2D(const std::string&)");
        data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    }

    if (data) {
        m_IsLoaded = true;

        m_Width = width;
        m_Height = height;

        GLenum internalFormat = 0, dataFormat = 0;
        if (channels == 4) {
            internalFormat = GL_RGBA8;
            dataFormat = GL_RGBA;
        } else if (channels == 3) {
            internalFormat = GL_RGB8;
            dataFormat = GL_RGB;
        } else {

            TB_CORE_ERROR("Format not supported! \"{0}\"", path);
        }

        m_InternalFormat = internalFormat;
        m_DataFormat = dataFormat;

        TB_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

        GL33::GL()->GenTextures(1, &m_RendererID);
        GL33::GL()->BindTexture(GL_TEXTURE_2D, m_RendererID);

        GL33::GL()->TexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, dataFormat, GL_UNSIGNED_BYTE, data);

        GL33::GL()->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        GL33::GL()->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        GL33::GL()->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        GL33::GL()->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        stbi_image_free(data);
    } else {
        TB_CORE_ERROR("Failed to load \"{0}\"!", path);
        TB_CORE_ERROR("  Failure reason: {0}", stbi_failure_reason());
    }
}

OpenGL33Texture2D::~OpenGL33Texture2D()
{
    TB_PROFILE_SCOPE_NAME("(Textures) Delete");

    GL33::GL()->DeleteTextures(1, &m_RendererID);
}

void OpenGL33Texture2D::SetData(void* data, uint32_t size)
{
    TB_PROFILE_SCOPE_NAME("(Texture) Set data");

    uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
    // Ensure that the provided size matches the expected size
    TB_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be the entire texture!");

    GL33::GL()->BindTexture(GL_TEXTURE_2D, m_RendererID);
    GL33::GL()->TexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
}

void OpenGL33Texture2D::SetSubData(void* data, uint32_t width, uint32_t height)
{
    TB_PROFILE_SCOPE_NAME("(Texture) Set sub data");

    GL33::GL()->BindTexture(GL_TEXTURE_2D, m_RendererID);
    GL33::GL()->TexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, m_DataFormat, GL_UNSIGNED_BYTE, data);
}

void OpenGL33Texture2D::Bind(uint32_t slot) const
{
    TB_PROFILE_SCOPE_NAME("(Texture) bind");

    GL33::GL()->ActiveTexture(GL_TEXTURE0 + slot);
    GL33::GL()->BindTexture(GL_TEXTURE_2D, m_RendererID);
}
}
