#include <tbpch.h>
#include <Drivers/gl46/OpenGL46Context.h>
#include <Drivers/GPUProfiler.h>

#include <Tabby/Core/Application.h>

#include <tracy/TracyOpenGL.hpp>
#include <SDL.h>
#define GLAD_GL_IMPLEMENTATION
#include <gl.h>

namespace Tabby {

#if defined(TB_PROFILE) && !defined(TRACY_ON_DEMAND) && UPDATE_PROFILER_FRAMEIMAGE
static GLuint m_fiTexture[4];
static GLuint m_fiFramebuffer[4];
static GLuint m_fiPbo[4];
static GLsync m_fiFence[4];
static int m_fiIdx = 0;
static std::vector<int> m_fiQueue;
#endif

OpenGL46Context::OpenGL46Context(SDL_Window* windowHandle)
    : m_WindowHandle(windowHandle)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL46Context::Constructor");

    TB_CORE_ASSERT_TAGGED(windowHandle, "Window handle is null!");
}

void OpenGL46Context::Init()
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL46Context::Init");

    SDL_GLContext windowContext = SDL_GL_CreateContext(m_WindowHandle);
    SDL_GL_MakeCurrent(m_WindowHandle, windowContext);

    int status = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);
    TB_CORE_ASSERT_TAGGED(status, "Failed to initialize Glad!");

#if defined(TB_PROFILE) && !defined(TRACY_ON_DEMAND)
    TracyGpuContext;
#endif

    TB_CORE_INFO("OpenGL Info:");
    TB_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
    TB_CORE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
    TB_CORE_INFO("  Version: OpenGL 4.6");
    TB_CORE_INFO("  Supported Version: {0}", glGetString(GL_VERSION));

    std::string version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    size_t dotPosition = version.find('.');

    int major = std::stoi(version.substr(0, dotPosition));
    int minor = std::stoi(version.substr(dotPosition + 1));

    TB_CORE_ASSERT_TAGGED(major > 4 || (major == 4 && minor >= 6), "This device does not support OpenGL 4.6!");

#if defined(TB_PROFILE) && !defined(TRACY_ON_DEMAND) && UPDATE_PROFILER_FRAMEIMAGE
    glGenTextures(4, m_fiTexture);
    glGenFramebuffers(4, m_fiFramebuffer);
    glGenBuffers(4, m_fiPbo);
    for (int i = 0; i < 4; i++) {
        glBindTexture(GL_TEXTURE_2D, m_fiTexture[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 320, 180, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fiFramebuffer[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fiTexture[i], 0);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, m_fiPbo[i]);
        glBufferData(GL_PIXEL_PACK_BUFFER, 320 * 180 * 4, nullptr, GL_STREAM_READ);
    }
#endif
}

void OpenGL46Context::SwapBuffers()
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL46Context::SwapBuffers");
    TB_PROFILE_GPU("Tabby::OpenGL46Context::SwapBuffers");

#if defined(TB_PROFILE) && !defined(TRACY_ON_DEMAND) && UPDATE_PROFILER_FRAMEIMAGE
    TB_CORE_ASSERT(m_fiQueue.empty() || m_fiQueue.front() != m_fiIdx); // check for buffer overrun
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fiFramebuffer[m_fiIdx]);
    glBlitFramebuffer(0, 0, Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight(), 0, 0, 320, 180, GL_COLOR_BUFFER_BIT, GL_LINEAR);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fiFramebuffer[m_fiIdx]);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, m_fiPbo[m_fiIdx]);
    glReadPixels(0, 0, 320, 180, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    m_fiFence[m_fiIdx] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    m_fiQueue.emplace_back(m_fiIdx);
    m_fiIdx = (m_fiIdx + 1) % 4;

    while (!m_fiQueue.empty()) {
        const auto fiIdx = m_fiQueue.front();
        if (glClientWaitSync(m_fiFence[fiIdx], 0, 0) == GL_TIMEOUT_EXPIRED)
            break;
        glDeleteSync(m_fiFence[fiIdx]);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, m_fiPbo[fiIdx]);
        auto ptr = glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, 320 * 180 * 4, GL_MAP_READ_BIT);
        FrameImage(ptr, 320, 180, m_fiQueue.size(), true);
        glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
        m_fiQueue.erase(m_fiQueue.begin());
    }
#endif

    SDL_GL_SwapWindow(m_WindowHandle);

#if defined(TB_PROFILE) && !defined(TRACY_ON_DEMAND)
    TracyGpuCollect;
#endif
}

}
