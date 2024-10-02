#pragma once

#include <Tabby/Renderer/Shader.h>
#include <Tabby/Asset/AssetBase.h>

namespace Tabby {

class Image;
class ShaderBuffer;
class ImageSampler;

struct MaterialSpecification {
    std::string name;
    Shared<Shader> shader;
};

class Material : public AssetBase {
public:
    static Shared<Material> Create(const MaterialSpecification& spec, AssetHandle handle = AssetHandle());
    virtual ~Material() { }

    const std::string& GetName() { return m_Specification.name; }
    Shared<Shader> GetShader() { return m_Specification.shader; }

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
