#include "tbpch.h"
#include "Platform/OpenGL/OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Tabby {

	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		TB_CORE_ASSERT(windowHandle, "Window handle is null!")
	}

	void OpenGLContext::Init()
	{
		TB_PROFILE_FUNCTION();

		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		TB_CORE_ASSERT(status, "Failed to initialize Glad!");

		TB_CORE_INFO("OpenGL Info:");
		TB_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
		TB_CORE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
		TB_CORE_INFO("  Version: {0}", glGetString(GL_VERSION));

		TB_CORE_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5), "Tabby requires at least OpenGL version 4.5!");
	}

	void OpenGLContext::SwapBuffers()
	{
		TB_PROFILE_FUNCTION();

		glfwSwapBuffers(m_WindowHandle);
	}

}
