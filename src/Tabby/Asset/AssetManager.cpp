#include "SDL_rwops.h"
#include <Tabby/Asset/AssetFile.h>
#include <Tabby/Asset/AssetManager.h>
#include <Tabby/Renderer/Font.h>
#include <Tabby/Renderer/Texture.h>
#include <Tabby/Audio/Audio.h>

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
    asset->Handle = id;
    m_Mutex.lock();
    m_AssetRegistry.emplace(id, asset);
    m_Mutex.unlock();

    return id;
}

AssetHandle AssetManager::ImportMeshSource(std::filesystem::path path, AssetHandle handle)
{
    TB_CORE_ASSERT_TAGGED(false, "Not implemented");

    return 0;
}

AssetHandle AssetManager::ImportImageSource(std::filesystem::path path, AssetHandle handle)
{
    if (m_UUIDs.find(path.string()) != m_UUIDs.end())
        return m_UUIDs.at(path.string());

    Buffer data;
    int image_width, image_height, channels;

    {

        stbi_set_flip_vertically_on_load(true);
        TB_PROFILE_SCOPE_NAME("stbi_load - AssetManager::ImportTextureSource");
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
    texture_spec.GenerateMips = false;

    Shared<Texture> image = Texture::Create(texture_spec, handle, data);
    data.Release();

    m_AssetRegistry.emplace(image->Handle, image);
    m_UUIDs.emplace(path.string(), image->Handle);

    return image->Handle;
}

AssetHandle AssetManager::ImportFontSource(std::filesystem::path path, AssetHandle handle)
{

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

    if (m_UUIDs.find(path.string()) != m_UUIDs.end())
        return m_UUIDs.at(path.string());

    Wav wavData;
    {
        SDL_RWops* rw = SDL_RWFromFile(path.c_str(), "rb");
        if (!rw) {
            throw std::runtime_error("Failed to open WAV file");
        }

        // static const auto buffer_to_number = [](Uint8* buffer, std::size_t len) -> std::int32_t {
        //     if (len > 4)
        //         throw std::logic_error("audio::engine::buffer_to_number: Buffer can only contain up to 4 bytes");
        //
        //     int num = 1;
        //     if (*(char*)&num != 1) // big endian
        //         std::reverse(buffer, buffer + len);
        //
        //     std::int32_t res = 0;
        //     std::memcpy(&res, buffer, len);
        //
        //     return res;
        // };
        //
        // std::array<uint8_t, 4> wav_header_buffer;
        //
        // if (SDL_RWread(rw, wav_header_buffer.data(), 1, 4) != 4)
        //     throw std::runtime_error("audio::engine::load_wav: Could not read RIFF");
        //
        // if (std::strncmp(reinterpret_cast<char*>(wav_header_buffer.data()), "RIFF", 4) != 0)
        //     throw std::runtime_error("audio::engine::load_wav: wav header does not contain RIFF");
        //
        // if (SDL_RWread(rw, wav_header_buffer.data(), 1, 4) != 4)
        //     throw std::runtime_error("audio::engine::load_wav: Could not read size of wav file");
        //
        // if (SDL_RWread(rw, wav_header_buffer.data(), 1, 4) != 4)
        //     throw std::runtime_error("audio::engine::load_wav: Could not read WAVE");
        //
        // if (std::strncmp(reinterpret_cast<char*>(wav_header_buffer.data()), "WAVE", 4) != 0)
        //     throw std::runtime_error("audio::engine::load_wav: wav header does not contain WAVE");
        //
        // int num_channels = 0;
        // int sample_rate = 0;
        // int bits_per_sample = 0;
        // ALenum format = AL_NONE;
        //
        // while (true) {
        //     if (SDL_RWread(rw, wav_header_buffer.data(), 1, 4) != 4)
        //         throw std::runtime_error("audio::engine::load_wav: Could not read chunk ID");
        //
        //     if (std::strncmp(reinterpret_cast<char*>(wav_header_buffer.data()), "fmt ", 4) == 0) {
        //         if (SDL_RWread(rw, wav_header_buffer.data(), 1, 4) != 4)
        //             throw std::runtime_error("audio::engine::load_wav: Could not read chunk size");
        //
        //         if (SDL_RWread(rw, wav_header_buffer.data(), 1, 2) != 2)
        //             throw std::runtime_error("audio::engine::load_wav: Could not read audio format");
        //
        //         if (SDL_RWread(rw, wav_header_buffer.data(), 1, 2) != 2)
        //             throw std::runtime_error("audio::engine::load_wav: Could not read number of channels");
        //
        //         num_channels = buffer_to_number(wav_header_buffer.data(), 2);
        //
        //         if (SDL_RWread(rw, wav_header_buffer.data(), 1, 4) != 4)
        //             throw std::runtime_error("audio::engine::load_wav: Could not read sample rate");
        //
        //         sample_rate = buffer_to_number(wav_header_buffer.data(), 4);
        //
        //         if (SDL_RWread(rw, wav_header_buffer.data(), 1, 4) != 4)
        //             throw std::runtime_error("audio::engine::load_wav: Could not read byte rate");
        //
        //         if (SDL_RWread(rw, wav_header_buffer.data(), 1, 2) != 2)
        //             throw std::runtime_error("audio::engine::load_wav: Could not read block align");
        //
        //         if (SDL_RWread(rw, wav_header_buffer.data(), 1, 2) != 2)
        //             throw std::runtime_error("audio::engine::load_wav: Could not read bits per sample");
        //
        //         bits_per_sample = buffer_to_number(wav_header_buffer.data(), 2);
        //
        //         if (num_channels == 1 && bits_per_sample == 8)
        //             format = AL_FORMAT_MONO8;
        //         else if (num_channels == 1 && bits_per_sample == 16)
        //             format = AL_FORMAT_MONO16;
        //         else if (num_channels == 2 && bits_per_sample == 8)
        //             format = AL_FORMAT_STEREO8;
        //         else if (num_channels == 2 && bits_per_sample == 16)
        //             format = AL_FORMAT_STEREO16;
        //         else
        //             throw std::runtime_error("audio::engine::load_wav: Invalid format");
        //
        //     } else if (std::strncmp(reinterpret_cast<char*>(wav_header_buffer.data()), "data", 4) == 0) {
        //         if (SDL_RWread(rw, wav_header_buffer.data(), 1, 4) != 4)
        //             throw std::runtime_error("audio::engine::load_wav: Could not read data size");
        //
        //         std::size_t data_start = SDL_RWtell(rw);
        //         std::size_t data_size = buffer_to_number(wav_header_buffer.data(), 4);
        //
        //         float num_samples = (float)data_size / ((float)num_channels * bits_per_sample / 8);
        //         float duration = num_samples / sample_rate;
        //
        //         // return wav { sample_rate, format, data_start, data_size, duration };
        //         wavData.sampleRate = sample_rate;
        //         wavData.duration = duration;
        //         wavData.dataSize = data_size;
        //         wavData.dataStart = data_start;
        //         wavData.format = sample_rate;
        //
        //     } else {
        //         if (SDL_RWread(rw, wav_header_buffer.data(), 1, 4) != 4)
        //             throw std::runtime_error("audio::engine::load_wav: Could not read chunk size");
        //
        //         Uint32 list_chunk_size = buffer_to_number(wav_header_buffer.data(), 4);
        //         SDL_RWseek(rw, list_chunk_size, RW_SEEK_CUR);
        //     }
        // }

        // static const auto buffer_to_number = [](Uint8* buffer, std::size_t len) -> std::int32_t {
        //     if (len > 4)
        //         throw std::logic_error("audio::engine::buffer_to_number: Buffer can only contain up to 4 bytes");
        //
        //     int num = 1;
        //     if (*(char*)&num != 1) // big endian
        //         std::reverse(buffer, buffer + len);
        //
        //     std::int32_t res = 0;
        //     std::memcpy(&res, buffer, len);
        //
        //     return res;
        // };
        //
        // std::array<Uint8, 4> wav_header_buffer;
        //
        // // Read and validate "RIFF" chunk
        // if (SDL_RWread(rw, wav_header_buffer.data(), 1, 4) != 4)
        //     throw std::runtime_error("audio::engine::load_wav: Could not read 'RIFF' chunk ID");
        //
        // if (std::strncmp(reinterpret_cast<char*>(wav_header_buffer.data()), "RIFF", 4) != 0)
        //     throw std::runtime_error("audio::engine::load_wav: WAV header does not contain 'RIFF'");
        //
        // // Read and ignore file size (not used here)
        // if (SDL_RWread(rw, wav_header_buffer.data(), 1, 4) != 4)
        //     throw std::runtime_error("audio::engine::load_wav: Could not read size of WAV file");
        //
        // // Read and validate "WAVE" format
        // if (SDL_RWread(rw, wav_header_buffer.data(), 1, 4) != 4)
        //     throw std::runtime_error("audio::engine::load_wav: Could not read 'WAVE' format");
        //
        // if (std::strncmp(reinterpret_cast<char*>(wav_header_buffer.data()), "WAVE", 4) != 0)
        //     throw std::runtime_error("audio::engine::load_wav: WAV header does not contain 'WAVE'");
        //
        // int num_channels = 0;
        // int sample_rate = 0;
        // int bits_per_sample = 0;
        // ALenum format = AL_NONE;
        //
        // while (true) {
        //     if (SDL_RWread(rw, wav_header_buffer.data(), 1, 4) != 4)
        //         throw std::runtime_error("audio::engine::load_wav: Could not read chunk ID");
        //
        //     std::string chunk_id(reinterpret_cast<char*>(wav_header_buffer.data()), 4);
        //
        //     if (chunk_id == "fmt ") {
        //         // Read and validate 'fmt ' chunk
        //         if (SDL_RWread(rw, wav_header_buffer.data(), 1, 4) != 4)
        //             throw std::runtime_error("audio::engine::load_wav: Could not read 'fmt ' chunk size");
        //
        //         if (SDL_RWread(rw, wav_header_buffer.data(), 1, 2) != 2)
        //             throw std::runtime_error("audio::engine::load_wav: Could not read audio format");
        //
        //         if (SDL_RWread(rw, wav_header_buffer.data(), 1, 2) != 2)
        //             throw std::runtime_error("audio::engine::load_wav: Could not read number of channels");
        //
        //         num_channels = buffer_to_number(wav_header_buffer.data(), 2);
        //
        //         if (SDL_RWread(rw, wav_header_buffer.data(), 1, 4) != 4)
        //             throw std::runtime_error("audio::engine::load_wav: Could not read sample rate");
        //
        //         sample_rate = buffer_to_number(wav_header_buffer.data(), 4);
        //
        //         if (SDL_RWread(rw, wav_header_buffer.data(), 1, 4) != 4)
        //             throw std::runtime_error("audio::engine::load_wav: Could not read byte rate");
        //
        //         if (SDL_RWread(rw, wav_header_buffer.data(), 1, 2) != 2)
        //             throw std::runtime_error("audio::engine::load_wav: Could not read block align");
        //
        //         if (SDL_RWread(rw, wav_header_buffer.data(), 1, 2) != 2)
        //             throw std::runtime_error("audio::engine::load_wav: Could not read bits per sample");
        //
        //         bits_per_sample = buffer_to_number(wav_header_buffer.data(), 2);
        //
        //         // Determine format based on channels and bits per sample
        //         if (num_channels == 1 && bits_per_sample == 8)
        //             format = AL_FORMAT_MONO8;
        //         else if (num_channels == 1 && bits_per_sample == 16)
        //             format = AL_FORMAT_MONO16;
        //         else if (num_channels == 2 && bits_per_sample == 8)
        //             format = AL_FORMAT_STEREO8;
        //         else if (num_channels == 2 && bits_per_sample == 16)
        //             format = AL_FORMAT_STEREO16;
        //         else
        //             throw std::runtime_error("audio::engine::load_wav: Invalid WAV format");
        //
        //     } else if (chunk_id == "data") {
        //         // Read and process 'data' chunk
        //         if (SDL_RWread(rw, wav_header_buffer.data(), 1, 4) != 4)
        //             throw std::runtime_error("audio::engine::load_wav: Could not read data chunk size");
        //
        //         std::size_t data_start = SDL_RWtell(rw);
        //         std::size_t data_size = buffer_to_number(wav_header_buffer.data(), 4);
        //
        //         float num_samples = (float)data_size / ((float)num_channels * bits_per_sample / 8);
        //         float duration = num_samples / sample_rate;
        //
        //         wavData.sampleRate = sample_rate;
        //         wavData.duration = duration;
        //         wavData.dataSize = data_size;
        //         wavData.dataStart = data_start;
        //         wavData.format = sample_rate;
        //     } else {
        //         // Skip unknown chunk
        //         if (SDL_RWread(rw, wav_header_buffer.data(), 1, 4) != 4)
        //             throw std::runtime_error("audio::engine::load_wav: Could not read chunk size");
        //
        //         Uint32 list_chunk_size = buffer_to_number(wav_header_buffer.data(), 4);
        //         SDL_RWseek(rw, list_chunk_size, RW_SEEK_CUR);
        //     }
        // }
        //
        // throw std::runtime_error("audio::engine::load_wav: WAV file stream went bad or could not find data chunk");

        if (!rw) {
            std::cerr << "Could not open file: " << SDL_GetError() << std::endl;
        }

        // Read the RIFF header
        char riff_header[4];
        SDL_RWread(rw, riff_header, 1, 4);
        if (std::strncmp(riff_header, "RIFF", 4) != 0) {
            std::cerr << "Invalid WAV file: missing RIFF header." << std::endl;
            SDL_RWclose(rw);
        }

        // Skip file size and read WAVE identifier
        SDL_RWseek(rw, 4, RW_SEEK_CUR); // Skip file size
        char wave_identifier[4];
        SDL_RWread(rw, wave_identifier, 1, 4);
        if (std::strncmp(wave_identifier, "WAVE", 4) != 0) {
            std::cerr << "Invalid WAV file: missing WAVE identifier." << std::endl;
            SDL_RWclose(rw);
        }

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
