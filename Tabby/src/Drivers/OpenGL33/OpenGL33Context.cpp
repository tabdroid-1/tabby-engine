#include "Drivers/OpenGL33/OpenGL33Context.h"
#include "tbpch.h"

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl33.h>

#include <GLFW/glfw3.h>

namespace Tabby {

OpenGL33Context::OpenGL33Context(GLFWwindow* windowHandle)
    : m_WindowHandle(windowHandle)
{
    TB_CORE_ASSERT(windowHandle, "Window handle is null!")
}

void OpenGL33Context::Init()
{
    TB_PROFILE_FUNCTION();

    glfwMakeContextCurrent(m_WindowHandle);
    glfwMakeContextCurrent(m_WindowHandle);
    int version = gladLoadGL(glfwGetProcAddress);
    TB_CORE_ASSERT(status, "Failed to initialize Glad!");

    TB_CORE_INFO("OpenGL Info:");
    TB_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
    TB_CORE_INFO("  Renderer: {0}", GLAD_VERSION_MAJOR(version));
    TB_CORE_INFO("  Version: {0}", GLAD_VERSION_MINOR(version));
}

void OpenGL33Context::SwapBuffers()
{
    TB_PROFILE_FUNCTION();

    glfwSwapBuffers(m_WindowHandle);
}

}
