#pragma once

#include <AL/al.h>
#include <AL/alc.h>

namespace Tabby {

typedef enum AudioSampleType {
    Int16,
    Float,
    IMA4,
    MSADPCM
} AudioSampleType;

class Audio;
class AudioSource;

class AudioEngine {
public:
    static void Init();
    static void Shutdown();

    static float GetVolume();
    static void SetVolume(float newVolume);

    static Vector3 GetPosition();
    static Vector3 GetVelocity();
    static float* GetOrientation();
    static void SetPosition(const Vector3& position);
    static void SetVelocity(const Vector3& velocity);
    static void SetOrientation(const Vector3& look, const Vector3& up);

    static AudioSource* CreateAudioSource(AssetHandle handle = 0);

private:
    AudioEngine();
    ~AudioEngine();

    static void FetchALErrors(const std::filesystem::path& file, int line);
    static void FetchALCErrors(ALCdevice* device, const std::filesystem::path& file, int line);

    void EnginePollingThread();

private:
    inline static AudioEngine* s_Instance;

    inline static ALCdevice* m_AlcDevice;
    inline static ALCcontext* m_AlcContext;

    inline static std::atomic_bool m_ShouldThreadClose;
    inline static std::thread m_PollingThread;

    inline static std::vector<AudioSource*> m_MusicMixer;
    inline static std::mutex m_MusicMixerLock;

    friend class AudioSource;
};

} // namespace Tabby
