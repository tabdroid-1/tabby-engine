#include "Drivers/gl33/OpenGL33Context.h"
#include "tbpch.h"

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl33.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Tabby {

OpenGL33Context::OpenGL33Context(GLFWwindow* windowHandle)
    : m_WindowHandle(windowHandle)
{
    TB_CORE_ASSERT(windowHandle, "Window handle is null!")
}

void OpenGL33Context::Init()
{
    // TB_PROFILE_FUNCTION();

    glfwMakeContextCurrent(m_WindowHandle);
    int status = gladLoadGL(glfwGetProcAddress);
    TB_CORE_ASSERT(status, "Failed to initialize Glad!");

    TB_CORE_INFO("OpenGL Info:");
    TB_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
    TB_CORE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
    TB_CORE_INFO("  Version: {0}", glGetString(GL_VERSION));

    TB_CORE_ASSERT(GLVersion.major > 3 || (GLVersion.major == 3 && GLVersion.minor >= 3), "Tabby requires at least OpenGL version 3.3!");
}

void OpenGL33Context::SwapBuffers()
{
    // TB_PROFILE_FUNCTION();

    glfwSwapBuffers(m_WindowHandle);
}

}
