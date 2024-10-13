#include "Tabby/Core/Assert.h"
#include "Tabby/Core/Base.h"
#include <Drivers/Vulkan/VulkanMemoryAllocator.h>
#include <Drivers/Vulkan/VulkanGraphicsContext.h>
#include <Drivers/Vulkan/VulkanDeviceCmdBuffer.h>
#include <Drivers/Vulkan/VulkanDescriptorSet.h>
#include <Drivers/Vulkan/VulkanShaderBuffer.h>
#include <Drivers/Vulkan/VulkanSwapchain.h>
#include <Drivers/Vulkan/VulkanDevice.h>
#include <Drivers/Vulkan/VulkanShader.h>

namespace Tabby {

#pragma region converts
VkBufferUsageFlags convert(ShaderBufferUsage usage)
{
    switch (usage) {
    case ShaderBufferUsage::VERTEX_BUFFER:
        return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    case ShaderBufferUsage::INDEX_BUFFER:
        return VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    case ShaderBufferUsage::UNIFORM_BUFFER:
        return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    case ShaderBufferUsage::STORAGE_BUFFER:
        return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    case ShaderBufferUsage::STAGING_BUFFER:
        return VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    case ShaderBufferUsage::SHADER_DEVICE_ADDRESS:
        return VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
    case ShaderBufferUsage::INDIRECT_PARAMS:
        return VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
    default:
        TB_CORE_ASSERT(false);
        return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    }
}

VmaAllocationCreateFlags convert(ShaderBufferMemoryUsage usage)
{
    switch (usage) {
    case ShaderBufferMemoryUsage::READ_BACK:
        return VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
    case ShaderBufferMemoryUsage::COHERENT_WRITE:
        return VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
    case ShaderBufferMemoryUsage::NO_HOST_ACCESS:
        return 0;
    default:
        TB_CORE_ASSERT(false);
        return 0;
    }
}
#pragma endregion

VulkanShaderBuffer::VulkanShaderBuffer(const ShaderBufferSpecification& spec, Buffer data)
    : m_Buffer(VK_NULL_HANDLE)
    , m_Specification(spec)
    , m_Data(nullptr)
{
    VulkanMemoryAllocator* alloc = VulkanMemoryAllocator::Get();
    uint64_t vma_flags = convert(spec.memory_usage);

    VkBufferCreateInfo buffer_create_info = {};
    buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_create_info.size = spec.size;
    buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    buffer_create_info.usage = convert(spec.buffer_usage);

    if (m_Specification.memory_usage == ShaderBufferMemoryUsage::NO_HOST_ACCESS) {
        buffer_create_info.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    }

    if (m_Specification.buffer_usage == ShaderBufferUsage::INDIRECT_PARAMS)
        m_Specification.buffer_usage = ShaderBufferUsage::SHADER_DEVICE_ADDRESS;

    m_Allocation = alloc->AllocateBuffer(&buffer_create_info, vma_flags, &m_Buffer);

    // #if TB_DEBUG
    //     VkDebugUtilsObjectNameInfoEXT name_info = {};
    //     name_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
    //     name_info.objectType = VK_OBJECT_TYPE_BUFFER;
    //     name_info.objectHandle = (uint64_t)m_Buffer;
    //     name_info.pObjectName = spec.debug_name.c_str();
    //
    //     vkSetDebugUtilsObjectNameEXT(VulkanGraphicsContext::Get()->GetDevice()->Raw(), &name_info);
    // #endif

    if (data.Data)
        this->UploadData(0, data);
}
VulkanShaderBuffer::VulkanShaderBuffer(const ShaderBufferSpecification& spec, void* data, uint64_t data_size)
    : m_Buffer(VK_NULL_HANDLE)
    , m_Specification(spec)
    , m_Data(nullptr)
{
    VulkanMemoryAllocator* alloc = VulkanMemoryAllocator::Get();
    uint64_t vma_flags = convert(spec.memory_usage);

    VkBufferCreateInfo buffer_create_info = {};
    buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_create_info.size = spec.size;
    buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    buffer_create_info.usage = convert(spec.buffer_usage);

    if (m_Specification.memory_usage == ShaderBufferMemoryUsage::NO_HOST_ACCESS) {
        buffer_create_info.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    }

    if (m_Specification.buffer_usage == ShaderBufferUsage::INDIRECT_PARAMS)
        m_Specification.buffer_usage = ShaderBufferUsage::SHADER_DEVICE_ADDRESS;

    m_Allocation = alloc->AllocateBuffer(&buffer_create_info, vma_flags, &m_Buffer);
    this->UploadData(0, data, data_size);
}

VulkanShaderBuffer::~VulkanShaderBuffer()
{
}

void VulkanShaderBuffer::Destroy()
{
    VulkanMemoryAllocator* alloc = VulkanMemoryAllocator::Get();
    alloc->DestroyBuffer(&m_Buffer, &m_Allocation);

    if (m_Data)
        delete m_Data;
}

uint64_t VulkanShaderBuffer::GetDeviceAddress()
{
    if (m_Specification.buffer_usage != ShaderBufferUsage::SHADER_DEVICE_ADDRESS) {
        TB_CORE_ERROR("[Renderer]: failed to capture device buffer address. \
				Buffer was create without \"SHADER_DEVICE_ADDRESS\" usage, returning invalid address");
        return 0;
    }

    VkBufferDeviceAddressInfo bda_info = {};
    bda_info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
    bda_info.buffer = m_Buffer;

    return vkGetBufferDeviceAddress(VulkanGraphicsContext::Get()->GetDevice()->Raw(), &bda_info);
}

void VulkanShaderBuffer::UploadData(uint64_t offset, void* data, uint64_t data_size)
{
    if (m_Specification.memory_usage == ShaderBufferMemoryUsage::NO_HOST_ACCESS) {
        Shared<VulkanDevice> device = VulkanGraphicsContext::Get()->GetDevice();

        ShaderBufferSpecification staging_buffer_spec = {};
        staging_buffer_spec.size = data_size;
        staging_buffer_spec.buffer_usage = ShaderBufferUsage::STAGING_BUFFER;
        staging_buffer_spec.memory_usage = ShaderBufferMemoryUsage::COHERENT_WRITE;

        VulkanShaderBuffer staging_buffer(staging_buffer_spec, data, data_size);

        VkBufferCopy buffer_copy = {};
        buffer_copy.size = data_size;
        buffer_copy.srcOffset = 0;
        buffer_copy.dstOffset = 0;

        VulkanDeviceCmdBuffer cmd_buffer = device->AllocateTransientCmdBuffer();

        vkCmdCopyBuffer(cmd_buffer, staging_buffer.Raw(), m_Buffer, 1, &buffer_copy);

        VkBufferMemoryBarrier buffer_memory_barrier = {};
        buffer_memory_barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        buffer_memory_barrier.buffer = m_Buffer;
        buffer_memory_barrier.offset = 0;
        buffer_memory_barrier.size = data_size;
        buffer_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        buffer_memory_barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;

        vkCmdPipelineBarrier(
            cmd_buffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
            0,
            0,
            nullptr,
            1,
            &buffer_memory_barrier,
            0,
            nullptr);

        device->ExecuteTransientCmdBuffer(cmd_buffer);
        staging_buffer.Destroy();
    } else {
        VulkanMemoryAllocator* allocator = VulkanMemoryAllocator::Get();
        void* memory = allocator->MapMemory(m_Allocation);
        memcpy((byte*)memory + offset, data, data_size);
        allocator->UnmapMemory(m_Allocation);
        allocator->InvalidateAllocation(m_Allocation);
    }

    // Further data should only be set for non-staging buffers, since staging buffer will not be used in rendering at all
    if (m_Specification.flags & (uint64_t)ShaderBufferFlags::CREATE_STAGING_BUFFER)
        return;

    if (m_Specification.buffer_usage == ShaderBufferUsage::INDEX_BUFFER) {
        if (!m_Data)
            m_Data = new IndexBufferData;

        IndexBufferData* ibo_data = (IndexBufferData*)m_Data;
        uint8_t index_size = 0;
        if (m_Specification.flags & (uint64_t)ShaderBufferFlags::INDEX_TYPE_UINT8)
            index_size = 1;
        if (m_Specification.flags & (uint64_t)ShaderBufferFlags::INDEX_TYPE_UINT16)
            index_size = 2;
        if (m_Specification.flags & (uint64_t)ShaderBufferFlags::INDEX_TYPE_UINT32)
            index_size = 4;

        TB_CORE_ASSERT_TAGGED(index_size, "Index size was not set. Unable to evaluate index count");

        ibo_data->index_count = data_size / index_size;
        ibo_data->index_type = ExtractIndexType(m_Specification.flags);
    }
}

void VulkanShaderBuffer::UploadData(uint64_t offset, Buffer data)
{
    if (m_Specification.memory_usage == ShaderBufferMemoryUsage::NO_HOST_ACCESS) {
        Shared<VulkanDevice> device = VulkanGraphicsContext::Get()->GetDevice();

        ShaderBufferSpecification staging_buffer_spec = {};
        staging_buffer_spec.size = data.Size;
        staging_buffer_spec.buffer_usage = ShaderBufferUsage::STAGING_BUFFER;
        staging_buffer_spec.memory_usage = ShaderBufferMemoryUsage::COHERENT_WRITE;

        VulkanShaderBuffer staging_buffer(staging_buffer_spec, data);

        VkBufferCopy buffer_copy = {};
        buffer_copy.size = data.Size;
        buffer_copy.srcOffset = 0;
        buffer_copy.dstOffset = 0;

        VulkanDeviceCmdBuffer cmd_buffer = device->AllocateTransientCmdBuffer();

        vkCmdCopyBuffer(cmd_buffer, staging_buffer.Raw(), m_Buffer, 1, &buffer_copy);

        VkBufferMemoryBarrier buffer_memory_barrier = {};
        buffer_memory_barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        buffer_memory_barrier.buffer = m_Buffer;
        buffer_memory_barrier.offset = 0;
        buffer_memory_barrier.size = data.Size;
        buffer_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        buffer_memory_barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;

        vkCmdPipelineBarrier(
            cmd_buffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
            0,
            0,
            nullptr,
            1,
            &buffer_memory_barrier,
            0,
            nullptr);

        device->ExecuteTransientCmdBuffer(cmd_buffer);
        staging_buffer.Destroy();
    } else {
        VulkanMemoryAllocator* allocator = VulkanMemoryAllocator::Get();
        void* memory = allocator->MapMemory(m_Allocation);
        memcpy((byte*)memory + offset, data.Data, data.Size);
        allocator->UnmapMemory(m_Allocation);
        allocator->InvalidateAllocation(m_Allocation);
    }

    // Further data should only be set for non-staging buffers, since staging buffer will not be used in rendering at all
    if (m_Specification.flags & (uint64_t)ShaderBufferFlags::CREATE_STAGING_BUFFER)
        return;

    if (m_Specification.buffer_usage == ShaderBufferUsage::INDEX_BUFFER) {
        if (!m_Data)
            m_Data = new IndexBufferData;

        IndexBufferData* ibo_data = (IndexBufferData*)m_Data;
        uint8_t index_size = 0;
        if (m_Specification.flags & (uint64_t)ShaderBufferFlags::INDEX_TYPE_UINT8)
            index_size = 1;
        if (m_Specification.flags & (uint64_t)ShaderBufferFlags::INDEX_TYPE_UINT16)
            index_size = 2;
        if (m_Specification.flags & (uint64_t)ShaderBufferFlags::INDEX_TYPE_UINT32)
            index_size = 4;

        TB_CORE_ASSERT_TAGGED(index_size, "Index size was not set. Unable to evaluate index count");

        ibo_data->index_count = data.Size / index_size;
        ibo_data->index_type = ExtractIndexType(m_Specification.flags);
    }
}

void VulkanShaderBuffer::CopyRegionTo(Shared<VulkanDeviceCmdBuffer> cmd_buffer, Shared<ShaderBuffer> dst_buffer, uint64_t src_offset, uint64_t dst_offset, uint64_t size)
{
    Shared<VulkanDeviceCmdBuffer> vk_cmd_buffer = ShareAs<VulkanDeviceCmdBuffer>(cmd_buffer);
    Shared<VulkanShaderBuffer> vk_buffer = ShareAs<VulkanShaderBuffer>(dst_buffer);

    VkBufferCopy params = {};
    params.srcOffset = src_offset;
    params.dstOffset = dst_offset;
    params.size = size;

    vkCmdCopyBuffer(*vk_cmd_buffer, m_Buffer, vk_buffer->Raw(), 1, &params);
}

void VulkanShaderBuffer::Clear(Shared<VulkanDeviceCmdBuffer> cmd_buffer, uint64_t offset, uint64_t size, uint32_t value)
{
    Shared<VulkanDeviceCmdBuffer> device_cmd_buffer = ShareAs<VulkanDeviceCmdBuffer>(cmd_buffer);
    vkCmdFillBuffer(device_cmd_buffer->Raw(), m_Buffer, offset, size, value);
}
}
