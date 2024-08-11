#pragma once

struct ma_engine;

namespace Tabby {

class AudioEngine {
public:
    static void Init();
    static void Shutdown();

private:
    AudioEngine();
    ~AudioEngine();

    inline static ma_engine* m_Engine;
};

}
