#include <Tabby/Asset/AssetFile.h>
#include <Tabby/Asset/AssetManager.h>
#include <Tabby/Renderer/Texture.h>

#include <Tabby/Utils/Utils.h>
#include <stb_image.h>

namespace Tabby {

AssetManager::AssetManager()
{
}

AssetManager::~AssetManager()
{
    FullUnload();
}

void AssetManager::Init()
{
    stbi_set_flip_vertically_on_load(true);
    s_Instance = new AssetManager;
    TB_CORE_INFO("Initialized asset manager");
}

void AssetManager::Shutdown()
{
    delete s_Instance;
}

void AssetManager::FullUnload()
{
    for (auto& [id, asset] : m_AssetRegistry)
        asset->Destroy();
    m_AssetRegistry.clear();
    m_UUIDs.clear();
}

AssetHandle AssetManager::LoadAssetSource(std::filesystem::path path, const AssetHandle& id /*= AssetHandle()*/)
{
    AssetType asset_type = FileExtensionToAssetType(path.extension().string());

    switch (asset_type) {
    case AssetType::MESH_SRC:
        return ImportMeshSource(path, id);
    case AssetType::MATERIAL:
        break;
    case AssetType::AUDIO_SRC:
        break;
    case AssetType::IMAGE_SRC:
        return ImportImageSource(path, id);
    case AssetType::UNKNOWN:
        return AssetHandle(0);
    default:
        return AssetHandle(0);
    }

    return 0;
}

AssetHandle AssetManager::RegisterAsset(Shared<AssetBase> asset, const AssetHandle& id)
{
    asset->Handle = id;
    m_Mutex.lock();
    m_AssetRegistry.emplace(id, asset);
    m_Mutex.unlock();

    return id;
}

AssetHandle AssetManager::ImportMeshSource(std::filesystem::path path, AssetHandle handle)
{
    TB_CORE_ASSERT(false, "Not implemented");

    return 0;
}

AssetHandle AssetManager::ImportImageSource(std::filesystem::path path, AssetHandle handle)
{
    if (m_UUIDs.find(path.string()) != m_UUIDs.end())
        return m_UUIDs.at(path.string());

    Buffer data;
    int image_width, image_height, channels;

    {

        TB_PROFILE_SCOPE_NAME("stbi_load - AssetManager::ImportTexture");
#if defined(TB_PLATFORM_ANDROID)
        std::vector<unsigned char> imageData;

        SDL_RWops* rw = SDL_RWFromFile(path.c_str(), "rb");
        if (rw != nullptr) {
            Sint64 size = SDL_RWsize(rw);

            if (size > 0) {
                imageData.resize(size);
                Sint64 bytesRead = SDL_RWread(rw, imageData.data(), 1, size);
                if (bytesRead != size) {
                    // Handle read error
                    TB_CORE_ERROR("Error reading file {0}", path);
                    imageData.clear(); // Clear the imageData to indicate an error
                }
            }

            SDL_RWclose(rw);
        } else {
            // Handle file open error
            TB_CORE_ERROR("Could not open file {0}", path);
            TB_CORE_INFO("Current working dir: {0}", std::filesystem::current_path());
        }

        data.Data = stbi_load_from_memory(imageData.data(), static_cast<int>(imageData.size()), &width, &height, &channels, 4);
        channels = 4;
#else
        data.Data = stbi_load(path.string().c_str(), &image_width, &image_height, &channels, 4);
#endif

        channels = 4;

        if (data.Data == nullptr) {
            TB_CORE_ERROR("TextureImporter::ImportTexture - Could not load texture from filepath: {}", path.string());
        }
    }

    data.Size = image_width * image_height * channels;

    // Configure file header
    AssetFileHeader file_header = {};
    file_header.header_size = sizeof(AssetFileHeader);
    file_header.asset_type = AssetType::IMAGE_SRC;
    file_header.subresources_size = 0;
    file_header.additional_data = (uint64_t)image_width | (uint64_t)image_height << 32;

    // Compute metadata for subresources
    std::array<AssetFileSubresourceMetadata, 16> subresources_metadata = {};

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
    texture_spec.path = path;

    Shared<Texture> image = Texture::Create(texture_spec, handle, data);
    data.Release();

    m_AssetRegistry.emplace(image->Handle, image);
    m_UUIDs.emplace(path.string(), image->Handle);

    return image->Handle;
}
}
