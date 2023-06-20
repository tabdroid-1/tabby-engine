#include "tbpch.h"
#include "Tabby/Renderer/VertexArray.h"

#include "Tabby/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Tabby {

	Ref<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:    TB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateRef<OpenGLVertexArray>();
		}

		TB_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}