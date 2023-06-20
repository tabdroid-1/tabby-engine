#include "tbpch.h"
#include "Tabby/Renderer/RenderCommand.h"

namespace Tabby {

	Scope<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();

}