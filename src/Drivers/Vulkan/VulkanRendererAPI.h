#pragma once

#include <Tabby/Renderer/RendererAPI.h>
#include <Tabby/Renderer/Renderer.h>

namespace Tabby {

class VulkanGraphicsContext;
class VulkanDeviceCmdBuffer;
class VulkanShaderBuffer;
class VulkanRenderPass;
class VulkanSwapchain;
class VulkanDevice;
class VulkanShader;

class VulkanRendererAPI : public RendererAPI {
public:
    VulkanRendererAPI(const RendererConfig& config);
    ~VulkanRendererAPI() override;

    RendererConfig GetConfig() const override { return m_Config; }
    // uint32 GetCurrentFrameIndex() const override { return m_Swapchain->GetCurrentFrameIndex(); }
    // uint32 GetDeviceMinimalUniformBufferAlignment() const override;
    // uint32 GetDeviceMinimalStorageBufferAlignment() const override;
    // uint32 GetDeviceOptimalTaskWorkGroupSize() const override;
    // uint32 GetDeviceOptimalMeshWorkGroupSize() const override;
    // uint32 GetDeviceOptimalComputeWorkGroupSize() const override;
    // Shared<DeviceCmdBuffer> GetCmdBuffer() override { return m_CurrentCmdBuffer; };
    // Shared<Swapchain> GetSwapchain() override { return m_Swapchain; };
    //
    // void BeginFrame() override;
    // void EndFrame() override;
    // void BeginRender(const std::vector<Shared<Image>> attachments, UIntVector3 render_area, IntVector2 render_offset, Vector4 clear_color) override;
    // void EndRender(Shared<Image> target) override;
    void WaitDevice() override;
    // void BindSet(Shared<DescriptorSet> set, Shared<Pipeline> pipeline, uint8 index) override;
    // void CopyToSwapchain(Shared<Image> image) override;
    // void InsertBarrier(const PipelineBarrierInfo& barrier) override;
    //
    // void ClearImage(Shared<Image> image, const fvec4& value) override;
    // void RenderMeshTasks(Shared<Pipeline> pipeline, const glm::uvec3& dimensions, MiscData data) override;
    // void RenderMeshTasksIndirect(Shared<Pipeline> pipeline, Shared<DeviceBuffer> params, MiscData data) override;
    // void RenderQuad(Shared<Pipeline> pipeline, MiscData data) override;
    // void RenderQuad(Shared<Pipeline> pipeline, uint32 amount, MiscData data) override;
    // void DispatchCompute(Shared<Pipeline> pipeline, const glm::uvec3& dimensions, MiscData data) override;
    // void RenderUnindexed(Shared<Pipeline> pipeline, Shared<DeviceBuffer> vertex_buffer, MiscData data) override;
    //
    void Render() override;
    void RenderImGui() override;

    // static std::vector<VkDescriptorSet> AllocateDescriptorSets(VkDescriptorSetLayout layout, uint32 count);
    // static void FreeDescriptorSets(std::vector<VkDescriptorSet> sets);

private:
    RendererConfig m_Config;

    Shared<VulkanGraphicsContext> m_GraphicsContext;
    Shared<VulkanDevice> m_Device;
    Shared<VulkanSwapchain> m_Swapchain;
    Shared<VulkanRenderPass> m_RenderPass;
    // NOTE: TEMP
    Shared<VulkanShader> m_Shader;
    Shared<VulkanShaderBuffer> m_VertexBuffer;
    Shared<VulkanShaderBuffer> m_IndexBuffer;

    std::vector<Shared<VulkanDeviceCmdBuffer>> m_CmdBuffers;
    Shared<VulkanDeviceCmdBuffer> m_CurrentCmdBuffer;

    // static VkDescriptorPool s_DescriptorPool;

    std::shared_mutex m_Mutex;
};

}
