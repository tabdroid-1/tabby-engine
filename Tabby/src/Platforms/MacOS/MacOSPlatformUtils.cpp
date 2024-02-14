#ifdef TB_PLATFORM_MACOS

#include "Tabby/Utils/PlatformUtils.h"

#include <SDL.h>

namespace Tabby {

float Time::GetTime()
{
    return static_cast<float>(SDL_GetTicks() / 1000.0f);
    // return SDL_GetPerformanceFrequency();
}

}

#endif
