#include <Tabby/Audio/AudioEngine.h>
#include <Tabby/Audio/AudioSource.h>
#include <Tabby/Asset/AssetManager.h>
#include <Tabby/Asset/AssetType.h>
#include "SDL_rwops.h"
#include <AL/al.h>
#include <AL/alext.h>

#define CHECK_AL_ERRORS() \
    AudioEngine::FetchALErrors(__FILE__, __LINE__)

namespace Tabby {

AudioSource::AudioSource()
    : m_Music(nullptr)
    , m_Playing(false)
{
    alGenSources(1, &m_SourceID);
    CHECK_AL_ERRORS();
    alSourcef(m_SourceID, AL_PITCH, 1);
    CHECK_AL_ERRORS();
    alSourcef(m_SourceID, AL_GAIN, 1.0f);
    CHECK_AL_ERRORS();
    alSource3f(m_SourceID, AL_POSITION, 0.0f, 0.0f, 0.0f);
    CHECK_AL_ERRORS();
    alSourcei(m_SourceID, AL_LOOPING, AL_FALSE);
    CHECK_AL_ERRORS();

    alGenBuffers(4, m_BufferIDs.data());
    CHECK_AL_ERRORS();
}

AudioSource::~AudioSource()
{
    alSourceStop(m_SourceID);
    CHECK_AL_ERRORS();
    alSourcei(m_SourceID, AL_BUFFER, NULL);
    CHECK_AL_ERRORS();
    alDeleteSources(1, &m_SourceID);
    alDeleteBuffers(NUM_BUFFERS, m_BufferIDs.data());
    CHECK_AL_ERRORS();
}

void AudioSource::Play()
{
    AudioEngine::m_MusicMixerLock.lock();
    if (!m_Music) {
        TB_CORE_ERROR("AudioSource's music is null/not set!");
        return;
    }

    m_Playing = true;
    alSourcePlay(m_SourceID);
    CHECK_AL_ERRORS();
    AudioEngine::m_MusicMixerLock.unlock();
}

void AudioSource::Pause()
{
    AudioEngine::m_MusicMixerLock.lock();
    if (!m_Music) {
        TB_CORE_ERROR("AudioSource's music is null/not set!");
        return;
    }

    m_Playing = false;
    alSourcePause(m_SourceID);
    CHECK_AL_ERRORS();
    AudioEngine::m_MusicMixerLock.unlock();
}

void AudioSource::TogglePlay()
{
    if (m_Playing)
        Pause();
    else
        Play();
}

bool AudioSource::IsPlaying()
{
    return m_Playing;
}

void AudioSource::SetAudio(AssetHandle audioHandle)
{
    AudioEngine::m_MusicMixerLock.lock();

    Shared<Audio> audio = AssetManager::GetAsset<Audio>(audioHandle);

    if (audio->Type != AssetType::TABBY_AUDIO) {
        TB_CORE_ERROR("Asset is not audio!");
        AudioEngine::m_MusicMixerLock.unlock();
        return;
    }

    m_Cursor = 0;
    m_Music = audio;
    UpdateBuffer();

    AudioEngine::m_MusicMixerLock.unlock();
}

void AudioSource::UnsetAudio()
{
    AudioEngine::m_MusicMixerLock.lock();
    alSourcei(m_SourceID, AL_BUFFER, NULL);
    CHECK_AL_ERRORS();
    m_Cursor = 0;

    AudioEngine::m_MusicMixerLock.unlock();
}

void AudioSource::UpdateBuffer()
{
    alSourceStop(m_SourceID);
    CHECK_AL_ERRORS();
    alSourcei(m_SourceID, AL_BUFFER, NULL);
    CHECK_AL_ERRORS();
    ALint buffer_count = 0;
    while (buffer_count < m_BufferIDs.size()) {
        ALsizei buffer_size = static_cast<ALsizei>(std::min(MUSIC_BUFFER_SIZE, m_Music->dataSize - m_Cursor));
        buffer_size -= buffer_size % 8;
        if (buffer_size == 0)
            break;

        std::vector<byte> buffer_data(buffer_size);
        SDL_RWseek(m_Music->m_RWops, m_Music->dataStart + m_Cursor, RW_SEEK_SET);
        TB_CORE_ASSERT_TAGGED(SDL_RWread(m_Music->m_RWops, buffer_data.data(), 1, buffer_size) == buffer_size, "Failed to read WAV data using SDL_RWops");
        alBufferData(m_BufferIDs[buffer_count], m_Music->format, buffer_data.data(), buffer_size, m_Music->sampleRate);
        CHECK_AL_ERRORS();
        buffer_count++;

        m_Cursor += buffer_size;
        if (buffer_size < MUSIC_BUFFER_SIZE) {
            m_Cursor = m_Music->dataSize;
            break;
        }
    }
    alSourceQueueBuffers(m_SourceID, buffer_count, m_BufferIDs.data());
    CHECK_AL_ERRORS();
}

void AudioSource::UpdatePlayer()
{
    if (!m_Music)
        return;

    ALint source_state = AL_NONE;
    alGetSourcei(m_SourceID, AL_SOURCE_STATE, &source_state);
    CHECK_AL_ERRORS();

    if (source_state != AL_PLAYING)
        return;

    ALint buffers_processed;
    alGetSourcei(m_SourceID, AL_BUFFERS_PROCESSED, &buffers_processed);
    CHECK_AL_ERRORS();
    while (buffers_processed > 0) {
        buffers_processed--;

        ALuint buffer;
        alSourceUnqueueBuffers(m_SourceID, 1, &buffer);
        CHECK_AL_ERRORS();

        if (m_Cursor >= m_Music->dataSize)
            continue;

        ALsizei buffer_size = static_cast<ALsizei>(std::min(MUSIC_BUFFER_SIZE, m_Music->dataSize - m_Cursor));
        buffer_size -= buffer_size % 8;

        std::vector<byte> buffer_data(buffer_size);
        SDL_RWseek(m_Music->m_RWops, m_Music->dataStart + m_Cursor, RW_SEEK_SET);
        TB_CORE_ASSERT_TAGGED(SDL_RWread(m_Music->m_RWops, buffer_data.data(), 1, buffer_size) == buffer_size, "Failed to read WAV data using SDL_RWops");

        alBufferData(buffer, m_Music->format, buffer_data.data(), buffer_size, m_Music->sampleRate);
        CHECK_AL_ERRORS();
        alSourceQueueBuffers(m_SourceID, 1, &buffer);
        CHECK_AL_ERRORS();

        m_Cursor += buffer_size;
        if (buffer_size < MUSIC_BUFFER_SIZE && !m_Loop)
            m_Cursor = m_Music->dataSize;
        else if (buffer_size < MUSIC_BUFFER_SIZE && m_Loop) // Using openal looping, same makes buffer loop. Which is not what we want
            m_Cursor = 0;
    }
}

float AudioSource::GetPitch()
{
    float pitch;
    alGetSourcef(m_SourceID, AL_PITCH, &pitch);
    return pitch;
}
float AudioSource::GetGain()
{
    float gain;
    alGetSourcef(m_SourceID, AL_GAIN, &gain);
    return gain;
}

void AudioSource::SetPitch(float pitch)
{
    alSourcef(m_SourceID, AL_PITCH, pitch);
    CHECK_AL_ERRORS();
}
void AudioSource::SetGain(float gain)
{
    alSourcef(m_SourceID, AL_GAIN, gain);
    CHECK_AL_ERRORS();
}

void AudioSource::SetMinGain(float gain)
{
    alSourcef(m_SourceID, AL_MIN_GAIN, gain);
    CHECK_AL_ERRORS();
}
void AudioSource::SetMaxGain(float gain)
{
    alSourcef(m_SourceID, AL_MAX_GAIN, gain);
    CHECK_AL_ERRORS();
}

float AudioSource::GetRolloffFactor()
{
    float factor;
    alGetSourcef(m_SourceID, AL_ROLLOFF_FACTOR, &factor);
    return factor;
}
float AudioSource::GetMaxDistance()
{
    float distance;
    alGetSourcef(m_SourceID, AL_MAX_DISTANCE, &distance);
    return distance;
}
float AudioSource::GetReferenceDistance()
{
    float distance;
    alGetSourcef(m_SourceID, AL_REFERENCE_DISTANCE, &distance);
    return distance;
}

void AudioSource::SetRolloffFactor(float rate)
{
    alSourcef(m_SourceID, AL_ROLLOFF_FACTOR, rate);
    CHECK_AL_ERRORS();
}
void AudioSource::SetMaxDistance(float distance)
{
    alSourcef(m_SourceID, AL_MAX_DISTANCE, distance);
    CHECK_AL_ERRORS();
}
void AudioSource::SetReferenceDistance(float distance)
{
    alSourcef(m_SourceID, AL_REFERENCE_DISTANCE, distance);
    CHECK_AL_ERRORS();
}

bool AudioSource::GetRelative()
{
    int relative;
    alGetSourcei(m_SourceID, AL_SOURCE_RELATIVE, &relative);

    if (relative == AL_TRUE)
        return true;
    else
        return false;
}
bool AudioSource::GetLooping()
{
    return m_Loop;
}

float AudioSource::GetMinGain()
{
    float gain;
    alGetSourcef(m_SourceID, AL_MIN_GAIN, &gain);
    return gain;
}
float AudioSource::GetMaxGain()
{
    float gain;
    alGetSourcef(m_SourceID, AL_MAX_GAIN, &gain);
    return gain;
}

void AudioSource::SetRelative(bool relative)
{
    if (relative) {
        alSourcei(m_SourceID, AL_SOURCE_RELATIVE, AL_TRUE);
        CHECK_AL_ERRORS();
    } else {
        alSourcei(m_SourceID, AL_SOURCE_RELATIVE, AL_FALSE);
        CHECK_AL_ERRORS();
    }
}
void AudioSource::SetLooping(bool looping)
{
    m_Loop = looping;
}

Vector3 AudioSource::GetPosition()
{
    float pos[3];
    alGetSourcefv(m_SourceID, AL_POSITION, pos);
    CHECK_AL_ERRORS();

    return { pos[0], pos[1], pos[2] };
}
Vector3 AudioSource::GetVelocity()
{
    float vel[3];
    alGetSourcefv(m_SourceID, AL_VELOCITY, vel);
    CHECK_AL_ERRORS();

    return { vel[0], vel[1], vel[2] };
}
Vector3 AudioSource::GetDirection()
{
    float dir[3];
    alGetSourcefv(m_SourceID, AL_DIRECTION, dir);
    CHECK_AL_ERRORS();

    return { dir[0], dir[1], dir[2] };
}

void AudioSource::SetPosition(const Vector3& position)
{
    ALfloat pos[] = { position.x, position.x, position.x };
    alSourcefv(m_SourceID, AL_POSITION, pos);
    CHECK_AL_ERRORS();
}
void AudioSource::SetVelocity(const Vector3& velocity)
{
    ALfloat vel[] = { velocity.x, velocity.x, velocity.x };
    alSourcefv(m_SourceID, AL_POSITION, vel);
    CHECK_AL_ERRORS();
}
void AudioSource::SetDirection(const Vector3& direction)
{
    ALfloat dir[] = { direction.x, direction.x, direction.x };
    alSourcefv(m_SourceID, AL_POSITION, dir);
    CHECK_AL_ERRORS();
}

float AudioSource::GetConeOuterGain()
{
    float gain;
    alGetSourcef(m_SourceID, AL_CONE_OUTER_GAIN, &gain);
    return gain;
}
float AudioSource::GetConeInnerAngle()
{
    float angle;
    alGetSourcef(m_SourceID, AL_CONE_INNER_ANGLE, &angle);
    return angle;
}
float AudioSource::GetConeOuterAngle()
{
    float angle;
    alGetSourcef(m_SourceID, AL_CONE_OUTER_ANGLE, &angle);
    return angle;
}

void AudioSource::SetConeOuterGain(float gain)
{
    alSourcei(m_SourceID, AL_CONE_OUTER_GAIN, gain);
    CHECK_AL_ERRORS();
}
void AudioSource::SetConeInnerAngle(float angle)
{
    alSourcei(m_SourceID, AL_CONE_INNER_ANGLE, angle);
    CHECK_AL_ERRORS();
}
void AudioSource::SetConeOuterAngle(float angle)
{
    alSourcei(m_SourceID, AL_CONE_OUTER_ANGLE, angle);
    CHECK_AL_ERRORS();
}

}
