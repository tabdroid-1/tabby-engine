#pragma once

#include <Tabby/Asset/AssetBase.h>
#include <AL/al.h>

struct SDL_RWops;

namespace Tabby {

struct Wav {
    int sampleRate;
    ALenum format;
    std::size_t dataStart;
    std::size_t dataSize;
    float duration;
};

class Audio : public AssetBase {
public:
    Audio(const std::string& path, AssetHandle handle, const Wav& wavData);
    ~Audio();

    void Destroy() override;

private:
    int sampleRate;
    ALenum format;
    std::size_t dataStart;
    std::size_t dataSize;
    float duration;
    SDL_RWops* m_RWops;

private:
    friend class AudioEngine;
    friend class AudioSource;
};

}
