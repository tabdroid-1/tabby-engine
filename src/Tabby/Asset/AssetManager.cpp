#include <Tabby/Asset/AssetCompressor.h>
#include <Tabby/Asset/AssetManager.h>
#include <Tabby/Renderer/Renderer.h>
#include <Tabby/Asset/AssetFile.h>
#include <Tabby/Renderer/Image.h>
// #include <Tabby/Renderer/Font.h>
#include <Tabby/Utils/Utils.h>
#include <Tabby/Audio/Audio.h>

#include <SDL_rwops.h>
#include <stb_image.h>

namespace Tabby {

AssetManager::AssetManager()
{
    TB_PROFILE_SCOPE_NAME("Tabby::AssetManager::Constructor");
}

AssetManager::~AssetManager()
{
    TB_PROFILE_SCOPE_NAME("Tabby::AssetManager::Destructor");
    FullUnload();
}

void AssetManager::FullUnload()
{
    TB_PROFILE_SCOPE_NAME("Tabby::AssetManager::FullUnload");

    for (auto& [id, asset] : m_AssetRegistry)
        asset->Destroy();
    m_AssetRegistry.clear();
    m_UUIDs.clear();
}

AssetHandle AssetManager::LoadAssetSource(std::filesystem::path path, const AssetHandle& id /*= AssetHandle()*/)
{
    TB_PROFILE_SCOPE_NAME("Tabby::AssetManager::LoadAssetSource");

    AssetType asset_type = FileExtensionToAssetType(path.extension().string());

    switch (asset_type) {
    case AssetType::MESH_SRC:
        return ImportMeshSource(path, id);
    case AssetType::MATERIAL:
        break;
    case AssetType::AUDIO_SRC:
        return ImportAudioSource(path, id);
    case AssetType::IMAGE_SRC:
        return ImportImageSource(path, id);
    case AssetType::FONT_SRC:
        return ImportFontSource(path, id);
    case AssetType::UNKNOWN:
        return AssetHandle(0);
    default:
        return AssetHandle(0);
    }

    return 0;
}

AssetHandle AssetManager::RegisterAsset(Shared<AssetBase> asset, const AssetHandle& id)
{
    TB_PROFILE_SCOPE_NAME("Tabby::AssetManager::RegisterAsset");

    asset->Handle = id;
    m_Mutex.lock();
    m_AssetRegistry.emplace(id, asset);
    m_Mutex.unlock();

    return id;
}

const Shared<Image> AssetManager::GetMissingTexture()
{

    if (!m_MissingTextureImage) [[unlikely]] {

        std::vector<RGBA32> image_data;
        image_data.reserve(4);
        image_data.push_back({ 255, 0, 220, 255 });
        image_data.push_back({ 1, 0, 1, 255 });
        image_data.push_back({ 1, 0, 1, 255 });
        image_data.push_back({ 255, 0, 220, 255 });

        std::vector<RGBA32> full_image_data = AssetCompressor::GenerateMipMaps(image_data, 2, 2);

        std::vector<byte> raw(full_image_data.size() * sizeof(RGBA32));
        memcpy(raw.data(), full_image_data.data(), full_image_data.size() * sizeof(RGBA32));
        full_image_data.clear();

        ImageSpecification texture_spec = {};
        texture_spec.pixels = std::move(raw);
        texture_spec.format = bgfx::TextureFormat::Enum::RGBA32U;
        texture_spec.type = ImageType::TYPE_2D;
        texture_spec.usage = ImageUsage::TEXTURE;
        texture_spec.extent = { 2, 2 };
        texture_spec.array_layers = 1;
        texture_spec.mip_levels = Utils::ComputeNumMipLevelsBC7(2, 2) + 1;
        texture_spec.path = "bin_missing_image";

        AssetHandle handle;
        m_MissingTextureImage = CreateShared<Image>(texture_spec, handle);

        AssetManager::RegisterAsset(m_MissingTextureImage, handle);
    }

    return m_MissingTextureImage;
}

AssetHandle AssetManager::ImportMeshSource(std::filesystem::path path, AssetHandle handle)
{
    TB_PROFILE_SCOPE_NAME("Tabby::AssetManager::ImportMeshSource");

    TB_CORE_ASSERT_TAGGED(false, "Not implemented");

    return 0;
}

AssetHandle AssetManager::ImportImageSource(std::filesystem::path path, AssetHandle handle)
{
    TB_PROFILE_SCOPE_NAME("Tabby::AssetManager::ImportImageSource");
    if (m_UUIDs.find(path.string()) != m_UUIDs.end())
        return m_UUIDs.at(path.string());

    std::vector<RGBA32> image_data;
    int image_width, image_height, channels;

    {
        TB_PROFILE_SCOPE_NAME("Tabby::AssetManager::ImportImageSource::Read");

        stbi_set_flip_vertically_on_load(true);
        std::vector<unsigned char> image_source;

        SDL_RWops* rw = SDL_RWFromFile(path.c_str(), "rb");
        if (rw != nullptr) {
            Sint64 size = SDL_RWsize(rw);

            if (size > 0) {
                image_source.resize(size);
                Sint64 bytesRead = SDL_RWread(rw, image_source.data(), 1, size);
                if (bytesRead != size) {
                    TB_CORE_ERROR("Error reading file {0}", path);
                    image_source.clear();
                    return m_MissingTextureImage->Handle;
                }
            }

            SDL_RWclose(rw);
        } else {
            TB_CORE_ERROR("Could not open file {0}", path);
            return m_MissingTextureImage->Handle;
        }

        RGBA32* raw_image_data = (RGBA32*)stbi_load_from_memory(image_source.data(), static_cast<int>(image_source.size()), &image_width, &image_height, &channels, 4);

        image_data.assign(raw_image_data, raw_image_data + (image_width * image_height));
        channels = 4;

        delete raw_image_data;

        if (image_data.empty()) {
            TB_CORE_ERROR("TextureImporter::ImportTextureSource - Could not load texture from filepath: {}", path.string());

            return m_MissingTextureImage->Handle;
        }
    }
    // Generate mip map
    std::vector<RGBA32> full_image_data = AssetCompressor::GenerateMipMaps(image_data, image_width, image_height);

    std::vector<byte> raw(full_image_data.size() * sizeof(RGBA32));
    memcpy(raw.data(), full_image_data.data(), full_image_data.size() * sizeof(RGBA32));
    full_image_data.clear();

    ImageSpecification texture_spec = {};
    texture_spec.pixels = std::move(raw);
    texture_spec.format = bgfx::TextureFormat::Enum::RGBA32U;
    texture_spec.type = ImageType::TYPE_2D;
    texture_spec.usage = ImageUsage::TEXTURE;
    texture_spec.extent = { (uint32_t)image_width, (uint32_t)image_height };
    texture_spec.array_layers = 1;
    texture_spec.mip_levels = Utils::ComputeNumMipLevelsBC7(image_width, image_height) + 1;
    texture_spec.path = path;

    Shared<Image> image = CreateShared<Image>(texture_spec, handle);

    m_AssetRegistry.emplace(image->Handle, image);
    m_UUIDs.emplace(path.string(), image->Handle);

    return image->Handle;
}

AssetHandle AssetManager::ImportFontSource(std::filesystem::path path, AssetHandle handle)
{
    TB_PROFILE_SCOPE_NAME("Tabby::AssetManager::ImportFontSource");

    TB_CORE_ASSERT_TAGGED(false, "Not implemented");

    return 0;

    // if (m_UUIDs.find(path.string()) != m_UUIDs.end())
    //     return m_UUIDs.at(path.string());
    //
    // Buffer data;
    //
    // {
    //     TB_PROFILE_SCOPE_NAME("AssetManager::ImportFontSource");
    //
    //     SDL_RWops* rw = SDL_RWFromFile(path.c_str(), "rb");
    //     if (rw != nullptr) {
    //         Sint64 size = SDL_RWsize(rw);
    //
    //         if (size > 0) {
    //             data.Allocate(size);
    //             data.Size = size;
    //
    //             Sint64 bytesRead = SDL_RWread(rw, data.Data, 1, size);
    //             if (bytesRead != size) {
    //                 TB_CORE_ERROR("Error reading file {0}", path);
    //                 data.Release();
    //             }
    //         }
    //
    //         SDL_RWclose(rw);
    //     } else {
    //         TB_CORE_ERROR("Could not open file {0}", path);
    //     }
    //
    //     if (data.Data == nullptr) {
    //         TB_CORE_ERROR("AssetManager::ImportFontSource - Could not load font from filepath: {}", path.string());
    //     }
    // }
    //
    // Shared<Font> font = CreateShared<Font>(path, handle, data);
    //
    // data.Release();
    //
    // m_AssetRegistry.emplace(font->Handle, font);
    // m_UUIDs.emplace(path.string(), font->Handle);
    //
    // return font->Handle;
}

AssetHandle AssetManager::ImportAudioSource(std::filesystem::path path, AssetHandle handle)
{
    TB_PROFILE_SCOPE_NAME("Tabby::AssetManager::ImportAudioSource");

    if (m_UUIDs.find(path.string()) != m_UUIDs.end())
        return m_UUIDs.at(path.string());

    Wav wavData;
    {
        SDL_RWops* rw = SDL_RWFromFile(path.c_str(), "rb");
        TB_CORE_ASSERT_TAGGED(rw, "Failed to open WAV file!");

        char riff_header[4];
        SDL_RWread(rw, riff_header, 1, 4);

        TB_CORE_ASSERT_TAGGED(std::strncmp(riff_header, "RIFF", 4) == 0, "Invalid WAV file: missing RIFF header.");

        SDL_RWseek(rw, 4, RW_SEEK_CUR);
        char wave_identifier[4];
        SDL_RWread(rw, wave_identifier, 1, 4);
        TB_CORE_ASSERT_TAGGED(std::strncmp(wave_identifier, "WAVE", 4) == 0, "Invalid WAV file: missing WAVE identifier.");

        bool fmt_found = false;
        bool data_found = false;

        while (!fmt_found || !data_found) {
            char chunk_id[4];
            SDL_RWread(rw, chunk_id, 1, 4);

            Uint32 chunk_size = 0;
            SDL_RWread(rw, &chunk_size, sizeof(Uint32), 1);

            uint16_t num_channels = 0;
            uint16_t bits_per_sample = 0;
            if (std::strncmp(chunk_id, "fmt ", 4) == 0) {
                fmt_found = true;

                uint16_t audio_format;
                uint32_t sample_rate;
                uint32_t byte_rate;
                uint16_t block_align;

                SDL_RWread(rw, &audio_format, sizeof(Uint16), 1);
                SDL_RWread(rw, &num_channels, sizeof(Uint16), 1);
                SDL_RWread(rw, &sample_rate, sizeof(Uint32), 1);
                SDL_RWread(rw, &byte_rate, sizeof(Uint32), 1);
                SDL_RWread(rw, &block_align, sizeof(Uint16), 1);
                SDL_RWread(rw, &bits_per_sample, sizeof(Uint16), 1);

                wavData.sampleRate = sample_rate;

                if (audio_format == 1) { // PCM format
                    if (num_channels == 1) {
                        if (bits_per_sample == 8) {
                            wavData.format = 0x1100; // AL_FORMAT_MONO8
                        } else if (bits_per_sample == 16) {
                            wavData.format = 0x1101; // AL_FORMAT_MONO16
                        }
                    } else if (num_channels == 2) {
                        if (bits_per_sample == 8) {
                            wavData.format = 0x1102; // AL_FORMAT_STEREO8
                        } else if (bits_per_sample == 16) {
                            wavData.format = 0x1103; // AL_FORMAT_STEREO16
                        }
                    }
                }

                if (chunk_size > 16) {
                    SDL_RWseek(rw, chunk_size - 16, RW_SEEK_CUR);
                }

            } else if (std::strncmp(chunk_id, "data", 4) == 0) {
                data_found = true;

                wavData.dataSize = chunk_size;
                wavData.dataStart = SDL_RWtell(rw);

                float num_samples = (float)wavData.dataSize / ((float)num_channels * bits_per_sample / 8);
                wavData.duration = num_samples / wavData.sampleRate;

                break;
            } else {
                SDL_RWseek(rw, chunk_size, RW_SEEK_CUR);
            }
        }
    }

    Shared<Audio> audio = CreateShared<Audio>(path.string(), handle, wavData);

    m_AssetRegistry.emplace(audio->Handle, audio);
    m_UUIDs.emplace(path.string(), audio->Handle);

    return audio->Handle;
}
}
