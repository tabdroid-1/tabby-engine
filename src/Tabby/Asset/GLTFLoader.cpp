// #include <Tabby/Renderer/VertexArray.h>
#include <Tabby/Asset/AssetCompressor.h>
#include <Tabby/Asset/AssetManager.h>
#include <Tabby/Renderer/Renderer.h>
// #include <Tabby/Renderer/Material.h>
#include <Tabby/Asset/GLTFLoader.h>
// #include <Tabby/Renderer/Buffer.h>
#include <Tabby/Renderer/Image.h>
#include <Tabby/Renderer/Mesh.h>
#include <Tabby/World/Entity.h>
#include <Tabby/World/World.h>
#include <Tabby/Utils/Utils.h>

#include <glm/gtx/quaternion.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/tools.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/tools.hpp>
#include <fastgltf/core.hpp>
#include <fastgltf/core.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <stb_image.h>

namespace Tabby {

std::vector<GLTFLoader::EntityGLTFMeshData> GLTFLoader::Parse(const std::filesystem::path& filePath)
{
    TB_PROFILE_SCOPE_NAME("Tabby::GLTFLoader::Parse");

    GLTFParserData data;

    std::string err;
    std::string warn;

    fastgltf::Parser parser;

    constexpr auto gltfOptions = fastgltf::Options::DontRequireValidAssetMember | fastgltf::Options::AllowDouble | fastgltf::Options::LoadExternalBuffers | fastgltf::Options::LoadExternalImages | fastgltf::Options::GenerateMeshIndices;

#ifdef TB_PLATFORM_ANDROID
    auto asset = parser.loadFileFromApk(filePath);
#else
    auto gltfFile = fastgltf::MappedGltfFile::FromPath(filePath);

    std::string message = "Failed to load glTF file: " + std::string(fastgltf::getErrorMessage(gltfFile.error()));
    TB_CORE_ASSERT_TAGGED(bool(gltfFile), message);
    auto asset = parser.loadGltf(gltfFile.get(), filePath.parent_path(), gltfOptions);
#endif

    message = "Failed to load glTF file: " + std::string(fastgltf::getErrorMessage(asset.error()));
    TB_CORE_ASSERT_TAGGED(asset.error() == fastgltf::Error::None, message);

    data.fastgltf_asset = std::move(asset.get());
    data.materials.emplace_back();

    LoadImages(data);
    LoadMaterials(data);
    LoadMeshes(data);

    return data.materials;
}

void GLTFLoader::LoadImages(GLTFParserData& data)
{
    TB_PROFILE_SCOPE_NAME("Tabby::GLTFLoader::LoadImages");

    for (auto& image : data.fastgltf_asset.images) {
        Shared<Image> imageptr;
        std::visit(fastgltf::visitor {
                       [](auto& arg) {},
                       [&](fastgltf::sources::URI& filePath) {
                           TB_CORE_ASSERT_TAGGED(filePath.fileByteOffset == 0, "File offsets not supported");
                           TB_CORE_ASSERT_TAGGED(filePath.uri.isLocalPath(), "Only local files allowed"); // We're only capable of loading local files.

                           const std::string path(filePath.uri.path());

                           AssetHandle handle = AssetManager::LoadAssetSource(path);
                           imageptr = AssetManager::GetAsset<Image>(handle);
                       },
                       [&](fastgltf::sources::Array& vector) {
                           int image_width, image_height, channels;

                           stbi_set_flip_vertically_on_load(false);
                           RGBA32* raw_image_data = (RGBA32*)stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(vector.bytes.data()), static_cast<int>(vector.bytes.size()), &image_width, &image_height, &channels, 4);

                           std::vector<RGBA32> image_data;
                           image_data.assign(raw_image_data, raw_image_data + (image_width * image_height));

                           std::vector<RGBA32> full_image_data = AssetCompressor::GenerateMipMaps(image_data, image_width, image_height);

                           std::vector<byte> raw(full_image_data.size() * sizeof(RGBA32));
                           memcpy(raw.data(), full_image_data.data(), full_image_data.size() * sizeof(RGBA32));
                           full_image_data.clear();

                           ImageSpecification texture_spec = {};
                           texture_spec.pixels = std::move(raw);
                           texture_spec.format = ImageFormat::RGBA32_UNORM;
                           texture_spec.type = ImageType::TYPE_2D;
                           texture_spec.usage = ImageUsage::TEXTURE;
                           texture_spec.extent = { (uint32_t)image_width, (uint32_t)image_height, 1 };
                           texture_spec.array_layers = 1;
                           texture_spec.mip_levels = Utils::ComputeNumMipLevelsBC7(image_width, image_height) + 1;
                           texture_spec.path = "gltf_bin_image";

                           AssetHandle handle;
                           imageptr = Image::Create(texture_spec, handle);
                       },
                       [&](fastgltf::sources::BufferView& view) {
                           auto& bufferView = data.fastgltf_asset.bufferViews[view.bufferViewIndex];
                           auto& buffer = data.fastgltf_asset.buffers[bufferView.bufferIndex];
                           std::visit(fastgltf::visitor {
                                          [](auto& arg) {},
                                          [&](fastgltf::sources::Array& vector) {
                                              int image_width, image_height, channels;

                                              stbi_set_flip_vertically_on_load(false);
                                              RGBA32* raw_image_data = (RGBA32*)stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(vector.bytes.data()), static_cast<int>(vector.bytes.size()), &image_width, &image_height, &channels, 4);

                                              std::vector<RGBA32> image_data;
                                              image_data.assign(raw_image_data, raw_image_data + (image_width * image_height));

                                              std::vector<RGBA32> full_image_data = AssetCompressor::GenerateMipMaps(image_data, image_width, image_height);

                                              std::vector<byte> raw(full_image_data.size() * sizeof(RGBA32));
                                              memcpy(raw.data(), full_image_data.data(), full_image_data.size() * sizeof(RGBA32));
                                              full_image_data.clear();

                                              ImageSpecification texture_spec = {};
                                              texture_spec.pixels = std::move(raw);
                                              texture_spec.format = ImageFormat::RGBA32_UNORM;
                                              texture_spec.type = ImageType::TYPE_2D;
                                              texture_spec.usage = ImageUsage::TEXTURE;
                                              texture_spec.extent = { (uint32_t)image_width, (uint32_t)image_height, 1 };
                                              texture_spec.array_layers = 1;
                                              texture_spec.mip_levels = Utils::ComputeNumMipLevelsBC7(image_width, image_height) + 1;
                                              texture_spec.path = "gltf_bin_image";

                                              AssetHandle handle;
                                              imageptr = Image::Create(texture_spec, handle);
                                          } },
                               buffer.data);
                       },
                   },
            image.data);

        data.images.emplace_back(imageptr);
    }
}

void GLTFLoader::LoadMaterials(GLTFParserData& data)
{
    TB_PROFILE_SCOPE_NAME("Tabby::GLTFLoader::LoadMaterials");

    for (auto& material : data.fastgltf_asset.materials) {

        auto uniforms = data.materials.emplace_back();
        uniforms.alpha_cutoff = material.alphaCutoff;

        uniforms.base_color_factor = Vector4(material.pbrData.baseColorFactor.x(), material.pbrData.baseColorFactor.y(), material.pbrData.baseColorFactor.z(), material.pbrData.baseColorFactor.w());
        if (material.pbrData.baseColorTexture.has_value())
            uniforms.flags |= MaterialUniformFlags::HasAlbedoMap;

        if (material.normalTexture.has_value())
            uniforms.flags |= MaterialUniformFlags::HasNormalMap;

        if (material.pbrData.metallicRoughnessTexture.has_value())
            uniforms.flags |= MaterialUniformFlags::HasRoughnessMap;

        if (material.occlusionTexture.has_value())
            uniforms.flags |= MaterialUniformFlags::HasOcclusionMap;

        data.materials.emplace_back(uniforms);
    }
}

void GLTFLoader::LoadMeshes(GLTFParserData& data)
{
    TB_PROFILE_SCOPE_NAME("Tabby::GLTFLoader::LoadMeshes");

    std::vector<std::pair<uint32_t, Shared<Mesh>>> tabbyMeshes;

    int meshID = 0;
    int primitiveID = 0;

    for (auto& mesh : data.fastgltf_asset.meshes) {

        for (auto it = mesh.primitives.begin(); it != mesh.primitives.end(); ++it) {

            MeshSpecification mesh_spec;
            mesh_spec.name = mesh.name;

            auto* positionIt = it->findAttribute("POSITION");
            TB_CORE_ASSERT(positionIt != it->attributes.end()); // A mesh primitive is required to hold the POSITION attribute.
            TB_CORE_ASSERT(it->indicesAccessor.has_value()); // We specify GenerateMeshIndices, so we should always have indices

            std::size_t baseColorTexcoordIndex;

            if (it->materialIndex.has_value()) {
                auto& material = data.fastgltf_asset.materials[it->materialIndex.value()];

                auto& baseColorTexture = material.pbrData.baseColorTexture;
                if (baseColorTexture.has_value()) {
                    auto& texture = data.fastgltf_asset.textures[baseColorTexture->textureIndex];
                    if (!texture.imageIndex.has_value())
                        return;

                    auto test = data.images[texture.imageIndex.value()];
                    data.materials[primitiveID].images["albedo"] = data.images[texture.imageIndex.value()];

                    if (baseColorTexture->transform && baseColorTexture->transform->texCoordIndex.has_value()) {
                        baseColorTexcoordIndex = baseColorTexture->transform->texCoordIndex.value();
                    } else {
                        baseColorTexcoordIndex = material.pbrData.baseColorTexture->texCoordIndex;
                    }
                }
            }

            std::vector<Vertex> meshVertices;
            {
                // Position
                auto& positionAccessor = data.fastgltf_asset.accessors[positionIt->accessorIndex];
                if (!positionAccessor.bufferViewIndex.has_value())
                    continue;

                meshVertices.resize(positionAccessor.count);

                fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(data.fastgltf_asset, positionAccessor, [&](fastgltf::math::fvec3 pos, std::size_t idx) {
                    meshVertices[idx].position = Vector3(pos.x(), pos.y(), pos.z());
                });
            }

            auto texcoordAttribute = std::string("TEXCOORD_") + std::to_string(baseColorTexcoordIndex);
            if (const auto* texcoord = it->findAttribute(texcoordAttribute); texcoord != it->attributes.end()) {
                // Tex coord
                auto& texCoordAccessor = data.fastgltf_asset.accessors[texcoord->accessorIndex];
                if (!texCoordAccessor.bufferViewIndex.has_value())
                    continue;

                fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec2>(data.fastgltf_asset, texCoordAccessor, [&](fastgltf::math::fvec2 uv, std::size_t idx) {
                    meshVertices[idx].uv = Vector2(uv.x(), uv.y());
                });
            }

            auto& indexAccessor = data.fastgltf_asset.accessors[it->indicesAccessor.value()];
            if (!indexAccessor.bufferViewIndex.has_value())
                return;
            std::vector<uint32_t> meshIndices(indexAccessor.count);

            fastgltf::copyFromAccessor<std::uint32_t>(data.fastgltf_asset, indexAccessor, (std::uint32_t*)meshIndices.data());

            mesh_spec.vertex_data = meshVertices.data();
            mesh_spec.vertex_data_size = meshVertices.size() * sizeof(Vertex);

            mesh_spec.index_data = meshIndices.data();
            mesh_spec.index_data_size = meshIndices.size() * sizeof(uint32_t);
            mesh_spec.index_type = MeshIndexType::UINT32;

            auto tabbyMesh = CreateShared<Mesh>(mesh_spec);

            data.materials[primitiveID].mesh = tabbyMesh;
            tabbyMeshes.push_back(std::make_pair(meshID, tabbyMesh));

            primitiveID++;
        }
        meshID++;
    }

    // auto SceneEntity = Tabby::World::CreateEntity("Scene");
    //
    // for (auto& node : data.fastgltf_asset.nodes) {
    //
    //     auto ent = Tabby::World::CreateEntity(node.name.c_str());
    //
    //     for (auto mesh : tabbyMeshes) {
    //         if (mesh.first == node.meshIndex.value()) {
    //             auto childEnt = Tabby::World::CreateEntity(mesh.second->GetName());
    //             auto& mC = childEnt.AddComponent<MeshComponent>();
    //             mC.m_Mesh = mesh.second;
    //             ent.AddChild(childEnt);
    //         }
    //     }
    //
    //     auto& tc = ent.GetComponent<TransformComponent>();
    //     std::visit(fastgltf::visitor { [&](const fastgltf::math::fmat4x4& matrix) {
    //                                       fastgltf::math::fvec3 scale;
    //                                       fastgltf::math::fquat rotation;
    //                                       fastgltf::math::fvec3 translation;
    //                                       fastgltf::math::decomposeTransformMatrix(matrix, scale, rotation, translation);
    //
    //                                       Vector3 Gscale = { scale.x(), scale.y(), scale.z() };
    //                                       Quaternion Grotation = { rotation.w(), rotation.x(), rotation.y(), rotation.z() };
    //                                       Vector3 Gtranslation = { translation.x(), translation.y(), translation.z() };
    //                                       Matrix4 rotMat = glm::toMat4(Grotation);
    //                                       tc.ApplyTransform(glm::translate(Matrix4(1.0f), (Vector3&)Gtranslation) * rotMat * glm::scale(Matrix4(1.0f), (Vector3&)Gscale));
    //                                   },
    //                    [&](const fastgltf::TRS& trs) {
    //                        Vector3 Gscale = { trs.scale.x(), trs.scale.y(), trs.scale.z() };
    //                        Quaternion Grotation = { trs.rotation.w(), trs.rotation.x(), trs.rotation.y(), trs.rotation.z() };
    //                        Vector3 Gtranslation = { trs.translation.x(), trs.translation.y(), trs.translation.z() };
    //                        Matrix4 rotMat = glm::toMat4(Grotation);
    //                        tc.ApplyTransform(glm::translate(Matrix4(1.0f), (Vector3&)Gtranslation) * rotMat * glm::scale(Matrix4(1.0f), (Vector3&)Gscale));
    //                    } },
    //         node.transform);
    //
    //     SceneEntity.AddChild(ent);
    // }
}
}
