#include <Drivers/Vulkan/VulkanGraphicsContext.h>
#include <Drivers/Vulkan/VulkanDeviceCmdBuffer.h>
#include <Drivers/Vulkan/VulkanShaderBuffer.h>
#include <Drivers/Vulkan/VulkanRendererAPI.h>
#include <Drivers/Vulkan/VulkanRenderPass.h>
#include <Drivers/Vulkan/VulkanSwapchain.h>
#include <Drivers/Vulkan/VulkanShader.h>
#include <Drivers/Vulkan/VulkanDevice.h>

#include <Tabby/Renderer/ShaderLibrary.h>

#include <imgui.h>

#include "backends/imgui_impl_vulkan.h"

namespace Tabby {

struct Vertex {
    Vector2 position;
    Vector3 color;
};

// const std::vector<Vector3> vertices = {
//     { Vector3(-0.5f, -0.5f, 0.0f) },
//     { Vector3(0.5f, -0.5f, 0.0f) },
//     { Vector3(0.5f, 0.5f, 0.0f) },
//     { Vector3(-0.5f, 0.5f, 0.0f) }
// };

const std::vector<Vertex> vertices = {
    { { -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
    { { 0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f } },
    { { 0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } },
    { { -0.5f, 0.5f }, { 1.0f, 1.0f, 1.0f } }
};

const std::vector<uint16_t> indices = {
    0, 1, 2, 2, 3, 0
};

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

    // std::vector<byte> vertex_data(icosphere_data.first.size() * sizeof(glm::vec3));
    // memcpy(vertex_data.data(), icosphere_data.first.data(), vertex_data.size());

    Buffer data;
    data.Allocate(vertices.size() * sizeof(Vertex));
    memcpy(data.Data, vertices.data(), vertices.size() * sizeof(Vertex));

    ShaderBufferSpecification buffer_spec = {};
    buffer_spec.buffer_usage = ShaderBufferUsage::VERTEX_BUFFER;
    buffer_spec.heap = ShaderBufferMemoryHeap::DEVICE;
    buffer_spec.memory_usage = ShaderBufferMemoryUsage::NO_HOST_ACCESS;
    // buffer_spec.memory_usage = ShaderBufferMemoryUsage::COHERENT_WRITE;
    buffer_spec.size = data.Size;

    m_VertexBuffer = ShareAs<VulkanShaderBuffer>(ShaderBuffer::Create(buffer_spec, data));
    data.Release();

    data.Allocate(indices.size() * sizeof(uint16_t));
    memcpy(data.Data, indices.data(), indices.size() * sizeof(uint16_t));
    buffer_spec.buffer_usage = ShaderBufferUsage::INDEX_BUFFER;
    buffer_spec.heap = ShaderBufferMemoryHeap::DEVICE;
    buffer_spec.memory_usage = ShaderBufferMemoryUsage::NO_HOST_ACCESS;
    // buffer_spec.memory_usage = ShaderBufferMemoryUsage::COHERENT_WRITE;
    buffer_spec.size = data.Size;
    buffer_spec.flags |= (uint64_t)ShaderBufferFlags::INDEX_TYPE_UINT16;

    m_IndexBuffer = ShareAs<VulkanShaderBuffer>(ShaderBuffer::Create(buffer_spec, data));
    data.Release();
}

VulkanRendererAPI::~VulkanRendererAPI()
{
    vkDeviceWaitIdle(m_Device->Raw());
    m_VertexBuffer->Destroy();
    m_IndexBuffer->Destroy();

    for (auto& cmd_buffer : m_CmdBuffers)
        cmd_buffer->Destroy();
}

void VulkanRendererAPI::Render()
{

    m_GraphicsContext->GetSwapchain()->BeginFrame();
    m_CmdBuffers[m_GraphicsContext->GetSwapchain()->GetCurrentFrameIndex()]->Reset();
    m_CmdBuffers[m_GraphicsContext->GetSwapchain()->GetCurrentFrameIndex()]->Begin();
    m_GraphicsContext->GetRenderPass()->Begin(m_CmdBuffers[m_GraphicsContext->GetSwapchain()->GetCurrentFrameIndex()]->Raw());

    VkBuffer vertexBuffers[] = { m_VertexBuffer->Raw() };
    VkDeviceSize offsets[] = { 0 };

    vkCmdBindPipeline(m_CmdBuffers[m_GraphicsContext->GetSwapchain()->GetCurrentFrameIndex()]->Raw(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_Shader->RawPipeline());
    vkCmdBindVertexBuffers(m_CmdBuffers[m_GraphicsContext->GetSwapchain()->GetCurrentFrameIndex()]->Raw(), 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(m_CmdBuffers[m_GraphicsContext->GetSwapchain()->GetCurrentFrameIndex()]->Raw(), m_IndexBuffer->Raw(), 0, VK_INDEX_TYPE_UINT16);

    vkCmdDrawIndexed(m_CmdBuffers[m_GraphicsContext->GetSwapchain()->GetCurrentFrameIndex()]->Raw(), static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
    // vkCmdDraw(m_CmdBuffers[m_GraphicsContext->GetSwapchain()->GetCurrentFrameIndex()]->Raw(), 6, 1, 0, 0);

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
