#include <Tabby/Asset/AssetManager.h>
#include <Tabby/Renderer/Texture.h>
#include <Tabby/Asset/AssetFile.h>
#include <Tabby/Renderer/Font.h>
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

    Buffer data;
    int image_width, image_height, channels;

    {
        TB_PROFILE_SCOPE_NAME("Tabby::AssetManager::ImportImageSource::Read");
        stbi_set_flip_vertically_on_load(true);
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
        }

        data.Data = stbi_load_from_memory(imageData.data(), static_cast<int>(imageData.size()), &image_width, &image_height, &channels, 4);
        channels = 4;

        if (data.Data == nullptr) {
            TB_CORE_ERROR("TextureImporter::ImportTextureSource - Could not load texture from filepath: {}", path.string());
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
    texture_spec.Format = ImageFormat::RGBA8;
    texture_spec.Width = image_width;
    texture_spec.Height = image_height;
    texture_spec.type = ImageType::TYPE_2D;
    texture_spec.usage = ImageUsage::TEXTURE;
    texture_spec.array_layers = 1;
    texture_spec.path = path;
    texture_spec.UnpackAlignment = 4;
    texture_spec.GenerateMips = false;

    Shared<Texture> image = Texture::Create(texture_spec, handle, data);
    data.Release();

    m_AssetRegistry.emplace(image->Handle, image);
    m_UUIDs.emplace(path.string(), image->Handle);

    return image->Handle;
}

AssetHandle AssetManager::ImportFontSource(std::filesystem::path path, AssetHandle handle)
{
    TB_PROFILE_SCOPE_NAME("Tabby::AssetManager::ImportFontSource");

    if (m_UUIDs.find(path.string()) != m_UUIDs.end())
        return m_UUIDs.at(path.string());

    Buffer data;

    {
        TB_PROFILE_SCOPE_NAME("AssetManager::ImportFontSource");

        SDL_RWops* rw = SDL_RWFromFile(path.c_str(), "rb");
        if (rw != nullptr) {
            Sint64 size = SDL_RWsize(rw);

            if (size > 0) {
                data.Data = new uint8_t[size];
                data.Size = size;

                Sint64 bytesRead = SDL_RWread(rw, data.Data, 1, size);
                if (bytesRead != size) {
                    TB_CORE_ERROR("Error reading file {0}", path);
                    data.Release();
                }
            }

            SDL_RWclose(rw);
        } else {
            TB_CORE_ERROR("Could not open file {0}", path);
        }

        if (data.Data == nullptr) {
            TB_CORE_ERROR("AssetManager::ImportFontSource - Could not load font from filepath: {}", path.string());
        }
    }

    Shared<Font> font = CreateShared<Font>(path, handle, data);

    data.Release();

    m_AssetRegistry.emplace(font->Handle, font);
    m_UUIDs.emplace(path.string(), font->Handle);

    return font->Handle;
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

        // Read the RIFF header
        char riff_header[4];
        SDL_RWread(rw, riff_header, 1, 4);

        TB_CORE_ASSERT_TAGGED(std::strncmp(riff_header, "RIFF", 4) == 0, "Invalid WAV file: missing RIFF header.");

        // Skip file size and read WAVE identifier
        SDL_RWseek(rw, 4, RW_SEEK_CUR); // Skip file size
        char wave_identifier[4];
        SDL_RWread(rw, wave_identifier, 1, 4);
        TB_CORE_ASSERT_TAGGED(std::strncmp(wave_identifier, "WAVE", 4) == 0, "Invalid WAV file: missing WAVE identifier.");

        // Read chunks until we find the "fmt " and "data" chunks
        bool fmt_found = false;
        bool data_found = false;

        while (!fmt_found || !data_found) {
            char chunk_id[4];
            SDL_RWread(rw, chunk_id, 1, 4);

            Uint32 chunk_size = 0;
            SDL_RWread(rw, &chunk_size, sizeof(Uint32), 1);

            uint16_t num_channels;
            uint16_t bits_per_sample;
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

                // Determine the format
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

                // Skip any extra format bytes
                if (chunk_size > 16) {
                    SDL_RWseek(rw, chunk_size - 16, RW_SEEK_CUR);
                }

            } else if (std::strncmp(chunk_id, "data", 4) == 0) {
                data_found = true;

                wavData.dataSize = chunk_size;
                wavData.dataStart = SDL_RWtell(rw);

                float num_samples = (float)wavData.dataSize / ((float)num_channels * bits_per_sample / 8);
                wavData.duration = num_samples / wavData.sampleRate;

                // We found the data chunk, so no need to read further
                break;
            } else {
                // Skip this chunk
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
