#pragma once

#include <Tabby/Foundation/Types.h>
#include <Tabby/Asset/AssetBase.h>

#include <bgfx/bgfx.h>

namespace Tabby {

enum class ImageUsage : uint8_t {
    TEXTURE,
    RENDER_TARGET,
    DEPTH_BUFFER
};

enum class ImageFormat : uint8_t {
    R8,
    RB16,
    RGB24,
    RGBA32_SRGB,
    RGBA32_UNORM,
    BGRA32_SRGB,
    BGRA32_UNORM,
    RGB32_HDR,
    RGBA64_HDR,
    RGBA128_HDR,
    D32_S8,
    D32,
    S8,
    BC1,
    BC5,
    BC6h,
    BC7,
    RGBA64_SFLOAT,
    RGB24_UNORM
};

enum class ImageType : uint8_t {
    TYPE_1D,
    TYPE_2D,
    TYPE_3D,
};

struct ImageSpecification {
    UIntVector2 extent = { 0, 0 };
    std::vector<uint8_t> pixels;
    std::filesystem::path path;
    bgfx::TextureFormat::Enum format = bgfx::TextureFormat::RGBA32U;
    /*ImageFormat format = ImageFormat::RGBA32_SRGB;*/
    ImageUsage usage = ImageUsage::TEXTURE;
    ImageType type = ImageType::TYPE_2D;
    uint8_t array_layers = 1;
    bool has_mips = false;

    static ImageSpecification Default()
    {
        ImageSpecification spec;
        spec.extent = { 0, 0 };
        spec.format = bgfx::TextureFormat::RGBA8;
        spec.usage = ImageUsage::TEXTURE;
        spec.type = ImageType::TYPE_2D;
        spec.has_mips = false;
        spec.array_layers = 1;

        return spec;
    };
};

class Image : public AssetBase {
public:
    static Shared<Image> Create(const ImageSpecification& spec, const AssetHandle& id = AssetHandle());

    Image(const ImageSpecification& spec, const AssetHandle& id = AssetHandle());
    ~Image();

    void Update(const ImageSpecification& spec);
    void Destroy();

    const ImageSpecification& GetSpecification() const { return m_Specification; }

    bgfx::TextureHandle Raw() { return m_TextureHandle; }

private:
    ImageSpecification m_Specification;
    bgfx::TextureHandle m_TextureHandle;
};

}
