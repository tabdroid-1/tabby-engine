#ifdef TB_PLATFORM_LINUX

#include "Tabby/Core/Application.h"
#include "Tabby/Utils/PlatformUtils.h"
#include "tbpch.h"

#include <GLFW/glfw3.h>

namespace Tabby {

float Time::GetTime()
{
    return glfwGetTime();
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
