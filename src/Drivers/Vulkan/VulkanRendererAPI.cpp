#include <Drivers/Vulkan/VulkanGraphicsContext.h>
#include <Drivers/Vulkan/VulkanDeviceCmdBuffer.h>
#include <Drivers/Vulkan/VulkanShaderBuffer.h>
#include <Drivers/Vulkan/VulkanRendererAPI.h>
#include <Drivers/Vulkan/VulkanRenderPass.h>
#include <Drivers/Vulkan/VulkanSwapchain.h>
#include <Drivers/Vulkan/VulkanShader.h>
#include <Drivers/Vulkan/VulkanDevice.h>
#include <Tabby/Core/Input/Input.h>

#include <Tabby/Renderer/ShaderLibrary.h>

#include <imgui.h>
#include <vulkan/vulkan_core.h>

#include "Drivers/Vulkan/VulkanDescriptorSet.h"
#include "Tabby/Core/Base.h"
#include "Tabby/Core/Input/KeyCode.h"
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

Uniform m_UniformBufferData = { { 0.0f, 0.0f } };

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
    if (s_DescriptorPool == VK_NULL_HANDLE) {
        uint32_t descriptor_count = UINT16_MAX * config.frames_in_flight;

        std::vector<VkDescriptorPoolSize> pool_sizes = {
            { VK_DESCRIPTOR_TYPE_SAMPLER, descriptor_count },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, descriptor_count },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, descriptor_count },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, descriptor_count },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, descriptor_count },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, descriptor_count },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, descriptor_count }
        };

        VkDescriptorPoolCreateInfo descriptor_pool_create_info = {};
        descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptor_pool_create_info.maxSets = 1000;
        descriptor_pool_create_info.poolSizeCount = pool_sizes.size();
        descriptor_pool_create_info.pPoolSizes = pool_sizes.data();
        descriptor_pool_create_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

        vkCreateDescriptorPool(m_Device->Raw(), &descriptor_pool_create_info, nullptr, &s_DescriptorPool);
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
    buffer_spec.size = data.Size;

    m_VertexBuffer = ShareAs<VulkanShaderBuffer>(ShaderBuffer::Create(buffer_spec, data));
    data.Release();

    data.Allocate(indices.size() * sizeof(uint16_t));
    memcpy(data.Data, indices.data(), indices.size() * sizeof(uint16_t));
    buffer_spec.buffer_usage = ShaderBufferUsage::INDEX_BUFFER;
    buffer_spec.heap = ShaderBufferMemoryHeap::DEVICE;
    buffer_spec.memory_usage = ShaderBufferMemoryUsage::NO_HOST_ACCESS;
    buffer_spec.size = data.Size;
    buffer_spec.flags |= (uint64_t)ShaderBufferFlags::INDEX_TYPE_UINT16;

    m_IndexBuffer = ShareAs<VulkanShaderBuffer>(ShaderBuffer::Create(buffer_spec, data));
    data.Release();

    data.Allocate(sizeof(Uniform));
    memcpy(data.Data, &m_UniformBufferData, sizeof(m_UniformBufferData));
    buffer_spec.buffer_usage = ShaderBufferUsage::UNIFORM_BUFFER;
    buffer_spec.heap = ShaderBufferMemoryHeap::HOST;
    buffer_spec.memory_usage = ShaderBufferMemoryUsage::COHERENT_WRITE;
    buffer_spec.size = data.Size;

    m_UniformBuffer = ShareAs<VulkanShaderBuffer>(ShaderBuffer::Create(buffer_spec, data));
    data.Release();

    m_DescriptionSet = CreateShared<VulkanDescriptorSet>(m_Shader->GetLayouts()[0]);
    m_DescriptionSet->Write(0, 0, m_UniformBuffer, data.Size, 0);
}

VulkanRendererAPI::~VulkanRendererAPI()
{
    vkDeviceWaitIdle(m_Device->Raw());
    m_VertexBuffer->Destroy();
    m_IndexBuffer->Destroy();

    for (auto& cmd_buffer : m_CmdBuffers)
        cmd_buffer->Destroy();

    vkDestroyDescriptorPool(m_Device->Raw(), s_DescriptorPool, nullptr);
}

void VulkanRendererAPI::Render()
{
    m_UniformBufferData.position = { 0.0f, 0.0f };
    if (Input::GetKey(Key::A))
        m_UniformBufferData.position.x--;
    if (Input::GetKey(Key::D))
        m_UniformBufferData.position.x++;

    if (Input::GetKey(Key::S))
        m_UniformBufferData.position.y++;
    if (Input::GetKey(Key::W))
        m_UniformBufferData.position.y--;

    Buffer data;
    data.Allocate(sizeof(Uniform));
    memcpy(data.Data, &m_UniformBufferData, sizeof(Uniform));

    m_UniformBuffer->UploadData(0, data);

    // m_DescriptionSet->Write(0, 0, m_UniformBuffer, data.Size, 0);
    data.Release();

    m_GraphicsContext->GetSwapchain()->BeginFrame();
    m_CurrentCmdBuffer = m_CmdBuffers[m_Swapchain->GetCurrentFrameIndex()];
    m_CmdBuffers[m_GraphicsContext->GetSwapchain()->GetCurrentFrameIndex()]->Reset();
    m_CmdBuffers[m_GraphicsContext->GetSwapchain()->GetCurrentFrameIndex()]->Begin();
    m_GraphicsContext->GetRenderPass()->Begin(m_CmdBuffers[m_GraphicsContext->GetSwapchain()->GetCurrentFrameIndex()]->Raw());

    VkPipelineBindPoint bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;

    VkDescriptorSet raw_set = m_DescriptionSet->Raw();

    VkBuffer vertexBuffers[] = { m_VertexBuffer->Raw() };
    VkDeviceSize offsets[] = { 0 };

    vkCmdBindPipeline(m_CmdBuffers[m_GraphicsContext->GetSwapchain()->GetCurrentFrameIndex()]->Raw(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_Shader->RawPipeline());
    vkCmdBindVertexBuffers(m_CmdBuffers[m_GraphicsContext->GetSwapchain()->GetCurrentFrameIndex()]->Raw(), 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(m_CmdBuffers[m_GraphicsContext->GetSwapchain()->GetCurrentFrameIndex()]->Raw(), m_IndexBuffer->Raw(), 0, VK_INDEX_TYPE_UINT16);

    vkCmdBindDescriptorSets(m_CurrentCmdBuffer->Raw(), bind_point, m_Shader->RawPipelineLayout(), 0, 1, &raw_set, 0, nullptr);

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

std::vector<VkDescriptorSet> VulkanRendererAPI::AllocateDescriptorSets(VkDescriptorSetLayout layout, uint32_t count)
{
    auto device = VulkanGraphicsContext::Get()->GetDevice();

    std::vector<VkDescriptorSet> sets(count);
    sets.resize(count);

    VkDescriptorSetAllocateInfo allocate_info = {};
    allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocate_info.descriptorPool = s_DescriptorPool;
    allocate_info.descriptorSetCount = count;
    allocate_info.pSetLayouts = &layout;

    if (vkAllocateDescriptorSets(device->Raw(), &allocate_info, sets.data()) != VK_SUCCESS) {
        TB_CORE_ERROR("Failed to allocate descriptor set. Possible issue: too many allocated descriptor sets.");
        return std::vector<VkDescriptorSet>(0);
    };

    return sets;
}

void VulkanRendererAPI::FreeDescriptorSets(std::vector<VkDescriptorSet> sets)
{
    auto device = VulkanGraphicsContext::Get()->GetDevice();

    VK_CHECK_RESULT(vkFreeDescriptorSets(device->Raw(), s_DescriptorPool, sets.size(), sets.data()));
}
}
