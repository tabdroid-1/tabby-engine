#include <tbpch.h>
#include <Drivers/gles3/GLES3.h>
#include <Drivers/gles3/OpenGLES3Context.h>

#ifdef TB_PLATFORM_WEB
#define GLAD_PLATFORM_EMSCRIPTEN
#include <emscripten.h>
#elif defined(TB_PLATFORM_ANDROID)
#include <SDL_egl.h>
#include <SDL_syswm.h>
#endif

#define GLAD_GLES2_IMPLEMENTATION
#include <gles3.h>
#include <SDL.h>

namespace Tabby {

OpenGLES3Context::OpenGLES3Context(SDL_Window* windowHandle)
    : m_WindowHandle(windowHandle)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Context::Constructor");

    TB_CORE_ASSERT_TAGGED(windowHandle, "Window handle is null!");
}

void OpenGLES3Context::Init()
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Context::Init");

    SDL_GLContext windowContext = SDL_GL_CreateContext(m_WindowHandle);
    SDL_GL_MakeCurrent(m_WindowHandle, windowContext);

    GladGLES2Context* context = (GladGLES2Context*)calloc(1, sizeof(GladGLES2Context));
    TB_CORE_ASSERT_TAGGED(context, "Failed to initialize GLES3 context!");

#if !defined(TB_PLATFORM_ANDROID)
    int status = gladLoadGLES2Context(context, (GLADloadfunc)SDL_GL_GetProcAddress);
#elif defined(TB_PLATFORM_ANDROID)
    int status = gladLoadGLES2Context(context, (GLADloadfunc)eglGetProcAddress);
#endif
    TB_CORE_ASSERT_TAGGED(status, "Failed to initialize Glad!");

    GLES::Init(context);

    TB_CORE_INFO("OpenGL Info:");
    TB_CORE_INFO("  Vendor: {0}", GLES::gl->GetString(GL_VENDOR));
    TB_CORE_INFO("  Renderer: {0}", GLES::gl->GetString(GL_RENDERER));
    TB_CORE_INFO("  Version: OpenGL ES 3.0");
    TB_CORE_INFO("  Max Supported Version: {0}", GLES::gl->GetString(GL_VERSION));

    std::string version = reinterpret_cast<const char*>(GLES::gl->GetString(GL_VERSION));
    size_t dotPosition = version.find('.');

    int major = std::stoi(version.substr(dotPosition - 1));
    int minor = std::stoi(version.substr(dotPosition + 1));

    TB_CORE_ASSERT_TAGGED(major > 3 || (major == 3 && minor >= 0), "Tabby requires at least OpenGL ES version 3.0!");
}

void OpenGLES3Context::SwapBuffers()
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Context::SwapBuffers");

    SDL_GL_SwapWindow(m_WindowHandle);
}

}
