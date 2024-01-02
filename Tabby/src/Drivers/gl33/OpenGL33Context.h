#pragma once


#include "Tabby/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Tabby {

class OpenGL33Context : public GraphicsContext {
public:
    OpenGL33Context(GLFWwindow* windowHandle);

    virtual void Init() override;
    virtual void SwapBuffers() override;

private:
    GLFWwindow* m_WindowHandle;
};

}
