#pragma once

#include "Tracy/Tracy.hpp"
#include "Tracy/TracyC.h"
#include "Tracy/TracyOpenGL.hpp"

#define TB_PROFILE 1
#if TB_PROFILE

#define TB_PROFILE_SCOPE() ZoneScoped
#define TB_PROFILE_SCOPE_NAME(x) ZoneScopedN(x)
#define TB_PROFILE_FRAME(x) FrameMark
#define TB_PROFILE_TAG(y, x) ZoneText(x, strlen(x))
#define TB_PROFILE_LOG(text, size) TracyMessage(text, size)
#define TB_PROFILE_VALUE(text, value) TracyPlot(text, value)
#define TB_PROFILE_ALLOC(p, size) TracyCAllocS(p, size, 12)
#define TB_PROFILE_FREE(p) TracyCFreeS(p, 12)
#define TB_PROFILE_GPU(x) TracyGpuZone(x)

#endif
