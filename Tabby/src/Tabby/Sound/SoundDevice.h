#pragma once

#include "miniaudio/miniaudio.h"

namespace Tabby {

struct SoundDeviceConfig {

    ma_uint32 iEngine;
    ma_sound sounds[MAX_SOUNDS];
    ma_uint32 engineCount;
    ma_engine engines[MAX_DEVICES];
    ma_device devices[MAX_DEVICES];
    ma_uint32 soundCount;
    ma_context context;
    ma_resource_manager_config resourceManagerConfig;
    ma_resource_manager resourceManager;
    ma_device_info* pPlaybackDeviceInfos;
    ma_uint32 playbackDeviceCount;
    ma_uint32 iAvailableDevice;
    ma_uint32 iChosenDevice;
    ma_uint32 iSound;
};

class SoundDevice {
public:
    static void Init();
    static SoundDevice* Get();
    static SoundDeviceConfig* GetConfig() { return m_Config; }

private:
    static void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
    {
        (void)pInput;

        ma_engine_read_pcm_frames((ma_engine*)pDevice->pUserData, pOutput, frameCount, NULL);
    }
    SoundDevice();
    ~SoundDevice();

    static SoundDevice* s_Instance;
    static SoundDeviceConfig* m_Config;
};
}
