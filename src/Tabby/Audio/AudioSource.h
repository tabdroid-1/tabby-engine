#pragma once

#include <Tabby/Audio/Audio.h>

namespace Tabby {

class AudioSource {
public:
    AudioSource();
    ~AudioSource();

    void UpdateBuffer();
    void UpdatePlayer();

    void SetAudio(AssetHandle audioHandle);
    void UnsetAudio();

    void Play();
    void Pause();
    void TogglePlay();
    bool IsPlaying();

    float GetPitch();
    float GetGain();
    void SetPitch(float pitch);
    void SetGain(float gain);

    float GetRolloffFactor();
    float GetMaxDistance();
    float GetReferenceDistance();
    void SetRolloffFactor(float rate);
    void SetMaxDistance(float distance);
    void SetReferenceDistance(float distance);

    bool GetRelative();
    bool GetLooping();
    float GetMinGain();
    float GetMaxGain();
    void SetRelative(bool relative);
    void SetLooping(bool looping);
    void SetMinGain(float gain);
    void SetMaxGain(float gain);

    Vector3 GetPosition();
    Vector3 GetVelocity();
    Vector3 GetDirection();
    void SetPosition(const Vector3& position);
    void SetVelocity(const Vector3& velocity);
    void SetDirection(const Vector3& direction);

    float GetConeOuterGain();
    float GetConeInnerAngle();
    float GetConeOuterAngle();
    void SetConeOuterGain(float gain);
    void SetConeInnerAngle(float angle);
    void SetConeOuterAngle(float angle);

private:
    Shared<Audio> m_Music;
    std::array<ALuint, 4> m_BufferIDs;
    ALuint m_SourceID;
    std::size_t m_Cursor;
    bool m_Loop;
    bool m_Playing;

    static constexpr int NUM_BUFFERS = 4;
    static constexpr std::size_t MUSIC_BUFFER_SIZE = 65536;
};

}
