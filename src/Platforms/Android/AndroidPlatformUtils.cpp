#ifdef TB_PLATFORM_ANDROID

#include "tbpch.h"

#include "Tabby/Utils/PlatformUtils.h"
#include "Tabby/Core/Time/Time.h"

namespace Tabby {

double Time::GetTime()
{
    TB_PROFILE_SCOPE_NAME("Tabby::Time::GetTime");

    using namespace std::chrono;
    system_clock::time_point currentTimePoint = system_clock::now();
    duration<double> timeSinceEpoch = currentTimePoint.time_since_epoch();

    double result = timeSinceEpoch.count();

    return result;
}

std::string FileDialogs::OpenFile(const char* filter)
{
    TB_PROFILE_SCOPE_NAME("Tabby::FileDialogs::OpenFile");

    return std::string();
}

std::string FileDialogs::SaveFile(const char* filter)
{
    TB_PROFILE_SCOPE_NAME("Tabby::FileDialogs::SaveFile");

    return std::string();
}

}

#endif
