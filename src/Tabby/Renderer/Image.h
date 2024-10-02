#pragma once

#include <Tabby/Asset/AssetBase.h>

namespace Tabby {

// =======
//  Image
// =======

enum class ImageLayout {
    UNDEFINED = 0,
    GENERAL = 1,
    COLOR_ATTACHMENT = 2,
    DEPTH_STENCIL_ATTACHMENT = 3,
    DEPTH_STENCIL_READ_ONLY = 4,
    SHADER_READ_ONLY = 5,
    TRANSFER_SRC = 6,
    TRANSFER_DST = 7,
    PRESENT_SRC = 1000001002
};

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
    // D32,
    D32_S8,
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
    UIntVector3 extent = { 0, 0, 0 };
    std::vector<byte> pixels;
    std::filesystem::path path;
    ImageFormat format = ImageFormat::RGBA32_SRGB;
    ImageUsage usage = ImageUsage::TEXTURE;
    ImageType type = ImageType::TYPE_2D;
    uint8_t array_layers = 1;
    uint8_t mip_levels = 1;

    static ImageSpecification Default()
    {
        ImageSpecification spec;
        spec.extent = { 0, 0, 0 };
        spec.format = ImageFormat::RGBA32_SRGB;
        spec.usage = ImageUsage::TEXTURE;
        spec.type = ImageType::TYPE_2D;
        spec.mip_levels = 1;
        spec.array_layers = 1;

        return spec;
    };
};

class Image : public AssetBase {
public:
    static Shared<Image> Create(const ImageSpecification& spec, const AssetHandle& id = AssetHandle());

    virtual ~Image() { }

    virtual void Destroy() = 0;

    virtual ImageSpecification GetSpecification() const = 0;
    // virtual void SetLayout(
    //     Shared<DeviceCmdBuffer> cmd_buffer,
    //     ImageLayout new_layout,
    //     PipelineStage src_stage,
    //     PipelineStage dst_stage,
    //     BitMask src_access = 0,
    //     BitMask dst_access = 0)
    //     = 0;

protected:
    Image(AssetHandle handle)
    {
        Handle = handle;
        Type = AssetType::TABBY_IMAGE;
    };
};

// ===============
//	Image sampler
// ===============
enum class SamplerFilteringMode : uint32_t {
    NEAREST,
    LINEAR
};

enum class SamplerAddressMode : uint32_t {
    REPEAT,
    MIRRORED_REPEAT,
    CLAMP,
    CLAMP_BORDER
};

struct ImageSamplerSpecification {
    SamplerFilteringMode min_filtering_mode;
    SamplerFilteringMode mag_filtering_mode;
    SamplerFilteringMode mipmap_filtering_mode;
    SamplerAddressMode address_mode;
    float min_lod;
    float max_lod;
    float lod_bias;
    uint8_t anisotropic_filtering_level;
};

class ImageSampler {
public:
    static Shared<ImageSampler> Create(const ImageSamplerSpecification& spec);

    virtual void Destroy() = 0;
};

}
