#include "VulkanDeviceCmdBuffer.h"
#include "VulkanGraphicsContext.h"
#include "VulkanDevice.h"

namespace Tabby {

VulkanDeviceCmdBuffer::VulkanDeviceCmdBuffer()
{
    auto device = VulkanGraphicsContext::Get()->GetDevice();

    QueueFamilyIndex queueFamilyIndices = device->GetPhysicalDevice()->GetQueueFamilyIndices();

    VkCommandPoolCreateInfo poolInfo {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphics.value();

    if (vkCreateCommandPool(device->Raw(), &poolInfo, nullptr, &m_Pool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }

    VkCommandBufferAllocateInfo allocInfo {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_Pool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(device->Raw(), &allocInfo, &m_Buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

VulkanDeviceCmdBuffer::~VulkanDeviceCmdBuffer()
{
}

void VulkanDeviceCmdBuffer::Destroy()
{
    auto device = VulkanGraphicsContext::Get()->GetDevice();
    vkDestroyCommandPool(device->Raw(), m_Pool, nullptr);
}

void VulkanDeviceCmdBuffer::Begin()
{
    // VkCommandBufferBeginInfo cmd_buffer_begin_info = {};
    // cmd_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    // cmd_buffer_begin_info.flags = m_BufferType == DeviceCmdBufferType::TRANSIENT ? VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT : 0;
    // VK_CHECK_RESULT(vkBeginCommandBuffer(m_Buffer, &cmd_buffer_begin_info));

    VkCommandBufferBeginInfo beginInfo {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VK_CHECK_RESULT(vkBeginCommandBuffer(m_Buffer, &beginInfo));
}

void VulkanDeviceCmdBuffer::End()
{
    VK_CHECK_RESULT(vkEndCommandBuffer(m_Buffer));
}

void VulkanDeviceCmdBuffer::Reset()
{
    auto device = VulkanGraphicsContext::Get()->GetDevice();
    VK_CHECK_RESULT(vkResetCommandPool(device->Raw(), m_Pool, 0));
}

void VulkanDeviceCmdBuffer::Execute(bool wait)
{
    // auto device = VulkanGraphicsContext::Get()->GetDevice();
    //
    // VkSubmitInfo submit_info = {};
    // submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    // submit_info.commandBufferCount = 1;
    // submit_info.pCommandBuffers = &m_Buffer;
    //
    // VkQueue queue = VK_NULL_HANDLE;
    // switch (m_CmdType) {
    // case DeviceCmdType::GENERAL:
    //     queue = device->GetGeneralQueue();
    //     break;
    // case DeviceCmdType::ASYNC_COMPUTE:
    //     queue = device->GetAsyncComputeQueue();
    //     break;
    // case DeviceCmdType::TRANSFER_DEDICATED:
    //     queue = device->GetGeneralQueue();
    //     break;
    // }
    //
    // VkFence fence = VK_NULL_HANDLE;
    // if (wait) {
    //     VkFenceCreateInfo fence_create_info = {};
    //     fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    //     vkCreateFence(device->Raw(), &fence_create_info, nullptr, &fence);
    // }
    //
    // m_SubmissionMutex.lock();
    // VK_CHECK_RESULT(vkQueueSubmit(queue, 1, &submit_info, fence));
    // m_SubmissionMutex.unlock();
    //
    // if (wait) {
    //     vkWaitForFences(device->Raw(), 1, &fence, VK_TRUE, UINT64_MAX);
    //     vkDestroyFence(device->Raw(), fence, nullptr);
    // }
}
}
