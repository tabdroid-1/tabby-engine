
#include "SoundBuffer.h"
#include <Tabby/Sound/SoundDevice.h>

namespace Tabby {
SoundBuffer::SoundBuffer(const std::string& filepath)
    : m_Path(filepath)
{
    ma_result result;
    // SoundDevice::GetConfig()
    for (SoundDevice::GetConfig()->iEngine = 0; SoundDevice::GetConfig()->iEngine < SoundDevice::GetConfig()->engineCount; SoundDevice::GetConfig()->iEngine += 1) {
        /* Just one sound per engine in this example. We're going to be loading this asynchronously. */
        result = ma_sound_init_from_file(&SoundDevice::GetConfig()->engines[SoundDevice::GetConfig()->iEngine], filepath.c_str(), MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_DECODE | MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_ASYNC | MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_STREAM, NULL, NULL, &SoundDevice::GetConfig()->sounds[SoundDevice::GetConfig()->iEngine]);
        if (result != MA_SUCCESS) {
            TB_CORE_WARN("Failed to load sound \"{0}\"", filepath);
            break;
        }

        /*
        The sound can be started as soon as ma_sound_init_from_file() returns, even for sounds that are initialized
        with MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_ASYNC. The sound will start playing while it's being loaded. Note that if the
        asynchronous loading process cannot keep up with the rate at which you try reading you'll end up glitching.
        If this is an issue, you need to not load sounds asynchronously.
        */
        result = ma_sound_start(&SoundDevice::GetConfig()->sounds[SoundDevice::GetConfig()->iEngine]);
        if (result != MA_SUCCESS) {
            TB_CORE_WARN("Failed to start sound.", filepath);
        }

        SoundDevice::GetConfig()->soundCount += 1;
    }
}
SoundBuffer::~SoundBuffer()
{
}

void SoundBuffer::UpdateBuffer()
{
}

void SoundBuffer::Play()
{
}
void SoundBuffer::Pause()
{
}

void SoundBuffer::Stop()
{
}

void SoundBuffer::SetGain(const float& val)
{
}

void SoundBuffer::Duration(float& val)
{
    return;
}
}
