#pragma once

#include <string>

namespace Tabby {

class FileDialogs {
public:
    // These return empty strings if cancelled
    static std::string OpenFile(const char* filter);
    static std::string SaveFile(const char* filter);
};

class Time {
public:
    static double GetTime();
    static double GetDeltaTime() { return m_DeltaTime; }

private:
    static void SetDeltaTime(double deltaTime) { m_DeltaTime = deltaTime; }

private:
    inline static double m_DeltaTime;

    friend class Application;
};

}
