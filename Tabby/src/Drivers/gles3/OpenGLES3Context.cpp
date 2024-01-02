#include "Drivers/gles3/OpenGLES3Context.h"
#include "tbpch.h"
#include <Drivers/gles3/GLES3.h>

#ifdef TB_PLATFORM_WEB
#define GLAD_PLATFORM_EMSCRIPTEN
#include <emscripten.h>
#endif

#define GLAD_GLES2_IMPLEMENTATION
#include <glad/gles3.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Tabby {

OpenGLES3Context::OpenGLES3Context(GLFWwindow* windowHandle)
    : m_WindowHandle(windowHandle)
{
    TB_CORE_ASSERT(windowHandle, "Window handle is null!")
}

void OpenGLES3Context::Init()
{
    // TB_PROFILE_FUNCTION();

    glfwMakeContextCurrent(m_WindowHandle);

    GladGLES2Context* context = (GladGLES2Context*)calloc(1, sizeof(GladGLES2Context));
    TB_CORE_ASSERT(context, "Failed to initialize GLES3 context!");

    int status = gladLoadGLES2Context(context, glfwGetProcAddress);
    TB_CORE_ASSERT(status, "Failed to initialize Glad!");

    GLES3::Init(context);

    TB_CORE_INFO("OpenGL Info:");
    TB_CORE_INFO("  Vendor: {0}", GLES3::GL()->GetString(GL_VENDOR));
    TB_CORE_INFO("  Renderer: {0}", GLES3::GL()->GetString(GL_RENDERER));
    TB_CORE_INFO("  Version: {0}", GLES3::GL()->GetString(GL_VERSION));

    // TB_CORE_ASSERT(GLVersion.major > 3 || (GLVersion.major == 3 && GLVersion.minor >= 3), "Tabby requires at least OpenGL version 3.3!");
}

void OpenGLES3Context::SwapBuffers()
{
    // TB_PROFILE_FUNCTION();

    glfwSwapBuffers(m_WindowHandle);
}

}
