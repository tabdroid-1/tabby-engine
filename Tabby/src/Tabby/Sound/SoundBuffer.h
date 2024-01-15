#pragma once

#include "miniaudio/miniaudio.h"

namespace Tabby {
class SoundBuffer {
public:
    SoundBuffer(const std::string& filename);
    ~SoundBuffer();

    // ALint Source() { return a_Source; }

    bool isPlaying() { return m_Playing; }

    void Play();
    void Pause();

    void PlayPause()
    {
        if (m_Playing) {
            Play();
        } else {
            Pause();
        }
        m_Playing = !m_Playing;
    };
    void Stop();

    void UpdateBuffer();

    std::string getPath() { return m_Path; }

    void SetGain(const float& val);
    void Duration(float& val);

private:
    ma_data_converter converter; // Audio data converter

    // AudioCallback callback; // Audio buffer callback for buffer filling on audio threads
    // rAudioProcessor* processor; // Audio processor

    float m_Volume; // Audio buffer volume
    float m_Pitch; // Audio buffer pitch
    float m_Pan; // Audio buffer pan (0.0f to 1.0f)

    bool m_Playing; // Audio buffer state: AUDIO_PLAYING
    bool m_Paused; // Audio buffer state: AUDIO_PAUSED
    bool m_Looping; // Audio buffer looping, default to true for AudioStreams
    int m_Usage; // Audio buffer usage mode: STATIC or STREAM

    bool m_IsSubBufferProcessed[2]; // SubBuffer processed (virtual double buffer)
    unsigned int m_SizeInFrames; // Total buffer size in frames
    unsigned int m_FrameCursorPos; // Frame cursor position
    unsigned int m_FramesProcessed; // Total frames processed in this buffer (required for play timing)

    unsigned char* m_Data; // Data buffer, on music stream keeps filling

    // rAudioBuffer* next; // Next audio buffer on the list
    // rAudioBuffer* prev; // Previous audio buffer on the list

    std::string m_Path;
};
}
