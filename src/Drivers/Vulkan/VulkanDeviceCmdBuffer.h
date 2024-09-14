#pragma once

#include "VulkanCommon.h"

namespace Tabby {

class VulkanDeviceCmdBuffer {
public:
    VulkanDeviceCmdBuffer();
    ~VulkanDeviceCmdBuffer();

    void Destroy();

    void Begin();
    void End();
    void Reset();
    void Execute(bool wait);

    VkCommandBuffer& Raw() { return m_Buffer; }
    VkCommandPool RawPool() const { return m_Pool; }

    // DeviceCmdBufferLevel GetLevel() const { return m_Level; }
    // DeviceCmdBufferType GetBufferType() const { return m_BufferType; }
    // DeviceCmdType GetCommandType() const { return m_CmdType; }

    operator VkCommandBuffer() { return m_Buffer; }

private:
    VkCommandPool m_Pool;
    VkCommandBuffer m_Buffer;

    // DeviceCmdBufferLevel m_Level;
    // DeviceCmdBufferType m_BufferType;
    // DeviceCmdType m_CmdType;

    inline static std::shared_mutex m_SubmissionMutex;
};
}
