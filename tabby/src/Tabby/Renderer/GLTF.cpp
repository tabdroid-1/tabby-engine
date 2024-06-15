#include <Tabby/Renderer/GLTF.h>
#include <Tabby/Renderer/Texture.h>
#include <Tabby/Renderer/Mesh.h>
#include <Tabby/Renderer/Material.h>

#include <Tabby/Asset/AssetFile.h>
#include <Tabby/Asset/AssetManager.h>
#include <Tabby/Renderer/Buffer.h>
#include <Tabby/Renderer/VertexArray.h>
#include <Tabby/Renderer/RenderCommand.h>

#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/tools.hpp>
#include <stb/stb_image.h>

namespace Tabby {

GLTF::GLTF(const std::filesystem::path& filePath)
{
    std::string err;
    std::string warn;

    fastgltf::Parser parser;

    constexpr auto gltfOptions = fastgltf::Options::DontRequireValidAssetMember | fastgltf::Options::AllowDouble | fastgltf::Options::LoadGLBBuffers | fastgltf::Options::LoadExternalBuffers | fastgltf::Options::LoadExternalImages | fastgltf::Options::GenerateMeshIndices;

    auto gltfFile = fastgltf::MappedGltfFile::FromPath(filePath);
    TB_CORE_ASSERT_TAGGED(bool(gltfFile), "Failed to open glTF file: {0}", fastgltf::getErrorMessage(gltfFile.error()));

    auto asset = parser.loadGltf(gltfFile.get(), filePath.parent_path(), gltfOptions);

    TB_CORE_ASSERT_TAGGED(asset.error() == fastgltf::Error::None, "Failed to load glTF file: {0}", fastgltf::getErrorMessage(asset.error()));

    m_Asset = std::move(asset.get());
    // TB_CORE_ASSERT_TAGGED(false, "Unknown File Format");

    // Add a default material
    auto& defaultMaterial = m_Materials.emplace_back();

    LoadImages();
    LoadMaterials();
    LoadMeshes();
}

void GLTF::LoadImages()
{
    auto getLevelCount = [](int width, int height) -> int {
        return static_cast<int>(1 + floor(log2(width > height ? width : height)));
    };

    for (auto& image : m_Asset.images) {
        Shared<Texture> imageptr;
        std::visit(fastgltf::visitor {
                       [](auto& arg) {},
                       [&](fastgltf::sources::URI& filePath) {
                           TB_CORE_ASSERT_TAGGED(filePath.fileByteOffset == 0, "File offsets not supported");
                           TB_CORE_ASSERT_TAGGED(filePath.uri.isLocalPath(), "Only local files allowed"); // We're only capable of loading local files.

                           const std::string path(filePath.uri.path().begin(), filePath.uri.path().end()); // Thanks C++.

                           stbi_set_flip_vertically_on_load(false);
                           Buffer data;
                           int image_width, image_height, channels;
                           data.Data = stbi_load(path.c_str(), &image_width, &image_height, &channels, STBI_rgb_alpha);
                           data.Size = image_width * image_height * channels;
                           if (data.Data == nullptr) {
                               TB_CORE_ERROR("TextureImporter::ImportTexture - Could not load texture from filepath: {0}", path);
                           }

                           AssetFileHeader file_header = {};
                           file_header.header_size = sizeof(AssetFileHeader);
                           file_header.asset_type = AssetType::IMAGE_SRC;
                           file_header.subresources_size = 0;
                           file_header.additional_data = (uint64_t)image_width | (uint64_t)image_height << 32;

                           TextureSpecification texture_spec = {};
                           texture_spec.Format = ImageFormat::RGBA32_UNORM;
                           texture_spec.Width = image_width;
                           texture_spec.Height = image_height;
                           texture_spec.type = ImageType::TYPE_2D;
                           texture_spec.usage = ImageUsage::TEXTURE;
                           texture_spec.array_layers = 1;
                           texture_spec.path = path;
                           texture_spec.GenerateMips = true;

                           AssetHandle handle;
                           imageptr = Texture::Create(texture_spec, handle, data);
                           data.Release();
                       },
                       [&](fastgltf::sources::Array& vector) {
                           Buffer data;
                           int image_width, image_height, channels;

                           stbi_set_flip_vertically_on_load(false);
                           data.Data = stbi_load_from_memory(vector.bytes.data(), static_cast<int>(vector.bytes.size()), &image_width, &image_height, &channels, STBI_rgb_alpha);
                           data.Size = image_width * image_height * channels;
                           if (data.Data == nullptr) {
                               TB_CORE_ERROR("TextureImporter::ImportTexture - Could not load texture from data");
                           }

                           AssetFileHeader file_header = {};
                           file_header.header_size = sizeof(AssetFileHeader);
                           file_header.asset_type = AssetType::IMAGE_SRC;
                           file_header.subresources_size = 0;
                           file_header.additional_data = (uint64_t)image_width | (uint64_t)image_height << 32;

                           TextureSpecification texture_spec = {};
                           // switch (channels) {
                           // case 3:
                           //     texture_spec.Format = ImageFormat::RGB8;
                           //     break;
                           // case 4:
                           texture_spec.Format = ImageFormat::RGBA8;
                           //     break;
                           // }
                           texture_spec.Width = image_width;
                           texture_spec.Height = image_height;
                           texture_spec.type = ImageType::TYPE_2D;
                           texture_spec.usage = ImageUsage::TEXTURE;
                           texture_spec.array_layers = 1;
                           // texture_spec.path = path;
                           texture_spec.GenerateMips = true;

                           AssetHandle handle;
                           imageptr = Texture::Create(texture_spec, handle, data);
                           data.Release();
                       },
                       [&](fastgltf::sources::BufferView& view) {
                           auto& bufferView = m_Asset.bufferViews[view.bufferViewIndex];
                           auto& buffer = m_Asset.buffers[bufferView.bufferIndex];
                           // Yes, we've already loaded every buffer into some GL buffer. However, with GL it's simpler
                           // to just copy the buffer data again for the texture. Besides, this is just an example.
                           std::visit(fastgltf::visitor {
                                          // We only care about VectorWithMime here, because we specify LoadExternalBuffers, meaning
                                          // all buffers are already loaded into a vector.
                                          [](auto& arg) {},
                                          [&](fastgltf::sources::Array& vector) {
                                              Buffer data;
                                              int image_width, image_height, channels;

                                              stbi_set_flip_vertically_on_load(false);
                                              data.Data = stbi_load_from_memory(vector.bytes.data(), static_cast<int>(vector.bytes.size()), &image_width, &image_height, &channels, 4);
                                              data.Size = image_width * image_height * channels;
                                              if (data.Data == nullptr) {
                                                  TB_CORE_ERROR("TextureImporter::ImportTexture - Could not load texture from data");
                                              }

                                              AssetFileHeader file_header = {};
                                              file_header.header_size = sizeof(AssetFileHeader);
                                              file_header.asset_type = AssetType::IMAGE_SRC;
                                              file_header.subresources_size = 0;
                                              file_header.additional_data = (uint64_t)image_width | (uint64_t)image_height << 32;

                                              TextureSpecification texture_spec = {};
                                              // switch (channels) {
                                              // case 3:
                                              //     texture_spec.Format = ImageFormat::RGB8;
                                              //     break;
                                              // case 4:
                                              texture_spec.Format = ImageFormat::RGBA8;
                                              //     break;
                                              // }
                                              texture_spec.Width = image_width;
                                              texture_spec.Height = image_height;
                                              texture_spec.type = ImageType::TYPE_2D;
                                              texture_spec.usage = ImageUsage::TEXTURE;
                                              texture_spec.array_layers = 1;
                                              // texture_spec.path = path;
                                              texture_spec.GenerateMips = true;

                                              AssetHandle handle;
                                              imageptr = Texture::Create(texture_spec, handle, data);
                                              data.Release();
                                          } },
                               buffer.data);
                       },
                   },
            image.data);

        m_Images.emplace_back(imageptr);
        // viewer->textures.emplace_back(Texture { texture });
        // return true;
    }
}

void GLTF::LoadMaterials()
{
    for (auto& material : m_Asset.materials) {

        auto uniforms = m_Materials.emplace_back();
        uniforms.alphaCutoff = material.alphaCutoff;

        uniforms.baseColorFactor = glm::vec4(material.pbrData.baseColorFactor.x(), material.pbrData.baseColorFactor.y(), material.pbrData.baseColorFactor.z(), material.pbrData.baseColorFactor.w());
        if (material.pbrData.baseColorTexture.has_value())
            uniforms.flags |= MaterialUniformFlags::HasAlbedoMap;

        if (material.normalTexture.has_value())
            uniforms.flags |= MaterialUniformFlags::HasNormalMap;

        if (material.pbrData.metallicRoughnessTexture.has_value())
            uniforms.flags |= MaterialUniformFlags::HasRoughnessMap;

        if (material.occlusionTexture.has_value())
            uniforms.flags |= MaterialUniformFlags::HasOcclusionMap;

        m_Materials.emplace_back(uniforms);

        // auto tabbyMaterial = m_Materials.emplace_back();
        // tabbyMaterial->SetAlbedoColor({ material.pbrData.baseColorFactor.x(), material.pbrData.baseColorFactor.y(), material.pbrData.baseColorFactor.z(), material.pbrData.baseColorFactor.w() });
        // if (material.pbrData.baseColorTexture.has_value()) {
        //     auto& albedoMap = material.pbrData.baseColorTexture;
        //     tabbyMaterial->SetAlbedoMap(m_Images[albedoMap->texCoordIndex]);
        //     tabbyMaterial->SetAlbedoMapOffset({ albedoMap->transform->uvOffset.x(), albedoMap->transform->uvOffset.y() });
        //     tabbyMaterial->SetAlbedoMapTiling({ albedoMap->transform->uvScale.x(), albedoMap->transform->uvScale.y() });
        //     // albedoMap->transform->texCoordIndex
        // }
        //
        // if (material.normalTexture.has_value())
        //     tabbyMaterial->SetNormalMap(m_Images[material.normalTexture->textureIndex]);
        //
        // if (material.pbrData.metallicRoughnessTexture.has_value())
        //     tabbyMaterial->SetMetallicMap(m_Images[material.pbrData.metallicRoughnessTexture->textureIndex]);
        //
        // if (material.occlusionTexture.has_value())
        //     tabbyMaterial->SetAmbientOcclusionMap(m_Images[material.occlusionTexture->textureIndex]);
    }
}

void GLTF::LoadMeshes()
{
    for (auto& mesh : m_Asset.meshes) {
        // auto tabbyMesh = m_Meshes.p;

        for (auto it = mesh.primitives.begin(); it != mesh.primitives.end(); ++it) {

            auto tabbyMesh = CreateShared<Mesh>();
            tabbyMesh->SetName(mesh.name.c_str());
            auto* positionIt = it->findAttribute("POSITION");
            TB_CORE_ASSERT(positionIt != it->attributes.end()); // A mesh primitive is required to hold the POSITION attribute.
            TB_CORE_ASSERT(it->indicesAccessor.has_value()); // We specify GenerateMeshIndices, so we should always have indices

            auto index = std::distance(mesh.primitives.begin(), it);
            // auto& primitive = .primitives[index];
            tabbyMesh->SetPrimitiveType((Mesh::PrimitiveType)fastgltf::to_underlying(it->type));

            std::size_t materialUniformsIndex;
            std::size_t baseColorTexcoordIndex;
            Shared<Material> tabbyMaterial = CreateShared<Material>("UnlitMaterial", "shaders/gl33/Renderer3D_MeshUnlit.glsl");

            if (it->materialIndex.has_value()) {
                materialUniformsIndex = it->materialIndex.value() + 1; // Adjust for default material
                auto& material = m_Asset.materials[it->materialIndex.value()];

                auto& baseColorTexture = material.pbrData.baseColorTexture;
                if (baseColorTexture.has_value()) {
                    auto& texture = m_Asset.textures[baseColorTexture->textureIndex];
                    if (!texture.imageIndex.has_value())
                        return; // Huh?

                    tabbyMaterial->SetAlbedoMap(m_Images[texture.imageIndex.value()]);

                    if (baseColorTexture->transform && baseColorTexture->transform->texCoordIndex.has_value()) {
                        baseColorTexcoordIndex = baseColorTexture->transform->texCoordIndex.value();
                    } else {
                        baseColorTexcoordIndex = material.pbrData.baseColorTexture->texCoordIndex;
                    }
                }
            } else {
                materialUniformsIndex = 0;
            }

            std::vector<Mesh::Vertex> meshVertices;
            {
                // Position
                auto& positionAccessor = m_Asset.accessors[positionIt->second];
                if (!positionAccessor.bufferViewIndex.has_value())
                    continue;

                meshVertices.resize(positionAccessor.count);

                fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(m_Asset, positionAccessor, [&](fastgltf::math::fvec3 pos, std::size_t idx) {
                    meshVertices[idx].Position = glm::vec4(pos.x(), pos.y(), pos.z(), 0.0f);
                    meshVertices[idx].TexCoords = glm::vec2();
                });
            }

            auto texcoordAttribute = std::string("TEXCOORD_") + std::to_string(baseColorTexcoordIndex);
            if (const auto* texcoord = it->findAttribute(texcoordAttribute); texcoord != it->attributes.end()) {
                // Tex coord
                auto& texCoordAccessor = m_Asset.accessors[texcoord->second];
                if (!texCoordAccessor.bufferViewIndex.has_value())
                    continue;

                fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec2>(m_Asset, texCoordAccessor, [&](fastgltf::math::fvec2 uv, std::size_t idx) {
                    meshVertices[idx].TexCoords = glm::vec2(uv.x(), uv.y());
                });
            }

            auto& indexAccessor = m_Asset.accessors[it->indicesAccessor.value()];
            if (!indexAccessor.bufferViewIndex.has_value())
                return;
            std::vector<uint32_t> meshIndices(indexAccessor.count);

            fastgltf::copyFromAccessor<std::uint32_t>(m_Asset, indexAccessor, (std::uint32_t*)meshIndices.data());

            tabbyMesh->SetMaterial(tabbyMaterial);
            tabbyMesh->SetVertices(meshVertices);
            tabbyMesh->SetIndices(meshIndices);

            tabbyMesh->Create();
            m_Meshes.push_back(tabbyMesh);
        }
    }
}

void GLTF::Draw()
{
    for (auto mesh : m_Meshes) {
        mesh->Render();
    }
}
}
