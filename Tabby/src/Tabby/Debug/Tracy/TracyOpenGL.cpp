#include "Tabby/Debug/Tracy/TracyOpenGL.hpp"
#include "Drivers/gl33/GL33.h"
#include "glad/gl33.h"

tracy::GpuCtx::GpuCtx()
    : m_context(GetGpuCtxCounter().fetch_add(1, std::memory_order_relaxed))
    , m_head(0)
    , m_tail(0)
{

    assert(m_context != 255);

    Tabby::GL33::GL()->GenQueries(QueryCount, m_query);

    int64_t tgpu;
    Tabby::GL33::GL()->GetInteger64v(GL_TIMESTAMP, &tgpu);
    int64_t tcpu = Profiler::GetTime();

    GLint bits;
    Tabby::GL33::GL()->GetQueryiv(GL_TIMESTAMP, GL_QUERY_COUNTER_BITS, &bits);

    const float period = 1.f;
    const auto thread = GetThreadHandle();
    TracyLfqPrepare(QueueType::GpuNewContext);
    MemWrite(&item->gpuNewContext.cpuTime, tcpu);
    MemWrite(&item->gpuNewContext.gpuTime, tgpu);
    MemWrite(&item->gpuNewContext.thread, thread);
    MemWrite(&item->gpuNewContext.period, period);
    MemWrite(&item->gpuNewContext.context, m_context);
    MemWrite(&item->gpuNewContext.flags, uint8_t(0));
    MemWrite(&item->gpuNewContext.type, GpuContextType::OpenGl);

#ifdef TRACY_ON_DEMAND
    GetProfiler().DeferItem(*item);
#endif

    TracyLfqCommit;
}

void tracy::GpuCtx::Name(const char* name, uint16_t len)
{
    auto ptr = (char*)tracy_malloc(len);
    memcpy(ptr, name, len);

    TracyLfqPrepare(QueueType::GpuContextName);
    MemWrite(&item->gpuContextNameFat.context, m_context);
    MemWrite(&item->gpuContextNameFat.ptr, (uint64_t)ptr);
    MemWrite(&item->gpuContextNameFat.size, len);
#ifdef TRACY_ON_DEMAND
    GetProfiler().DeferItem(*item);
#endif
    TracyLfqCommit;
}

void tracy::GpuCtx::Collect()
{
    ZoneScopedC(Color::Red4);

    if (m_tail == m_head)
        return;

#ifdef TRACY_ON_DEMAND
    if (!GetProfiler().IsConnected()) {
        m_head = m_tail = 0;
        return;
    }
#endif

    while (m_tail != m_head) {
        GLint available;
        Tabby::GL33::GL()->GetQueryObjectiv(m_query[m_tail], GL_QUERY_RESULT_AVAILABLE, &available);
        if (!available)
            return;

        uint64_t time;
        Tabby::GL33::GL()->GetQueryObjectui64v(m_query[m_tail], GL_QUERY_RESULT, &time);

        TracyLfqPrepare(QueueType::GpuTime);
        MemWrite(&item->gpuTime.gpuTime, (int64_t)time);
        MemWrite(&item->gpuTime.queryId, (uint16_t)m_tail);
        MemWrite(&item->gpuTime.context, m_context);
        TracyLfqCommit;

        m_tail = (m_tail + 1) % QueryCount;
    }
}

tracy::GpuCtxScope::GpuCtxScope(const SourceLocationData* srcloc, bool is_active)
#ifdef TRACY_ON_DEMAND
    : m_active(is_active && GetProfiler().IsConnected())
#else
    : m_active(is_active)
#endif
{
    if (!m_active)
        return;

    const auto queryId = GetGpuCtx().ptr->NextQueryId();
    Tabby::GL33::GL()->QueryCounter(GetGpuCtx().ptr->TranslateOpenGlQueryId(queryId), GL_TIMESTAMP);

    TracyLfqPrepare(QueueType::GpuZoneBegin);
    MemWrite(&item->gpuZoneBegin.cpuTime, Profiler::GetTime());
    memset(&item->gpuZoneBegin.thread, 0, sizeof(item->gpuZoneBegin.thread));
    MemWrite(&item->gpuZoneBegin.queryId, uint16_t(queryId));
    MemWrite(&item->gpuZoneBegin.context, GetGpuCtx().ptr->GetId());
    MemWrite(&item->gpuZoneBegin.srcloc, (uint64_t)srcloc);
    TracyLfqCommit;
}

tracy::GpuCtxScope::GpuCtxScope(const SourceLocationData* srcloc, int depth, bool is_active)
#ifdef TRACY_ON_DEMAND
    : m_active(is_active && GetProfiler().IsConnected())
#else
    : m_active(is_active)
#endif
{
    if (!m_active)
        return;

    const auto queryId = GetGpuCtx().ptr->NextQueryId();
    Tabby::GL33::GL()->QueryCounter(GetGpuCtx().ptr->TranslateOpenGlQueryId(queryId), GL_TIMESTAMP);

#ifdef TRACY_FIBERS
    TracyLfqPrepare(QueueType::GpuZoneBegin);
    memset(&item->gpuZoneBegin.thread, 0, sizeof(item->gpuZoneBegin.thread));
#else
    GetProfiler().SendCallstack(depth);
    TracyLfqPrepare(QueueType::GpuZoneBeginCallstack);
    MemWrite(&item->gpuZoneBegin.thread, GetThreadHandle());
#endif
    MemWrite(&item->gpuZoneBegin.cpuTime, Profiler::GetTime());
    MemWrite(&item->gpuZoneBegin.queryId, uint16_t(queryId));
    MemWrite(&item->gpuZoneBegin.context, GetGpuCtx().ptr->GetId());
    MemWrite(&item->gpuZoneBegin.srcloc, (uint64_t)srcloc);
    TracyLfqCommit;
}

tracy::GpuCtxScope::GpuCtxScope(uint32_t line, const char* source, size_t sourceSz, const char* function, size_t functionSz, const char* name, size_t nameSz, bool is_active)
#ifdef TRACY_ON_DEMAND
    : m_active(is_active && GetProfiler().IsConnected())
#else
    : m_active(is_active)
#endif
{
    if (!m_active)
        return;

    const auto queryId = GetGpuCtx().ptr->NextQueryId();
    Tabby::GL33::GL()->QueryCounter(GetGpuCtx().ptr->TranslateOpenGlQueryId(queryId), GL_TIMESTAMP);

    TracyLfqPrepare(QueueType::GpuZoneBeginAllocSrcLoc);
    const auto srcloc = Profiler::AllocSourceLocation(line, source, sourceSz, function, functionSz, name, nameSz);
    MemWrite(&item->gpuZoneBegin.cpuTime, Profiler::GetTime());
    memset(&item->gpuZoneBegin.thread, 0, sizeof(item->gpuZoneBegin.thread));
    MemWrite(&item->gpuZoneBegin.queryId, uint16_t(queryId));
    MemWrite(&item->gpuZoneBegin.context, GetGpuCtx().ptr->GetId());
    MemWrite(&item->gpuZoneBegin.srcloc, (uint64_t)srcloc);
    TracyLfqCommit;
}

tracy::GpuCtxScope::GpuCtxScope(uint32_t line, const char* source, size_t sourceSz, const char* function, size_t functionSz, const char* name, size_t nameSz, int depth, bool is_active)
#ifdef TRACY_ON_DEMAND
    : m_active(is_active && GetProfiler().IsConnected())
#else
    : m_active(is_active)
#endif
{
    if (!m_active)
        return;

    const auto queryId = GetGpuCtx().ptr->NextQueryId();
    Tabby::GL33::GL()->QueryCounter(GetGpuCtx().ptr->TranslateOpenGlQueryId(queryId), GL_TIMESTAMP);

#ifdef TRACY_FIBERS
    TracyLfqPrepare(QueueType::GpuZoneBeginAllocSrcLoc);
    memset(&item->gpuZoneBegin.thread, 0, sizeof(item->gpuZoneBegin.thread));
#else
    GetProfiler().SendCallstack(depth);
    TracyLfqPrepare(QueueType::GpuZoneBeginAllocSrcLocCallstack);
    MemWrite(&item->gpuZoneBegin.thread, GetThreadHandle());
#endif
    const auto srcloc = Profiler::AllocSourceLocation(line, source, sourceSz, function, functionSz, name, nameSz);
    MemWrite(&item->gpuZoneBegin.cpuTime, Profiler::GetTime());
    MemWrite(&item->gpuZoneBegin.queryId, uint16_t(queryId));
    MemWrite(&item->gpuZoneBegin.context, GetGpuCtx().ptr->GetId());
    MemWrite(&item->gpuZoneBegin.srcloc, (uint64_t)srcloc);
    TracyLfqCommit;
}

tracy::GpuCtxScope::~GpuCtxScope()
{
    if (!m_active)
        return;

    const auto queryId = GetGpuCtx().ptr->NextQueryId();
    Tabby::GL33::GL()->QueryCounter(GetGpuCtx().ptr->TranslateOpenGlQueryId(queryId), GL_TIMESTAMP);

    TracyLfqPrepare(QueueType::GpuZoneEnd);
    MemWrite(&item->gpuZoneEnd.cpuTime, Profiler::GetTime());
    memset(&item->gpuZoneEnd.thread, 0, sizeof(item->gpuZoneEnd.thread));
    MemWrite(&item->gpuZoneEnd.queryId, uint16_t(queryId));
    MemWrite(&item->gpuZoneEnd.context, GetGpuCtx().ptr->GetId());
    TracyLfqCommit;
}
