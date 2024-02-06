#include "Drivers/gl33/OpenGL33Context.h"
#include "Drivers/gl33/GL33.h"
#include "tbpch.h"

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl33.h>

#include <SDL.h>

#if !defined(TB_PLATFORM_ANDROID)
#include <TracyOpenGL.hpp>
#endif

namespace Tabby {

OpenGL33Context::OpenGL33Context(SDL_Window* windowHandle)
    : m_WindowHandle(windowHandle)
{
    TB_CORE_ASSERT(windowHandle, "Window handle is null!")
}

void OpenGL33Context::Init()
{
    TB_PROFILE_SCOPE();

    SDL_GLContext windowContext = SDL_GL_CreateContext(m_WindowHandle);
    SDL_GL_MakeCurrent(m_WindowHandle, windowContext);

    GladGLContext* context = (GladGLContext*)calloc(1, sizeof(GladGLContext));
    TB_CORE_ASSERT(context, "Failed to initialize GL33 context!");

    int status = gladLoadGLContext(context, (GLADloadfunc)SDL_GL_GetProcAddress);
    TB_CORE_ASSERT(status, "Failed to initialize Glad!");

    GL33::Init(context);

#if !defined(TB_PLATFORM_ANDROID)
    TracyGpuContext;
#endif

    TB_CORE_INFO("OpenGL Info:");
    TB_CORE_INFO("  Vendor: {0}", GL33::GL()->GetString(GL_VENDOR));
    TB_CORE_INFO("  Renderer: {0}", GL33::GL()->GetString(GL_RENDERER));
    TB_CORE_INFO("  Version: {0}", GL33::GL()->GetString(GL_VERSION));

    TB_CORE_ASSERT(GLVersion.major > 3 || (GLVersion.major == 3 && GLVersion.minor >= 3), "Tabby requires at least OpenGL version 3.3!");
}

void OpenGL33Context::SwapBuffers()
{
    TB_PROFILE_SCOPE();

    SDL_GL_SwapWindow(m_WindowHandle);

#if !defined(TB_PLATFORM_ANDROID)
    TracyGpuCollect;
#endif
}

}
