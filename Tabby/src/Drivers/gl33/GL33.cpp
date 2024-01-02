#include <glad/gl33.h>

#include "Drivers/gl33/GL33.h"

namespace Tabby {

GL33* GL33::s_Instance = nullptr;
GladGLContext* GL33::gl = nullptr;

GL33::GL33()
{
    TB_CORE_ASSERT(!s_Instance, "GL33 context already exists!");
    s_Instance = this;
}

void GL33::Init(GladGLContext* context)
{
    if (!s_Instance) {
        s_Instance = new GL33();
    }

    s_Instance->gl = context;
}

}
