#ifdef TB_PLATFORM_MACOS

#include "Tabby/Utils/PlatformUtils.h"

#include <SDL3/SDL.h>

namespace Tabby {

double Time::GetTime()
{
    using namespace std::chrono;
    system_clock::time_point currentTimePoint = system_clock::now();
    duration<double> timeSinceEpoch = currentTimePoint.time_since_epoch();

    // Convert duration to double
    double result = timeSinceEpoch.count();

    return result;
}

}

#endif
