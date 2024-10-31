#pragma once

#include <Tabby/Renderer/Pipeline.h>
#include <Tabby/Asset/AssetBase.h>

namespace Tabby {

class Image;
class ShaderBuffer;
class ImageSampler;

struct MaterialData {
    MaterialData(const std::string& name, uint64_t offset, void* data, uint64_t size)
        : type(Type::Uniform)
        , name(name)
        , offset(offset)
        , data(data)
        , data_size(size)
    {
    }

    MaterialData(const std::string& name, uint16_t array_element, Shared<Image> img, Shared<ImageSampler> samp)
        : type(Type::Image)
        , name(name)
        , array_element(array_element)
        , image(img)
        , sampler(samp)
    {
    }

    enum class Type {
        Uniform,
        Image
    } type;

    std::string name;

    uint64_t offset;
    void* data;
    uint64_t data_size;

    uint16_t array_element;
    Shared<Image> image;
    Shared<ImageSampler> sampler;
};

struct MaterialSpecification {
    std::string name;
    Shared<Pipeline> pipeline;
};

class Material : public AssetBase {
public:
    static Shared<Material> Create(const MaterialSpecification& spec, AssetHandle handle = AssetHandle());
    virtual ~Material() { }

    const std::string& GetName() { return m_Specification.name; }
    Shared<Pipeline> GetPipeline() { return m_Specification.pipeline; }

    virtual void UploadData(const std::string& name, uint64_t offset, void* data, uint64_t data_size) = 0;
    virtual void UploadData(const std::string& name, uint16_t array_element, Shared<Image> image, Shared<ImageSampler> sampler) = 0;

    virtual void Destroy() override = 0;

protected:
    Material(AssetHandle handle, MaterialSpecification spec)
    {
        Handle = handle;
        Type = AssetType::TABBY_MATERIAL;
        m_Specification = spec;
    };

    MaterialSpecification m_Specification;
};

}
