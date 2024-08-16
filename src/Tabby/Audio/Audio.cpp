#include <Tabby/Audio/Audio.h>

#include "SDL_rwops.h"
#include <AL/al.h>

namespace Tabby {

Audio::Audio(const std::string& path, AssetHandle handle, const Wav& wavData)
{
    TB_PROFILE_SCOPE_NAME("Tabby::Audio::Constructor");
    Handle = handle;
    Type = AssetType::TABBY_AUDIO;

    sampleRate = wavData.sampleRate;
    format = wavData.format;
    dataStart = wavData.dataStart;
    dataSize = wavData.dataSize;
    duration = wavData.duration;
    m_RWops = SDL_RWFromFile(path.c_str(), "rb");
}

Audio::~Audio()
{
    TB_PROFILE_SCOPE_NAME("Tabby::Audio::Destructor");
    Destroy();
}

void Audio::Destroy()
{
    TB_PROFILE_SCOPE_NAME("Tabby::Audio::Destroy");
    SDL_RWclose(m_RWops);
}

}
