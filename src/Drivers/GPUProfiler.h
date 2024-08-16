#pragma once

// TRACY_ON_DEMAND=1 breaks gpu profiling
#if !defined(TB_PROFILE) || defined(TRACY_ON_DEMAND)
#define TB_PROFILE_GPU(x)

#elif defined(TB_PROFILE) && !defined(TRACY_ON_DEMAND)
#include <gl.h>
#include "tracy/TracyOpenGL.hpp"
#define TB_PROFILE_GPU(x) TracyGpuZone(x)

#endif
