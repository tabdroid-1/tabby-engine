#ifdef TB_PLATFORM_MACOS

#include "Tabby/Utils/PlatformUtils.h"

#include <GLFW/glfw3.h>

namespace Tabby {

float Time::GetTime()
{
    return glfwGetTime();
}

}

#endif
