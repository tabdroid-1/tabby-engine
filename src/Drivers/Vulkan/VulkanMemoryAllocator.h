#pragma once

#include "VulkanCommon.h"

#include <vk_mem_alloc.h>

namespace Tabby {

struct VulkanAllocatorStatistics {
    std::vector<VmaAllocation> allocations;
    uint64_t allocated;
    uint64_t freed;
    uint64_t currently_allocated;
};

class VulkanMemoryAllocator {
public:
    static void Init();
    static void Destroy();
    static VulkanMemoryAllocator* Get() { return s_Instance; }

    void InvalidateAllocation(VmaAllocation allocation, uint64_t size = VK_WHOLE_SIZE, uint64_t offset = 0);

    VmaAllocation AllocateBuffer(VkBufferCreateInfo* create_info, uint32_t flags, VkBuffer* buffer);
    VmaAllocation AllocateImage(VkImageCreateInfo* create_info, uint32_t flags, VkImage* image);

    void DestroyBuffer(VkBuffer* buffer, VmaAllocation* allocation);
    void DestroyImage(VkImage* image, VmaAllocation* allocation);

    void* MapMemory(VmaAllocation allocation);
    void UnmapMemory(VmaAllocation allocation);

    VmaAllocator Raw() { return m_Allocator; }
    VulkanAllocatorStatistics GetStats() const { return m_Statistics; }

private:
    VulkanMemoryAllocator();
    ~VulkanMemoryAllocator();

private:
    static VulkanMemoryAllocator* s_Instance;

    VmaAllocator m_Allocator;

    VulkanAllocatorStatistics m_Statistics;

    std::shared_mutex m_Mutex;
};

}
