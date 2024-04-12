#include "Drivers/gles3/OpenGLES3Texture.h"
#include "Drivers/gles3/GLES3.h"
#include "SDL2/include/SDL_surface.h"
#include "tbpch.h"
#include <Tabby/Debug/Instrumentor.h>
#include <glad/gles3.h>

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

OpenGLES3Texture::OpenGLES3Texture(const TextureSpecification& specification, AssetHandle handle, Buffer data)
    : Texture(0)
    , m_Specification(specification)
    , m_Width(m_Specification.Width)
    , m_Height(m_Specification.Height)
{
    // TB_PROFILE_FUNCTION();

    Handle = handle;
    m_InternalFormat = Utils::TabbyImageFormatToGLInternalFormat(m_Specification.Format);
    m_DataFormat = Utils::TabbyImageFormatToGLDataFormat(m_Specification.Format);

    GLES3::GL()->GenTextures(1, &m_RendererID);
    GLES3::GL()->BindTexture(GL_TEXTURE_2D, m_RendererID);

    GLES3::GL()->TexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    GLES3::GL()->GenerateMipmap(GL_TEXTURE_2D);

    GLES3::GL()->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    GLES3::GL()->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GLES3::GL()->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    GLES3::GL()->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    if (data)
        SetData(data);
}
// OpenGLES3Texture::OpenGLES3Texture(const std::string& path)
//     : Texture(0)
//     , m_Path(path)
// {
//     // TB_PROFILE_FUNCTION();
//
//     int width, height, channels;
//     stbi_set_flip_vertically_on_load(1);
//     stbi_uc* data = nullptr;
//     {
// #if defined(TB_PLATFORM_ANDROID)
//         std::vector<unsigned char> imageData;
//
//         SDL_RWops* rw = SDL_RWFromFile(path.c_str(), "rb");
//         if (rw != nullptr) {
//             Sint64 size = SDL_RWsize(rw);
//
//             if (size > 0) {
//                 imageData.resize(size);
//                 Sint64 bytesRead = SDL_RWread(rw, imageData.data(), 1, size);
//                 if (bytesRead != size) {
//                     // Handle read error
//                     TB_CORE_ERROR("Error reading file {0}", path);
//                     imageData.clear(); // Clear the imageData to indicate an error
//                 }
//             }
//
//             SDL_RWclose(rw);
//         } else {
//             // Handle file open error
//             TB_CORE_ERROR("Could not open file {0}", path);
//             TB_CORE_INFO("Current working dir: {0}", std::filesystem::current_path());
//         }
//
//         data = stbi_load_from_memory(imageData.data(), static_cast<int>(imageData.size()), &width, &height, &channels, 0);
//
// #else
//         TB_PROFILE_SCOPE_NAME("stbi_load - OpenGLES3Texture::OpenGLES3Texture(const std::string&)");
//         data = stbi_load(path.c_str(), &width, &height, &channels, 0);
// #endif
//     }
//
//     if (data) {
//         m_IsLoaded = true;
//
//         m_Width = width;
//         m_Height = height;
//
//         GLenum internalFormat = 0, dataFormat = 0;
//         if (channels == 4) {
//             internalFormat = GL_RGBA8;
//             dataFormat = GL_RGBA;
//         } else if (channels == 3) {
//             internalFormat = GL_RGB8;
//             dataFormat = GL_RGB;
//         }
//
//         m_InternalFormat = internalFormat;
//         m_DataFormat = dataFormat;
//
//         TB_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");
//
//         // Generate and bind a texture
//         GLES3::GL()->GenTextures(1, &m_RendererID);
//         GLES3::GL()->BindTexture(GL_TEXTURE_2D, m_RendererID);
//
//         GLES3::GL()->TexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
//
//         // Set texture filtering and wrapping options
//         GLES3::GL()->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//         GLES3::GL()->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//         GLES3::GL()->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//         GLES3::GL()->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//         stbi_image_free(data);
//     }
// }

OpenGLES3Texture::~OpenGLES3Texture()
{
    Destroy();
}

void OpenGLES3Texture::SetData(Buffer data)
{
    // TB_PROFILE_FUNCTION();

    uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
    TB_CORE_ASSERT(data.Size == m_Width * m_Height * bpp, "Data must be the entire texture!");

    GLES3::GL()->BindTexture(GL_TEXTURE_2D, m_RendererID);
    GLES3::GL()->TexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data.Data);
}

void OpenGLES3Texture::SetSubData(void* data, uint32_t width, uint32_t height)
{
    // TB_PROFILE_FUNCTION();

    GLES3::GL()->BindTexture(GL_TEXTURE_2D, m_RendererID);
    GLES3::GL()->TexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, m_DataFormat, GL_UNSIGNED_BYTE, data);
}

void OpenGLES3Texture::Bind(uint32_t slot) const
{
    // TB_PROFILE_FUNCTION();

    GLES3::GL()->ActiveTexture(GL_TEXTURE0 + slot);
    GLES3::GL()->BindTexture(GL_TEXTURE_2D, m_RendererID);
}

void OpenGLES3Texture::Destroy()
{
    TB_PROFILE_SCOPE_NAME("(Textures) Delete");

    GLES3::GL()->DeleteTextures(1, &m_RendererID);
}
}
