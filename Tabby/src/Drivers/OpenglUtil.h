#pragma once

#include <Tabby/Renderer/RendererAPI.h>
#include <Drivers/gl33/GL33.h>
#include <Drivers/gles3/GLES3.h>

struct GladGLContext;

namespace Tabby {

static GladGLContext* GL()
{
    if (RendererAPI::GetAPI() == RendererAPI::API::OpenGL33)
        return GL33::GL();
    else if (RendererAPI::GetAPI() == RendererAPI::API::OpenGLES3)
        return GLES3::GL();
}

}
