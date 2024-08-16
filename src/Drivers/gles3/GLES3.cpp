#include <gles3.h>

#include "Drivers/gles3/GLES3.h"

namespace Tabby {

GLES3* GLES3::s_Instance = nullptr;
GladGLES2Context* GLES3::gl = nullptr;

GLES3::GLES3()
{
    TB_PROFILE_SCOPE_NAME("Tabby::GLES3::Constructor");

    TB_CORE_ASSERT_TAGGED(!s_Instance, "GLES3 context already exists!");
    s_Instance = this;
}

void GLES3::Init(GladGLES2Context* context)
{
    TB_PROFILE_SCOPE_NAME("Tabby::GLES3::Init");

    if (!s_Instance) {
        s_Instance = new GLES3();
    }

    s_Instance->gl = context;
}
}
