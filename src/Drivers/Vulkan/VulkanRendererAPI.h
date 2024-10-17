#pragma once

#include "Drivers/Vulkan/VulkanDescriptorSet.h"
#include "Tabby/Renderer/Image.h"
#include <Drivers/Vulkan/VulkanCommon.h>
#include <Tabby/Foundation/Types.h>
#include <Tabby/Renderer/RendererAPI.h>
#include <Tabby/Renderer/Renderer.h>
#include <vulkan/vulkan_core.h>

namespace Tabby {

class VulkanGraphicsContext;
class VulkanDeviceCmdBuffer;
class VulkanShaderBuffer;
class VulkanSwapchain;
class VulkanDevice;
class VulkanShader;
class Material;
class Mesh;

class VulkanRendererAPI : public RendererAPI {
public:
    VulkanRendererAPI(const RendererConfig& config);
    ~VulkanRendererAPI() override;

    RendererConfig GetConfig() const override { return m_Config; }
    uint32_t GetCurrentFrameIndex() const override;
    // uint32 GetDeviceMinimalUniformBufferAlignment() const override;
    // uint32 GetDeviceMinimalStorageBufferAlignment() const override;
    // uint32 GetDeviceOptimalTaskWorkGroupSize() const override;
    // uint32 GetDeviceOptimalMeshWorkGroupSize() const override;
    // uint32 GetDeviceOptimalComputeWorkGroupSize() const override;
    // Shared<DeviceCmdBuffer> GetCmdBuffer() override { return m_CurrentCmdBuffer; };
    // Shared<Swapchain> GetSwapchain() override { return m_Swapchain; };
    //
    void BeginFrame() override;
    void EndFrame() override;
    void BeginRender(const std::vector<Shared<Image>> attachments, UIntVector3 render_area, IntVector2 render_offset, Vector4 clear_color) override;
    void EndRender(Shared<Image> target) override;
    void WaitDevice() override;
    // void BindSet(Shared<DescriptorSet> set, Shared<Pipeline> pipeline, uint8 index) override;
    // void CopyToSwapchain(Shared<Image> image) override;
    // void InsertBarrier(const PipelineBarrierInfo& barrier) override;

    void BeginCommandRecord() override;
    void EndCommandRecord() override;
    void ExecuteCurrentCommands() override;

    void ClearImage(Shared<Image> image, const Vector4& value) override;
    void RenderTasks(Shared<Mesh> mesh, std::vector<MaterialData> elements, Buffer data = Buffer()) override;
    void RenderTasks(Shared<Shader> shader, uint32_t vertex_count, Buffer data = Buffer()) override;
    // void RenderQuad(Shared<Pipeline> pipeline, MiscData data) override;
    // void RenderQuad(Shared<Pipeline> pipeline, uint32 amount, MiscData data) override;
    // void DispatchCompute(Shared<Pipeline> pipeline, const glm::uvec3& dimensions, MiscData data) override;
    // void RenderUnindexed(Shared<Pipeline> pipeline, Shared<DeviceBuffer> vertex_buffer, MiscData data) override;

    void Render() override;
    void RenderImGui() override;

    static std::vector<VkDescriptorSet> AllocateDescriptorSets(VkDescriptorSetLayout layout, uint32_t count);
    static void FreeDescriptorSets(std::vector<VkDescriptorSet> sets);

private:
    inline static PFN_vkCmdBeginRenderingKHR vkCmdBeginRenderingKHR;
    inline static PFN_vkCmdEndRenderingKHR vkCmdEndRenderingKHR;

    RendererConfig m_Config;

    Shared<VulkanGraphicsContext> m_GraphicsContext;
    Shared<VulkanDevice> m_Device;
    Shared<VulkanSwapchain> m_Swapchain;

    std::vector<Shared<VulkanDeviceCmdBuffer>> m_CmdBuffers;
    Shared<VulkanDeviceCmdBuffer> m_CurrentCmdBuffer;
    std::shared_mutex m_Mutex;

    inline static VkDescriptorPool s_DescriptorPool = VK_NULL_HANDLE;
};

}
