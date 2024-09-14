#pragma once

#include <Tabby/Renderer/ShaderBuffer.h>

#include "VulkanCommon.h"

#include <vk_mem_alloc.h>

namespace Tabby {

struct IndexBufferData {
    uint32_t index_count;
    VkIndexType index_type;
};

constexpr VkIndexType ExtractIndexType(BitMask buffer_flags)
{
    if (buffer_flags & (uint64_t)ShaderBufferFlags::INDEX_TYPE_UINT8)
        return VK_INDEX_TYPE_UINT8_EXT;
    if (buffer_flags & (uint64_t)ShaderBufferFlags::INDEX_TYPE_UINT16)
        return VK_INDEX_TYPE_UINT16;
    if (buffer_flags & (uint64_t)ShaderBufferFlags::INDEX_TYPE_UINT32)
        return VK_INDEX_TYPE_UINT32;

    return VK_INDEX_TYPE_MAX_ENUM;
}

class VulkanDeviceCmdBuffer;

class VulkanShaderBuffer : public ShaderBuffer {
public:
    VulkanShaderBuffer(const ShaderBufferSpecification& spec, Buffer data = Buffer());
    ~VulkanShaderBuffer();

    void Destroy() override;

    VkBuffer Raw() const { return m_Buffer; }

    ShaderBufferSpecification GetSpecification() const override { return m_Specification; }
    uint64_t GetDeviceAddress();
    uint64_t GetPerFrameSize();
    uint64_t GetFrameOffset();
    // uint64_t GetPerFrameSize() { return m_Specification.size / VulkanGraphicsContext::GetFramesInFlight(); }
    // uint64_t GetFrameOffset() { return VulkanGraphicsContext::Get()->GetSwapchain()->GetCurrentFrameIndex() * GetPerFrameSize(); }
    VmaAllocation RawAllocation() const { return m_Allocation; }
    void* GetAdditionalData() const { return m_Data; }

    void UploadData(uint64_t offset, Buffer data) override;
    void CopyRegionTo(Shared<VulkanDeviceCmdBuffer> cmd_buffer, Shared<ShaderBuffer> dst_buffer, uint64_t src_offset, uint64_t dst_offset, uint64_t size);
    void Clear(Shared<VulkanDeviceCmdBuffer> cmd_buffer, uint64_t offset, uint64_t size, uint32_t value);

private:
    VkBuffer m_Buffer;
    VmaAllocation m_Allocation;

    ShaderBufferSpecification m_Specification;
    void* m_Data; // VBOs, IBOs and other buffers can have different data: vertex count, index count;
};

}
