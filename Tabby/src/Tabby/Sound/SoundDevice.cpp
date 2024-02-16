
#include "SoundDevice.h"
#include <Tabby/Core/Assert.h>
#include <Tabby/Core/Log.h>

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio/miniaudio.h"

namespace Tabby {
SoundDevice* SoundDevice::s_Instance = nullptr;
SoundDeviceConfig* SoundDevice::m_Config = nullptr;

SoundDevice::SoundDevice()
{
    m_Config = new SoundDeviceConfig;
    ma_result m_Result;

    m_Config->resourceManagerConfig = ma_resource_manager_config_init();
    m_Config->resourceManagerConfig.decodedFormat = ma_format_f32; /* ma_format_f32 should almost always be used as that's what the engine (and most everything else) uses for mixing. */
    m_Config->resourceManagerConfig.decodedChannels = 0; /* Setting the channel count to 0 will cause sounds to use their native channel count. */
    m_Config->resourceManagerConfig.decodedSampleRate = 48000; /* Using a consistent sample rate is useful for avoiding expensive resampling in the audio thread. This will result in resampling being performed by the loading thread(s). */

    m_Result = ma_resource_manager_init(&m_Config->resourceManagerConfig, &m_Config->resourceManager);
    TB_CORE_ASSERT(m_Result == MA_SUCCESS, "Failed to initialize resource manager.")

    /* We're going to want a context so we can enumerate our playback devices. */
    m_Result = ma_context_init(NULL, 0, NULL, &m_Config->context);
    TB_CORE_ASSERT(m_Result == MA_SUCCESS, "Failed to initialize context.")

    m_Result = ma_context_get_devices(&m_Config->context, &m_Config->pPlaybackDeviceInfos, &m_Config->playbackDeviceCount, NULL, NULL);
    TB_CORE_ASSERT(m_Result == MA_SUCCESS, "Failed to enumerate playback devices.")

    m_Config->engineCount = 0;

    for (m_Config->iChosenDevice = 0; m_Config->iChosenDevice < MAX_DEVICES; m_Config->iChosenDevice++) {
        for (;;) {

            TB_CORE_INFO("Available devices:");
            for (m_Config->iAvailableDevice = 0; m_Config->iAvailableDevice < m_Config->playbackDeviceCount; m_Config->iAvailableDevice += 1) {
                TB_CORE_INFO("  {0}: {1}", m_Config->iAvailableDevice, m_Config->pPlaybackDeviceInfos[m_Config->iAvailableDevice].name);
            }

            ma_device_config deviceConfig;
            ma_engine_config engineConfig;

            deviceConfig = ma_device_config_init(ma_device_type_playback);
            deviceConfig.playback.pDeviceID = &m_Config->pPlaybackDeviceInfos[m_Config->iChosenDevice].id;
            deviceConfig.playback.format = m_Config->resourceManager.config.decodedFormat;
            deviceConfig.playback.channels = 0;
            deviceConfig.sampleRate = m_Config->resourceManager.config.decodedSampleRate;
            deviceConfig.dataCallback = data_callback;
            deviceConfig.pUserData = &m_Config->engines[m_Config->engineCount];

            m_Result = ma_device_init(&m_Config->context, &deviceConfig, &m_Config->devices[m_Config->engineCount]);
            TB_CORE_ASSERT(m_Result == MA_SUCCESS, "Failed to initialize device for {0}.", m_Config->pPlaybackDeviceInfos[0].name);

            /* Now that we have the device we can initialize the engine. The device is passed into the engine's config. */
            engineConfig = ma_engine_config_init();
            engineConfig.pDevice = &m_Config->devices[m_Config->engineCount];
            engineConfig.pResourceManager = &m_Config->resourceManager;
            engineConfig.noAutoStart = MA_TRUE; /* Don't start the engine by default - we'll do that manually below. */

            m_Result = ma_engine_init(&engineConfig, &m_Config->engines[m_Config->engineCount]);
            TB_CORE_ASSERT(m_Result == MA_SUCCESS, "Failed to initialize engine for {0}.", m_Config->pPlaybackDeviceInfos[0].name);

            m_Config->engineCount += 1;
            break;
        }

        TB_CORE_INFO("Device {0}: {1}", m_Config->iChosenDevice + 1, m_Config->pPlaybackDeviceInfos[m_Config->iChosenDevice].name);
    }

    /* We should now have our engine's initialized. We can now start them. */
    for (m_Config->iEngine = 0; m_Config->iEngine < m_Config->engineCount; m_Config->iEngine += 1) {
        m_Result = ma_engine_start(&m_Config->engines[m_Config->iEngine]);
        if (m_Result != MA_SUCCESS) {
            TB_CORE_WARN("Failed to start engine {0}.", m_Config->iEngine);
        }
    }
}
SoundDevice::~SoundDevice()
{
    /* The application owns the `ma_sound` object which means you're responsible for uninitializing them. */
    for (m_Config->iSound = 0; m_Config->iSound < m_Config->soundCount; m_Config->iSound += 1) {
        ma_sound_uninit(&m_Config->sounds[m_Config->iSound]);
    }

    /* We can now uninitialize each engine. */
    for (m_Config->iEngine = 0; m_Config->iEngine < m_Config->engineCount; m_Config->iEngine += 1) {
        ma_engine_uninit(&m_Config->engines[m_Config->iEngine]);

        /*
        The engine has been uninitialized so now lets uninitialize the device. Do this first to ensure we don't
        uninitialize the resource manager from under the device while the data callback is running.
        */
        ma_device_uninit(&m_Config->devices[m_Config->iEngine]);
    }

    /* The context can only be uninitialized after the devices. */
    ma_context_uninit(&m_Config->context);

    /*
    Do the resource manager last. This way we can guarantee the data callbacks of each device aren't trying to access
    and data managed by the resource manager.
    */
    ma_resource_manager_uninit(&m_Config->resourceManager);
}

SoundDevice* SoundDevice::Get()
{
    // Init();
    return s_Instance;
}

void SoundDevice::Init()
{
    if (s_Instance == nullptr)
        s_Instance = new SoundDevice();
}

}
