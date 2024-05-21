#pragma once

struct GladGLContext;

namespace Tabby {

class GL33 {

public:
    GL33();
    static void Init(GladGLContext*);
    static GladGLContext* GL() { return s_Instance->gl; }

private:
    static GL33* s_Instance;
    static GladGLContext* gl;
};

}
