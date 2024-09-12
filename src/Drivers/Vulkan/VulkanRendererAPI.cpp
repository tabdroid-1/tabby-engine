#include <Drivers/Vulkan/VulkanGraphicsContext.h>
#include <Drivers/Vulkan/VulkanDeviceCmdBuffer.h>
#include <Drivers/Vulkan/VulkanRendererAPI.h>
#include <Drivers/Vulkan/VulkanRenderPass.h>
#include <Drivers/Vulkan/VulkanSwapchain.h>
#include <Drivers/Vulkan/VulkanShader.h>
#include <Drivers/Vulkan/VulkanDevice.h>

#include <Tabby/Renderer/ShaderLibrary.h>

#include <imgui.h>

#include "backends/imgui_impl_vulkan.h"

namespace Tabby {

static std::vector<uint8_t> readFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<uint8_t> buffer(fileSize);

    file.seekg(0);
    file.read(reinterpret_cast<char*>(buffer.data()), fileSize);

    file.close();

    return buffer;
}

VulkanRendererAPI::VulkanRendererAPI(const RendererConfig& config)
    : m_Config(config)
{
    m_GraphicsContext = std::make_shared<VulkanGraphicsContext>();
    m_Device = m_GraphicsContext->GetDevice();

    m_Swapchain = m_GraphicsContext->GetSwapchain();
    m_RenderPass = m_GraphicsContext->GetRenderPass();

    m_CmdBuffers.resize(m_GraphicsContext->GetFramesInFlight());

    for (auto& buf : m_CmdBuffers) {
        buf = std::make_shared<VulkanDeviceCmdBuffer>();
    }

    ShaderSpecification shader_spec;
    shader_spec = ShaderSpecification::Default();

    ShaderBufferLayoutElement element("inPosition", ShaderDataType::FLOAT2);
    ShaderBufferLayoutElement element2("inColor", ShaderDataType::FLOAT3);
    ShaderBufferLayout buffer_layout(std::vector { element, element2 });
    shader_spec.input_layout = buffer_layout;

    ShaderLibrary::LoadShader(shader_spec, "shaders/vulkan/test.glsl");
    m_Shader = ShareAs<VulkanShader>(ShaderLibrary::GetShader("test.glsl"));
}

VulkanRendererAPI::~VulkanRendererAPI()
{
    vkDeviceWaitIdle(m_Device->Raw());
    for (auto& cmd_buffer : m_CmdBuffers)
        cmd_buffer->Destroy();
}

void VulkanRendererAPI::Render()
{

    m_GraphicsContext->GetSwapchain()->BeginFrame();
    m_CmdBuffers[m_GraphicsContext->GetSwapchain()->GetCurrentFrameIndex()]->Reset();
    m_CmdBuffers[m_GraphicsContext->GetSwapchain()->GetCurrentFrameIndex()]->Begin();
    m_GraphicsContext->GetRenderPass()->Begin(m_CmdBuffers[m_GraphicsContext->GetSwapchain()->GetCurrentFrameIndex()]->Raw());

    vkCmdBindPipeline(m_CmdBuffers[m_GraphicsContext->GetSwapchain()->GetCurrentFrameIndex()]->Raw(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_Shader->RawPipeline());

    vkCmdDraw(m_CmdBuffers[m_GraphicsContext->GetSwapchain()->GetCurrentFrameIndex()]->Raw(), 3, 1, 0, 0);

    m_GraphicsContext->GetRenderPass()->End(m_CmdBuffers[m_GraphicsContext->GetSwapchain()->GetCurrentFrameIndex()]->Raw());
    m_CmdBuffers[m_GraphicsContext->GetSwapchain()->GetCurrentFrameIndex()]->End();

    VkSubmitInfo submitInfo {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { m_GraphicsContext->GetSwapchain()->GetSemaphores().present_complete };
    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_CmdBuffers[m_GraphicsContext->GetSwapchain()->GetCurrentFrameIndex()]->Raw();

    VkSemaphore signalSemaphores[] = { m_GraphicsContext->GetSwapchain()->GetSemaphores().render_complete };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    VK_CHECK_RESULT(vkQueueSubmit(m_GraphicsContext->GetDevice()->GetGraphicsQueue(), 1, &submitInfo, m_GraphicsContext->GetSwapchain()->GetCurrentFence()));
    m_GraphicsContext->GetSwapchain()->EndFrame();
}

void VulkanRendererAPI::RenderImGui()
{
    // auto image = m_RenderPass->RawFramebuffer()[0];
    // auto image_extent = m_Swapchain->GetSpecification().extent;
    //
    // BeginRender(
    //     { image },
    //     image_extent,
    //     { 0, 0 },
    //     { 0.0f, 0.0f, 0.0f, 1.0f });
    //
    // Renderer::Submit([=]() mutable {
    //     ImGui::Render();
    //     ImDrawData* draw_data = ImGui::GetDrawData();
    //     ImGui_ImplVulkan_RenderDrawData(draw_data, m_CurrentCmdBuffer->Raw());
    // });
    // EndRender(image);
}

void VulkanRendererAPI::WaitDevice()
{
    vkDeviceWaitIdle(m_Device->Raw());
}

}
