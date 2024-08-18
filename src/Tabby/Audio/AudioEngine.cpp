#include <tbpch.h>
#include <Tabby/Audio/AudioEngine.h>
#include <Tabby/Audio/AudioSource.h>
#include <Tabby/Audio/Audio.h>

#include <AL/alext.h>
#include <AL/al.h>

#define CHECK_AL_ERRORS() \
    FetchALErrors(__FILE__, __LINE__)

#define CHECK_ALC_ERRORS(device) \
    FetchALCErrors(device, __FILE__, __LINE__)

namespace Tabby {

static constexpr int BufferMillisec = 200;

void AudioEngine::FetchALErrors(const std::filesystem::path& file, int line)
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioEngine::FetchALErrors");

    bool error_found = false;
    std::stringstream err_msg_stream;

    ALenum error_flag = alGetError();
    while (error_flag != AL_NO_ERROR) {
        if (!error_found) {
            error_found = true;
            err_msg_stream << "OpenAL Flags: ";
        }

        switch (error_flag) {
        case AL_INVALID_NAME:
            err_msg_stream << "\"AL_INVALID_NAME\" ";
            break;
        case AL_INVALID_ENUM:
            err_msg_stream << "\"AL_INVALID_ENUM\" ";
            break;
        case AL_INVALID_VALUE:
            err_msg_stream << "\"AL_INVALID_VALUE\" ";
            break;
        case AL_INVALID_OPERATION:
            err_msg_stream << "\"AL_INVALID_OPERATION\" ";
            break;
        case AL_OUT_OF_MEMORY:
            err_msg_stream << "\"AL_OUT_OF_MEMORY\" ";
            break;
        default:
            err_msg_stream << "\"Flag not deducable\" ";
            break;
        }

        error_flag = alGetError();
    }

    if (error_found) {
        err_msg_stream << "File " << file << " @ Line " << line;
    }

    TB_CORE_ASSERT_TAGGED(!error_found, err_msg_stream.str());
}

void AudioEngine::FetchALCErrors(ALCdevice* device, const std::filesystem::path& file, int line)
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioEngine::FetchALCErrors");

    bool error_found = false;
    std::stringstream err_msg_stream;

    ALCenum error_flag = alcGetError(device);
    while (error_flag != ALC_NO_ERROR) {
        if (!error_found) {
            error_found = true;
            err_msg_stream << "OpenAL Flags: ";
        }

        switch (error_flag) {
        case ALC_INVALID_DEVICE:
            err_msg_stream << "\"ALC_INVALID_DEVICE\" ";
            break;
        case ALC_INVALID_CONTEXT:
            err_msg_stream << "\"ALC_INVALID_CONTEXT\" ";
            break;
        case ALC_INVALID_ENUM:
            err_msg_stream << "\"ALC_INVALID_ENUM\" ";
            break;
        case ALC_INVALID_VALUE:
            err_msg_stream << "\"ALC_INVALID_VALUE\" ";
            break;
        case ALC_OUT_OF_MEMORY:
            err_msg_stream << "\"ALC_OUT_OF_MEMORY\" ";
            break;
        default:
            err_msg_stream << "\"Flag not deducable\" ";
            break;
        }

        error_flag = alcGetError(device);
    }

    if (error_found) {
        err_msg_stream << "File " << file << " @ Line " << line;
    }

    TB_CORE_ASSERT_TAGGED(!error_found, err_msg_stream.str());
}

AudioEngine::AudioEngine()
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioEngine::Constructor");

    TB_CORE_ASSERT_TAGGED(!s_Instance, "Audio Engine instance already created!");
    s_Instance = this;

    m_AlcDevice = alcOpenDevice(nullptr);
    CHECK_ALC_ERRORS(m_AlcDevice);
    TB_CORE_ASSERT_TAGGED(m_AlcDevice, "alcOpenDevice: Unable to create OpenAL device");

    m_AlcContext = alcCreateContext(m_AlcDevice, nullptr);
    CHECK_ALC_ERRORS(m_AlcDevice);
    TB_CORE_ASSERT_TAGGED(m_AlcContext, "alcCreateContext: Unable to create OpenAL context");

    if (!alcMakeContextCurrent(m_AlcContext))
        TB_CORE_ASSERT_TAGGED(false, "alcMakeCurrentContext: Could not set OpenAL context to current context");
    CHECK_ALC_ERRORS(m_AlcDevice);

    alListenerf(AL_GAIN, 0.50f);
    CHECK_AL_ERRORS();
    alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
    CHECK_AL_ERRORS();

    m_ShouldThreadClose = false;
    m_PollingThread = std::thread(&AudioEngine::EnginePollingThread, this);
}

AudioEngine::~AudioEngine()
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioEngine::Destructor");

    m_ShouldThreadClose = true;
    if (m_PollingThread.joinable()) {
        m_PollingThread.join();
    }
}

void AudioEngine::Init()
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioEngine::Init");

    if (!s_Instance)
        s_Instance = new AudioEngine();
}

void AudioEngine::Shutdown()
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioEngine::Shutdown");

    delete s_Instance;
}

Vector3 AudioEngine::GetPosition()
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioEngine::GetPosition");

    ALfloat* listenerPos;
    alGetListenerfv(AL_POSITION, listenerPos);
    return { listenerPos[0], listenerPos[0], listenerPos[0] };
}

Vector3 AudioEngine::GetVelocity()
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioEngine::GetVelocity");

    ALfloat* listenerVel;
    alGetListenerfv(AL_POSITION, listenerVel);
    return { listenerVel[0], listenerVel[0], listenerVel[0] };
}

float* AudioEngine::GetOrientation()
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioEngine::GetOrientation");
    ALfloat* listenerOri;
    alGetListenerfv(AL_POSITION, listenerOri);
    return listenerOri;
}

void AudioEngine::SetPosition(const Vector3& position)
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioEngine::SetPosition");

    ALfloat listenerPos[] = { position.x, position.x, position.x };
    alListenerfv(AL_POSITION, listenerPos);
}
void AudioEngine::SetVelocity(const Vector3& velocity)
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioEngine::SetVelocity");

    ALfloat listenerVel[] = { velocity.x, velocity.x, velocity.x };
    alListenerfv(AL_VELOCITY, listenerVel);
}
void AudioEngine::SetOrientation(const Vector3& look, const Vector3& up)
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioEngine::SetOrientation");

    ALfloat listenerOri[] = { look.x, look.x, look.x, up.x, up.y, up.z };
    alListenerfv(AL_ORIENTATION, listenerOri);
}

AudioSource* AudioEngine::CreateAudioSource(AssetHandle handle)
{
    TB_PROFILE_SCOPE_NAME("Tabby::AudioEngine::CreateAudioSource");

    AudioSource* audioSource = new AudioSource();
    if (handle)
        audioSource->SetAudio(handle);

    m_MusicMixer.push_back(audioSource);

    return audioSource;
}

void AudioEngine::EnginePollingThread()
{

    TB_PROFILE_SET_THREAD_NAME("Tabby::AudioEngine");

    while (!m_ShouldThreadClose) {
        TB_PROFILE_FRAME();

        TB_PROFILE_SCOPE_NAME("Tabby::AudioEngine::Update");

        m_MusicMixerLock.lock();

        for (AudioSource* player : m_MusicMixer) {
            TB_PROFILE_SCOPE_NAME("Tabby::AudioEngine::Update::UpdatePlayer");
            player->UpdatePlayer();
        }

        m_MusicMixerLock.unlock();

        static constexpr int UPDATES_PER_SECOND = 200;
        static constexpr int UPDATE_FRAME_MS = 1000 / UPDATES_PER_SECOND;
        std::this_thread::sleep_for(std::chrono::milliseconds(UPDATE_FRAME_MS));
    }
}

}
