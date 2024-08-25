#include <tbpch.h>
#include <Tabby/Renderer/GraphicsContext.h>
#include <Drivers/gles3/OpenGLES3Context.h>
#include <Drivers/gl46/OpenGL46Context.h>
#include <Drivers/gl33/OpenGL33Context.h>
#include <Drivers/null/NullAPIContext.h>
#include <Tabby/Renderer/Renderer.h>

namespace Tabby {

Scope<GraphicsContext> GraphicsContext::Create(void* window)
{
    TB_PROFILE_SCOPE_NAME("Tabby::GraphicsContext::Create");

    switch (Renderer::GetAPI()) {
    case RendererAPI::API::Null:
        return CreateScope<NullAPIContext>(static_cast<SDL_Window*>(window));
    case RendererAPI::API::OpenGL46:
        return CreateScope<OpenGL46Context>(static_cast<SDL_Window*>(window));
    case RendererAPI::API::OpenGL33:
        return CreateScope<OpenGL33Context>(static_cast<SDL_Window*>(window));
    case RendererAPI::API::OpenGLES3:
        return CreateScope<OpenGLES3Context>(static_cast<SDL_Window*>(window));
    }

    TB_CORE_ASSERT_TAGGED(false, "Unknown RendererAPI!");
    return nullptr;
}

}
