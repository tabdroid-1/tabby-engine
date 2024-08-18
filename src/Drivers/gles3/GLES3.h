#pragma once

struct GladGLES2Context;

namespace Tabby {

class GLES {
public:
    static void Init(GladGLES2Context* context) { gl = context; }
    static GladGLES2Context* GL() { return gl; }

    inline static GladGLES2Context* gl;
};

}
