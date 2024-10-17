#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

#include "VulkanMemoryAllocator.h"
#include "VulkanGraphicsContext.h"
#include "VulkanDevice.h"

#include <Tabby/Utils/Utils.h>

#define TB_TRACE_DEVICE_ALLOCATIONS 1

namespace Tabby {

VulkanMemoryAllocator* VulkanMemoryAllocator::s_Instance = nullptr;

VulkanMemoryAllocator::VulkanMemoryAllocator()
{
    VulkanGraphicsContext* vk_context = VulkanGraphicsContext::Get();

    VmaVulkanFunctions vulkan_functions = {};
    vulkan_functions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
    vulkan_functions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;

    VmaAllocatorCreateInfo allocator_create_info = {};
    allocator_create_info.vulkanApiVersion = VK_API_VERSION_1_2;
    allocator_create_info.instance = vk_context->GetVulkanInstance();
    allocator_create_info.physicalDevice = vk_context->GetDevice()->GetPhysicalDevice()->Raw();
    allocator_create_info.device = vk_context->GetDevice()->Raw();
    allocator_create_info.pVulkanFunctions = &vulkan_functions;
    // allocator_create_info.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

    vmaCreateAllocator(&allocator_create_info, &m_Allocator);

    m_Statistics = { 0, 0, 0 };
}

VulkanMemoryAllocator::~VulkanMemoryAllocator()
{
    TB_CORE_TRACE("Destroying vulkan memory allocator: ");
    TB_CORE_TRACE("\tTotal memory allocated: {0}", Utils::FormatAllocationSize(m_Statistics.allocated));
    TB_CORE_TRACE("\tTotal memory freed: {0}", Utils::FormatAllocationSize(m_Statistics.freed));
    TB_CORE_TRACE("\tIn use at the moment: {0}", Utils::FormatAllocationSize(m_Statistics.currently_allocated));
    vmaDestroyAllocator(m_Allocator);
}

void VulkanMemoryAllocator::Init()
{
    s_Instance = new VulkanMemoryAllocator;
}

void VulkanMemoryAllocator::Destroy()
{
    delete s_Instance;
}

void VulkanMemoryAllocator::InvalidateAllocation(VmaAllocation allocation, uint64_t size /*= VK_WHOLE_SIZE*/, uint64_t offset /*= 0*/)
{
    std::lock_guard lock(m_Mutex);
    vmaInvalidateAllocation(m_Allocator, allocation, offset, size);
}

VmaAllocation VulkanMemoryAllocator::AllocateBuffer(VkBufferCreateInfo* create_info, uint32_t flags, VkBuffer* buffer)
{
    std::lock_guard lock(m_Mutex);
    VmaAllocationCreateInfo allocation_create_info = {};
    allocation_create_info.usage = VMA_MEMORY_USAGE_AUTO;
    allocation_create_info.flags = flags;

    VmaAllocation allocation;
    VmaAllocationInfo allocation_info;

    VK_CHECK_RESULT(vmaCreateBuffer(m_Allocator, create_info, &allocation_create_info, buffer, &allocation, &allocation_info));

    m_Statistics.allocated += allocation_info.size;
    m_Statistics.currently_allocated += allocation_info.size;

    if (TB_TRACE_DEVICE_ALLOCATIONS) {
        TB_CORE_TRACE("Allocating device buffer:");
        TB_CORE_TRACE("\tRequested size: {0}", Utils::FormatAllocationSize(create_info->size));
        TB_CORE_TRACE("\tAllocated size: {0}", Utils::FormatAllocationSize(allocation_info.size));
        TB_CORE_TRACE("\tCurrently allocated: {0}", Utils::FormatAllocationSize(m_Statistics.currently_allocated));
    }
    return allocation;
}

VmaAllocation VulkanMemoryAllocator::AllocateImage(VkImageCreateInfo* create_info, uint32_t flags, VkImage* image)
{
    std::lock_guard lock(m_Mutex);
    if (create_info->extent.depth == 0)
        TB_CORE_WARN("Trying to allocate image with 0 depth. No allocation done");

    VmaAllocationCreateInfo allocation_create_info = {};
    allocation_create_info.usage = VMA_MEMORY_USAGE_AUTO;
    allocation_create_info.flags = flags;

    VmaAllocation allocation;
    VmaAllocationInfo allocation_info;

    VK_CHECK_RESULT(vmaCreateImage(m_Allocator, create_info, &allocation_create_info, image, &allocation, &allocation_info));
    m_Statistics.allocated += allocation_info.size;
    m_Statistics.currently_allocated += allocation_info.size;

    if (TB_TRACE_DEVICE_ALLOCATIONS) {
        TB_CORE_TRACE("Allocating device image:");
        TB_CORE_TRACE("\tAllocated size: {0}", Utils::FormatAllocationSize(allocation_info.size));
        TB_CORE_TRACE("\tCurrently allocated: {0}", Utils::FormatAllocationSize(m_Statistics.currently_allocated));
    }
    return allocation;
}

void VulkanMemoryAllocator::DestroyBuffer(VkBuffer* buffer, VmaAllocation* allocation)
{
    std::lock_guard lock(m_Mutex);
    VmaAllocationInfo allocation_info;
    vmaGetAllocationInfo(m_Allocator, *allocation, &allocation_info);

    vmaDestroyBuffer(m_Allocator, *buffer, *allocation);

    m_Statistics.freed += allocation_info.size;
    m_Statistics.currently_allocated -= allocation_info.size;

    if (TB_TRACE_DEVICE_ALLOCATIONS) {
        TB_CORE_TRACE("Destroying device buffer:");
        TB_CORE_TRACE("\tFreed memory: {0}", Utils::FormatAllocationSize(allocation_info.size));
        TB_CORE_TRACE("\tCurrently allocated: {0}", Utils::FormatAllocationSize(m_Statistics.currently_allocated));
    }

    *buffer = VK_NULL_HANDLE;
    *allocation = VK_NULL_HANDLE;
}

void VulkanMemoryAllocator::DestroyImage(VkImage* image, VmaAllocation* allocation)
{
    std::lock_guard lock(m_Mutex);
    VmaAllocationInfo allocation_info;
    vmaGetAllocationInfo(m_Allocator, *allocation, &allocation_info);

    vmaDestroyImage(m_Allocator, *image, *allocation);

    m_Statistics.freed += allocation_info.size;
    m_Statistics.currently_allocated -= allocation_info.size;

    if (TB_TRACE_DEVICE_ALLOCATIONS) {
        TB_CORE_TRACE("Destroying device image:");
        TB_CORE_TRACE("\tFreed memory: {0}", Utils::FormatAllocationSize(allocation_info.size));
        TB_CORE_TRACE("\tCurrently allocated: {0}", Utils::FormatAllocationSize(m_Statistics.currently_allocated));
    }

    *image = VK_NULL_HANDLE;
    *allocation = VK_NULL_HANDLE;
}

void* VulkanMemoryAllocator::MapMemory(VmaAllocation allocation)
{
    std::lock_guard lock(m_Mutex);
    void* mapped_memory;
    VK_CHECK_RESULT(vmaMapMemory(m_Allocator, allocation, &mapped_memory));
    return mapped_memory;
}

void VulkanMemoryAllocator::UnmapMemory(VmaAllocation allocation)
{
    std::lock_guard lock(m_Mutex);
    vmaUnmapMemory(m_Allocator, allocation);
}

}
