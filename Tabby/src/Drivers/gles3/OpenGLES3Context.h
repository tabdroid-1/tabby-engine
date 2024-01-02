#pragma once

#include "Tabby/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Tabby {

class OpenGLES3Context : public GraphicsContext {
public:
    OpenGLES3Context(GLFWwindow* windowHandle);

    virtual void Init() override;
    virtual void SwapBuffers() override;

private:
    GLFWwindow* m_WindowHandle;
};

}
