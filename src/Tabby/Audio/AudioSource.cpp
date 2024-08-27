#include <Tabby/Asset/AssetManager.h>
#include <Tabby/Audio/AudioEngine.h>
#include <Tabby/Audio/AudioSource.h>
#include <Tabby/Asset/AssetType.h>

#include <SDL_rwops.h>
#include <AL/alext.h>
#include <AL/al.h>

#define CHECK_AL_ERRORS() \
    AudioEngine::FetchALErrors(__FILE__, __LINE__)

namespace Tabby {

AudioSource::AudioSource()
    : m_Music(nullptr)
    , m_Playing(false)
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSource::Constructor");
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
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSource::Destructor");

    alSourceStop(m_SourceID);
    CHECK_AL_ERRORS();
    alSourcei(m_SourceID, AL_BUFFER, (int)NULL);
    CHECK_AL_ERRORS();
    alDeleteSources(1, &m_SourceID);
    alDeleteBuffers(NUM_BUFFERS, m_BufferIDs.data());
    CHECK_AL_ERRORS();
}

void AudioSource::Play()
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSource::Play");

#if !TB_HEADLESS
    AudioEngine::m_MusicMixerLock.lock();
    if (!m_Music) {
        TB_CORE_ERROR("AudioSource's music is null/not set!");
        return;
    }

    m_Playing = true;
    alSourcePlay(m_SourceID);
    CHECK_AL_ERRORS();
    AudioEngine::m_MusicMixerLock.unlock();
#endif
}

void AudioSource::Pause()
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSource::Pause");

#if !TB_HEADLESS
    AudioEngine::m_MusicMixerLock.lock();
    if (!m_Music) {
        TB_CORE_ERROR("AudioSource's music is null/not set!");
        return;
    }

    m_Playing = false;
    alSourcePause(m_SourceID);
    CHECK_AL_ERRORS();
    AudioEngine::m_MusicMixerLock.unlock();
#endif
}

void AudioSource::TogglePlay()
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSource::TogglePlay");

    if (m_Playing)
        Pause();
    else
        Play();
}

bool AudioSource::IsPlaying()
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSource::IsPlaying");

    return m_Playing;
}

void AudioSource::SetAudio(AssetHandle audioHandle)
{
#if !TB_HEADLESS
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSource::SetAudio");

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
#endif
}

void AudioSource::UnsetAudio()
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSource::UnsetAudio");

#if !TB_HEADLESS
    AudioEngine::m_MusicMixerLock.lock();
    alSourcei(m_SourceID, AL_BUFFER, (int)NULL);
    CHECK_AL_ERRORS();
    m_Cursor = 0;

    AudioEngine::m_MusicMixerLock.unlock();
#endif
}

void AudioSource::UpdateBuffer()
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSource::UpdateBuffer");

#if !TB_HEADLESS
    alSourceStop(m_SourceID);
    CHECK_AL_ERRORS();
    alSourcei(m_SourceID, AL_BUFFER, (int)NULL);
    CHECK_AL_ERRORS();
    uint64_t buffer_count = 0;
    while (buffer_count < m_BufferIDs.size()) {
        TB_PROFILE_SCOPE_NAME("Tabby::AudioSource::UpdateBuffer::Iteration");

        uint64_t buffer_size = static_cast<ALsizei>(std::min(MUSIC_BUFFER_SIZE, m_Music->dataSize - m_Cursor));
        buffer_size -= buffer_size % 8;
        if (buffer_size == 0)
            break;

        std::vector<byte> buffer_data(buffer_size);
        SDL_RWseek(m_Music->m_RWops, m_Music->dataStart + m_Cursor, RW_SEEK_SET);
        if (SDL_RWread(m_Music->m_RWops, buffer_data.data(), 1, buffer_size) != buffer_size) {
            TB_CORE_ASSERT_TAGGED(false, "Failed to read WAV data using SDL_RWops");
        }
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
#endif
}

void AudioSource::UpdatePlayer()
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSource::UpdatePlayer");

#if !TB_HEADLESS
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
        TB_PROFILE_SCOPE_NAME("Tabby::AudioSource::UpdatePlayer::Iteration");

        buffers_processed--;

        ALuint buffer;
        alSourceUnqueueBuffers(m_SourceID, 1, &buffer);
        CHECK_AL_ERRORS();

        if (m_Cursor >= m_Music->dataSize)
            continue;

        uint64_t buffer_size = static_cast<ALsizei>(std::min(MUSIC_BUFFER_SIZE, m_Music->dataSize - m_Cursor));
        buffer_size -= buffer_size % 8;

        std::vector<byte> buffer_data(buffer_size);
        SDL_RWseek(m_Music->m_RWops, m_Music->dataStart + m_Cursor, RW_SEEK_SET);
        if (SDL_RWread(m_Music->m_RWops, buffer_data.data(), 1, buffer_size) != buffer_size) {
            TB_CORE_ASSERT_TAGGED(false, "Failed to read WAV data using SDL_RWops");
        }

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
#endif
}

float AudioSource::GetPitch()
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSource::GetPitch");

    float pitch;
    alGetSourcef(m_SourceID, AL_PITCH, &pitch);
    return pitch;
}

float AudioSource::GetGain()
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSource::GetGain");

    float gain;
    alGetSourcef(m_SourceID, AL_GAIN, &gain);
    return gain;
}

void AudioSource::SetPitch(float pitch)
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSource::SetPitch");

    alSourcef(m_SourceID, AL_PITCH, pitch);
    CHECK_AL_ERRORS();
}
void AudioSource::SetGain(float gain)
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSource::SetGain");

    alSourcef(m_SourceID, AL_GAIN, gain);
    CHECK_AL_ERRORS();
}

void AudioSource::SetMinGain(float gain)
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSource::SetMinGain");
    alSourcef(m_SourceID, AL_MIN_GAIN, gain);
    CHECK_AL_ERRORS();
}

void AudioSource::SetMaxGain(float gain)
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSource::SetMaxGain");

    alSourcef(m_SourceID, AL_MAX_GAIN, gain);
    CHECK_AL_ERRORS();
}

float AudioSource::GetRolloffFactor()
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSource::GetRolloffFactor");

    float factor;
    alGetSourcef(m_SourceID, AL_ROLLOFF_FACTOR, &factor);
    return factor;
}

float AudioSource::GetMaxDistance()
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSource::GetMaxDistance");
    float distance;
    alGetSourcef(m_SourceID, AL_MAX_DISTANCE, &distance);
    return distance;
}

float AudioSource::GetReferenceDistance()
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSource::GetReferenceDistance");
    float distance;
    alGetSourcef(m_SourceID, AL_REFERENCE_DISTANCE, &distance);
    return distance;
}

void AudioSource::SetRolloffFactor(float rate)
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSource::SetRolloffFactor");

    alSourcef(m_SourceID, AL_ROLLOFF_FACTOR, rate);
    CHECK_AL_ERRORS();
}

void AudioSource::SetMaxDistance(float distance)
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSource::SetMaxDistance");

    alSourcef(m_SourceID, AL_MAX_DISTANCE, distance);
    CHECK_AL_ERRORS();
}

void AudioSource::SetReferenceDistance(float distance)
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSource::SetReferenceDistance");

    alSourcef(m_SourceID, AL_REFERENCE_DISTANCE, distance);
    CHECK_AL_ERRORS();
}

bool AudioSource::GetRelative()
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSource::GetRelative");

    int relative;
    alGetSourcei(m_SourceID, AL_SOURCE_RELATIVE, &relative);

    if (relative == AL_TRUE)
        return true;
    else
        return false;
}
bool AudioSource::GetLooping()
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSource::GetLooping");

    return m_Loop;
}

float AudioSource::GetMinGain()
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSource::GetMinGain");

    float gain;
    alGetSourcef(m_SourceID, AL_MIN_GAIN, &gain);
    return gain;
}

float AudioSource::GetMaxGain()
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSource::GetMaxGain");

    float gain;
    alGetSourcef(m_SourceID, AL_MAX_GAIN, &gain);
    return gain;
}

void AudioSource::SetRelative(bool relative)
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSource::SetRelative");

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
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSource::SetLooping");

    m_Loop = looping;
}

Vector3 AudioSource::GetPosition()
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSource::GetPosition");

    Vector3 position;
    alGetSource3f(m_SourceID, AL_POSITION, &position.x, &position.y, &position.z);
    CHECK_AL_ERRORS();

    return position;
}

Vector3 AudioSource::GetVelocity()
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSource::GetVelocity");

    Vector3 velocity;
    alGetSource3f(m_SourceID, AL_VELOCITY, &velocity.x, &velocity.y, &velocity.z);
    CHECK_AL_ERRORS();

    return velocity;
}

Vector3 AudioSource::GetDirection()
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSource::GetDirection");

    Vector3 direction;
    alGetSource3f(m_SourceID, AL_DIRECTION, &direction.x, &direction.y, &direction.z);
    CHECK_AL_ERRORS();

    return direction;
}

void AudioSource::SetPosition(const Vector3& position)
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSource::SetPosition");

    alSource3f(m_SourceID, AL_POSITION, position.x, position.y, position.z);
    CHECK_AL_ERRORS();
}
void AudioSource::SetVelocity(const Vector3& velocity)
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSource::SetVelocity");

    alSource3f(m_SourceID, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
    CHECK_AL_ERRORS();
}
void AudioSource::SetDirection(const Vector3& direction)
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSource::SetDirection");

    alSource3f(m_SourceID, AL_DIRECTION, direction.x, direction.y, direction.z);
    CHECK_AL_ERRORS();
}

float AudioSource::GetConeOuterGain()
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSource::GetConeOuterGain");

    float gain;
    alGetSourcef(m_SourceID, AL_CONE_OUTER_GAIN, &gain);
    return gain;
}

float AudioSource::GetConeInnerAngle()
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSource::GetConeInnerAngle");

    float angle;
    alGetSourcef(m_SourceID, AL_CONE_INNER_ANGLE, &angle);
    return angle;
}

float AudioSource::GetConeOuterAngle()
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSource::GetConeOuterAngle");

    float angle;
    alGetSourcef(m_SourceID, AL_CONE_OUTER_ANGLE, &angle);
    return angle;
}

void AudioSource::SetConeOuterGain(float gain)
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSource::SetConeOuterGain");

    alSourcei(m_SourceID, AL_CONE_OUTER_GAIN, gain);
    CHECK_AL_ERRORS();
}

void AudioSource::SetConeInnerAngle(float angle)
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSource::SetConeInnerAngle");

    alSourcei(m_SourceID, AL_CONE_INNER_ANGLE, angle);
    CHECK_AL_ERRORS();
}

void AudioSource::SetConeOuterAngle(float angle)
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioSource::SetConeOuterAngle");

    alSourcei(m_SourceID, AL_CONE_OUTER_ANGLE, angle);
    CHECK_AL_ERRORS();
}

}
