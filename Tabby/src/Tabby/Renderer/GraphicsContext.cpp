#include "tbpch.h"
#include "Tabby/Renderer/GraphicsContext.h"

#include "Tabby/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLContext.h"

namespace Tabby {

	Scope<GraphicsContext> GraphicsContext::Create(void* window)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:    TB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateScope<OpenGLContext>(static_cast<GLFWwindow*>(window));
		}

		TB_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}