#pragma once

#include <Drivers/Vulkan/VulkanCommon.h>

namespace Tabby {

enum class DeviceCmdBufferType {
    GENERAL,
    TRANSIENT
};

enum class DeviceCmdType {
    GENERAL,
    ASYNC_COMPUTE,
    TRANSFER_DEDICATED
};

enum class DeviceCmdBufferLevel {
    PRIMARY,
    SECONDARY
};

class VulkanDeviceCmdBuffer {
public:
    VulkanDeviceCmdBuffer(DeviceCmdBufferLevel level, DeviceCmdBufferType buffer_type, DeviceCmdType cmd_type);
    ~VulkanDeviceCmdBuffer();

    void Destroy();

    void Begin();
    void End();
    void Reset();
    void Execute(bool wait);

    VkCommandBuffer Raw() const { return m_Buffer; }
    VkCommandPool RawPool() const { return m_Pool; }

    DeviceCmdBufferLevel GetLevel() const { return m_Level; }
    DeviceCmdBufferType GetBufferType() const { return m_BufferType; }
    DeviceCmdType GetCommandType() const { return m_CmdType; }

    operator VkCommandBuffer() { return m_Buffer; }

private:
    VkCommandPool m_Pool;
    VkCommandBuffer m_Buffer;

    DeviceCmdBufferLevel m_Level;
    DeviceCmdBufferType m_BufferType;
    DeviceCmdType m_CmdType;

    inline static std::shared_mutex m_SubmissionMutex;
};

}
