#ifdef TB_PLATFORM_WEB

#include "Tabby/Core/Application.h"
#include "Tabby/Utils/PlatformUtils.h"
#include "tbpch.h"

#include <SDL.h>

namespace Tabby {

float Time::GetTime()
{
    return static_cast<float>(SDL_GetTicks() / 1000.0f);
}

std::string FileDialogs::OpenFile(const char* filter)
{
    return std::string();
}

std::string FileDialogs::SaveFile(const char* filter)
{

    return std::string();
}

}

#endif
