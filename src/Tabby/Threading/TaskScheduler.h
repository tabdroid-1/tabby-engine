#pragma once

#include <TaskScheduler.h>

namespace Tabby {

constexpr int32_t MaxTasks = 64;
constexpr int32_t MaxThreads = 64;

class TaskScheduler {
public:
    static enki::TaskScheduler& GetScheduler() { return m_Scheduler; }
    static void AddTaskSet(enki::ITaskSet* task)
    {
        m_Scheduler.AddTaskSetToPipe(task);
        m_Tasks.push_back(task);
    }

    static void WaitForTask(enki::ITaskSet* task)
    {
        m_Scheduler.WaitforTask(task);
    }

    static int GetNumHardwareThreads()
    {
        return enki::GetNumHardwareThreads();
    }

private:
    inline static std::vector<enki::ITaskSet*> m_Tasks;
    inline static enki::TaskScheduler m_Scheduler;
    inline static int32_t m_TaskCount;
    inline static int m_ThreadCount;
};

}
