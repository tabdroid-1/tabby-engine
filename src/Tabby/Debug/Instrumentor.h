#pragma once

#if !defined(TB_PROFILE) || defined(TB_PLATFORM_ANDROID)

#define TB_PROFILE_SCOPE()
#define TB_PROFILE_SCOPE_NAME(x)
#define TB_PROFILE_SET_THREAD_NAME(x) TracyCSetThreadName(x)
#define TB_PROFILE_FRAME(x) FrameMark
#define TB_PROFILE_FRAME_START(x) FrameMarkStart(x)
#define TB_PROFILE_FRAME_END(x) FrameMarkEnd(x)
#define TB_PROFILE_TAG(y, x)
#define TB_PROFILE_LOG(text, size)
#define TB_PROFILE_VALUE(text, value)
#define TB_PROFILE_ALLOC(p, size)
#define TB_PROFILE_FREE(p)
#define TB_PROFILE_GPU(x)

#elif defined(TB_PROFILE) && !defined(TB_PLATFORM_ANDROID)
#define TRACY_ENABLE 1
#include "tracy/Tracy.hpp"
#include "tracy/TracyC.h"

#define TB_PROFILE_SCOPE() ZoneScoped
#define TB_PROFILE_SCOPE_NAME(x) ZoneScopedN(x)
#define TB_PROFILE_SET_THREAD_NAME(x) TracyCSetThreadName(x)
#define TB_PROFILE_FRAME(x) FrameMark
#define TB_PROFILE_FRAME_START(x) FrameMarkStart(x)
#define TB_PROFILE_FRAME_END(x) FrameMarkEnd(x)
#define TB_PROFILE_TAG(y, x) ZoneText(x, strlen(x))
#define TB_PROFILE_LOG(text, size) TracyMessage(text, size)
#define TB_PROFILE_VALUE(text, value) TracyPlot(text, value)
#define TB_PROFILE_ALLOC(p, size) TracyAlloc(p, size)
#define TB_PROFILE_FREE(p) TracyFree(p)

#endif
