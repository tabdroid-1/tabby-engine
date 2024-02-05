#include "Drivers/gles3/OpenGLES3Context.h"
#include "tbpch.h"
#include <Drivers/gles3/GLES3.h>

#ifdef TB_PLATFORM_WEB
#define GLAD_PLATFORM_EMSCRIPTEN
#include <emscripten.h>
#endif

#define GLAD_GLES2_IMPLEMENTATION
#include <glad/gles3.h>

#include <SDL.h>

namespace Tabby {

OpenGLES3Context::OpenGLES3Context(SDL_Window* windowHandle)
    : m_WindowHandle(windowHandle)
{
    TB_CORE_ASSERT(windowHandle, "Window handle is null!")
}

void OpenGLES3Context::Init()
{
    TB_PROFILE_SCOPE();

    SDL_GLContext windowContext = SDL_GL_CreateContext(m_WindowHandle);
    SDL_GL_MakeCurrent(m_WindowHandle, windowContext);

    GladGLES2Context* context = (GladGLES2Context*)calloc(1, sizeof(GladGLES2Context));
    TB_CORE_ASSERT(context, "Failed to initialize GLES3 context!");

    int status = gladLoadGLES2Context(context, (GLADloadfunc)SDL_GL_GetProcAddress);
    TB_CORE_ASSERT(status, "Failed to initialize Glad!");

    GLES3::Init(context);

    TB_CORE_INFO("OpenGL Info:");
    TB_CORE_INFO("  Vendor: {0}", GLES3::GL()->GetString(GL_VENDOR));
    TB_CORE_INFO("  Renderer: {0}", GLES3::GL()->GetString(GL_RENDERER));
    TB_CORE_INFO("  Version: {0}", GLES3::GL()->GetString(GL_VERSION));

    TB_CORE_ASSERT(GLVersion.major > 3 || (GLVersion.major == 3 && GLVersion.minor >= 0), "Tabby requires at least OpenGL ES version 3.0!");
}

void OpenGLES3Context::SwapBuffers()
{
    TB_PROFILE_SCOPE();

    SDL_GL_SwapWindow(m_WindowHandle);
}

}
