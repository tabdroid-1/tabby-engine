#include <Drivers/Vulkan/VulkanGraphicsContext.h>
#include <Drivers/Vulkan/VulkanDeviceCmdBuffer.h>
#include <Drivers/Vulkan/VulkanShaderBuffer.h>
#include <Drivers/Vulkan/VulkanRendererAPI.h>
#include <Drivers/Vulkan/VulkanRenderPass.h>
#include <Drivers/Vulkan/VulkanSwapchain.h>
#include <Drivers/Vulkan/VulkanPipeline.h>
#include <Drivers/Vulkan/VulkanMaterial.h>
#include <Drivers/Vulkan/VulkanShader.h>
#include <Drivers/Vulkan/VulkanDevice.h>

#include <Tabby/Renderer/ShaderLibrary.h>
#include <Tabby/Asset/AssetManager.h>
#include <Tabby/Asset/GLTFLoader.h>
#include <Tabby/Core/Input/Input.h>
#include <Tabby/Renderer/Mesh.h>

#include <imgui.h>

#include "backends/imgui_impl_vulkan.h"

namespace Tabby {

struct Vertex {
    Vector3 position;
    Vector3 color;
    Vector2 texCoord;
};

const std::vector<Vertex> vertices = {
    { { -0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
    { { 0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },
    { { 0.5f, 0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
    { { -0.5f, 0.5f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } },

    { { -0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
    { { 0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },
    { { 0.5f, 0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
    { { -0.5f, 0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } }
};

const std::vector<uint16_t> indices = {
    0, 1, 2, 2, 3, 0,
    4, 5, 6, 6, 7, 4
};

struct Uniform {
    Matrix4 model;
    Matrix4 view;
    Matrix4 proj;
};
std::vector<GLTFLoader::EntityGLTFMeshData> meshes;

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

    auto image_asset_handle = AssetManager::LoadAssetSource("textures/Tabby.png");
    m_Image = AssetManager::GetAsset<Image>(image_asset_handle);

    ImageSamplerSpecification sampler_spec = {};
    sampler_spec.min_filtering_mode = SamplerFilteringMode::LINEAR;
    sampler_spec.mag_filtering_mode = SamplerFilteringMode::NEAREST;
    sampler_spec.mipmap_filtering_mode = SamplerFilteringMode::LINEAR;
    sampler_spec.address_mode = SamplerAddressMode::REPEAT;
    sampler_spec.min_lod = 0.0f;
    sampler_spec.max_lod = 1000.0f;
    sampler_spec.lod_bias = 0.0f;
    sampler_spec.anisotropic_filtering_level = 16;

    m_ImageSampler = ImageSampler::Create(sampler_spec);

    ShaderSpecification shader_spec = ShaderSpecification::Default();
    shader_spec.culling_mode = PipelineCullingMode::NONE;
    shader_spec.output_attachments_formats = { ImageFormat::RGBA32_UNORM };

    // ShaderBufferLayoutElement element0("inPosition", ShaderDataType::FLOAT3);
    // ShaderBufferLayoutElement element1("inColor", ShaderDataType::FLOAT3);
    // ShaderBufferLayoutElement element2("inColor", ShaderDataType::FLOAT2);
    // ShaderBufferLayout buffer_layout(std::vector { element0, element1, element2 });
    // shader_spec.input_layout = buffer_layout;

    ShaderLibrary::LoadShader(shader_spec, "shaders/vulkan/test.glsl");
    m_Shader = ShareAs<VulkanShader>(ShaderLibrary::GetShader("test.glsl"));

    MaterialSpecification mat_spec;
    mat_spec.name = "test_mat";
    mat_spec.shader = m_Shader;

    m_Material = Material::Create(mat_spec);

    // MeshSpecification mesh_spec;
    // mesh_spec.name = "test_mesh";
    // mesh_spec.material = m_Material;
    //
    // Buffer vertex_data;
    // vertex_data.Allocate(vertices.size() * sizeof(Vertex));
    // memcpy(vertex_data.Data, vertices.data(), vertices.size() * sizeof(Vertex));
    //
    // mesh_spec.vertex_data = vertex_data;
    //
    // Buffer index_data;
    // index_data.Allocate(indices.size() * sizeof(uint16_t));
    // memcpy(index_data.Data, indices.data(), indices.size() * sizeof(uint16_t));
    //
    // mesh_spec.index_data = index_data;
    //
    // m_Mesh = CreateShared<Mesh>(mesh_spec);
    //
    // m_Material->UploadData("texSampler", 0, m_Image, m_ImageSampler);

    meshes = GLTFLoader::Parse("scenes/sponza-small/sponza.gltf");

    for (auto& data : meshes) {
        if (!data.mesh)
            continue;

        data.mesh->SetMaterial(m_Material);
        for (auto& image : data.images) {
            data.mesh->GetMaterial()->UploadData("texSampler", 0, image.second, m_ImageSampler);
        }
    }

    // Buffer data;
    // data.Allocate(vertices.size() * sizeof(Vertex));
    // memcpy(data.Data, vertices.data(), vertices.size() * sizeof(Vertex));
    //
    // TB_CORE_INFO("MANUAL: {}", sizeof(Matrix4));
    // ShaderBufferSpecification buffer_spec = {};
    // buffer_spec.buffer_usage = ShaderBufferUsage::VERTEX_BUFFER;
    // buffer_spec.heap = ShaderBufferMemoryHeap::DEVICE;
    // buffer_spec.memory_usage = ShaderBufferMemoryUsage::NO_HOST_ACCESS;
    // buffer_spec.size = data.Size;
    //
    // m_VertexBuffer = ShareAs<VulkanShaderBuffer>(ShaderBuffer::Create(buffer_spec, data));
    // data.Release();
    //
    // data.Allocate(indices.size() * sizeof(uint16_t));
    // memcpy(data.Data, indices.data(), indices.size() * sizeof(uint16_t));
    // buffer_spec.buffer_usage = ShaderBufferUsage::INDEX_BUFFER;
    // buffer_spec.heap = ShaderBufferMemoryHeap::DEVICE;
    // buffer_spec.memory_usage = ShaderBufferMemoryUsage::NO_HOST_ACCESS;
    // buffer_spec.size = data.Size;
    // buffer_spec.flags |= (uint64_t)ShaderBufferFlags::INDEX_TYPE_UINT16;
    //
    // m_IndexBuffer = ShareAs<VulkanShaderBuffer>(ShaderBuffer::Create(buffer_spec, data));
    // data.Release();
    //
    // data.Allocate(sizeof(Uniform));
    // // memcpy(data.Data, nullptr, sizeof(m_UniformBufferData));
    // buffer_spec.buffer_usage = ShaderBufferUsage::UNIFORM_BUFFER;
    // buffer_spec.heap = ShaderBufferMemoryHeap::HOST;
    // buffer_spec.memory_usage = ShaderBufferMemoryUsage::COHERENT_WRITE;
    // buffer_spec.size = data.Size;
    //
    // m_UniformBuffer = ShareAs<VulkanShaderBuffer>(ShaderBuffer::Create(buffer_spec, data));
    // data.Release();
    //
    // auto image_asset_handle = AssetManager::LoadAssetSource("textures/Tabby.png");
    // m_Image = AssetManager::GetAsset<Image>(image_asset_handle);
    //
    // ImageSamplerSpecification sampler_spec = {};
    // sampler_spec.min_filtering_mode = SamplerFilteringMode::LINEAR;
    // sampler_spec.mag_filtering_mode = SamplerFilteringMode::NEAREST;
    // sampler_spec.mipmap_filtering_mode = SamplerFilteringMode::LINEAR;
    // sampler_spec.address_mode = SamplerAddressMode::REPEAT;
    // sampler_spec.min_lod = 0.0f;
    // sampler_spec.max_lod = 1000.0f;
    // sampler_spec.lod_bias = 0.0f;
    // sampler_spec.anisotropic_filtering_level = 16;
    //
    // m_ImageSampler = ImageSampler::Create(sampler_spec);
    //
    // VulkanDescriptorBinding binding0;
    // binding0.type = VulkanDescriptorBindingType::UNIFORM_BUFFER;
    // binding0.binding = 0;
    // binding0.array_count = 1;
    //
    // VulkanDescriptorBinding binding1;
    // binding1.type = VulkanDescriptorBindingType::SAMPLED_IMAGE;
    // binding1.binding = 1;
    // binding1.array_count = 1;
    //
    // VulkanDescriptorSetSpecification descriptorset_spec;
    // descriptorset_spec.bindings = { binding0, binding1 };
    //
    // m_DescriptionSet = CreateShared<VulkanDescriptorSet>(descriptorset_spec);
    // m_DescriptionSet->Write(0, 0, m_UniformBuffer, sizeof(Uniform), 0);
    // m_DescriptionSet->Write(1, 0, m_Image, m_ImageSampler);
}

VulkanRendererAPI::~VulkanRendererAPI()
{
    vkDeviceWaitIdle(m_Device->Raw());
    // m_VertexBuffer->Destroy();
    // m_IndexBuffer->Destroy();
    // m_UniformBuffer->Destroy();
    // m_DescriptionSet->Destroy();
    m_ImageSampler->Destroy();
    m_Mesh->Destroy();

    for (auto& cmd_buffer : m_CmdBuffers)
        cmd_buffer->Destroy();

    vkDestroyDescriptorPool(m_Device->Raw(), s_DescriptorPool, nullptr);
}

void VulkanRendererAPI::Render()
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    Uniform ubo {};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    // ubo.view = glm::rotate(ubo.view, time * glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), VulkanGraphicsContext::Get()->GetSwapchain()->RawExtend().width / (float)VulkanGraphicsContext::Get()->GetSwapchain()->RawExtend().height, 0.1f, 1000.0f);
    ubo.proj[1][1] *= -1;

    m_Material->UploadData("ubo", 0, &ubo, sizeof(Uniform));

    m_GraphicsContext->GetSwapchain()->BeginFrame();
    m_CurrentCmdBuffer = m_CmdBuffers[m_Swapchain->GetCurrentFrameIndex()];
    m_CmdBuffers[m_GraphicsContext->GetSwapchain()->GetCurrentFrameIndex()]->Reset();
    m_CmdBuffers[m_GraphicsContext->GetSwapchain()->GetCurrentFrameIndex()]->Begin();
    m_GraphicsContext->GetRenderPass()->Begin(m_CmdBuffers[m_GraphicsContext->GetSwapchain()->GetCurrentFrameIndex()]->Raw());

    // VkPipelineBindPoint bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;
    //
    // std::vector<VkDescriptorSet> raw_set;
    //
    // for (auto& descriptor : ShareAs<VulkanMaterial>(m_Mesh->GetMaterial())->GetDescriptorSets()) {
    //     raw_set.push_back(descriptor->Raw());
    // }
    //
    // VkBuffer vertexBuffers[] = { ShareAs<VulkanShaderBuffer>(m_Mesh->GetVertexBuffer())->Raw() };
    // VkDeviceSize offsets[] = { 0 };
    //
    // vkCmdBindPipeline(m_CmdBuffers[m_GraphicsContext->GetSwapchain()->GetCurrentFrameIndex()]->Raw(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_Shader->GetPipeline()->Raw());
    // vkCmdBindVertexBuffers(m_CmdBuffers[m_GraphicsContext->GetSwapchain()->GetCurrentFrameIndex()]->Raw(), 0, 1, vertexBuffers, offsets);
    //
    // vkCmdBindIndexBuffer(m_CmdBuffers[m_GraphicsContext->GetSwapchain()->GetCurrentFrameIndex()]->Raw(), ShareAs<VulkanShaderBuffer>(m_Mesh->GetIndexBuffer())->Raw(), 0, VK_INDEX_TYPE_UINT16);
    //
    // vkCmdBindDescriptorSets(m_CurrentCmdBuffer->Raw(), bind_point, m_Shader->GetPipeline()->RawLayout(), 0, raw_set.size(), raw_set.data(), 0, nullptr);
    //
    // vkCmdDrawIndexed(m_CmdBuffers[m_GraphicsContext->GetSwapchain()->GetCurrentFrameIndex()]->Raw(), m_Mesh->TotalIndexCount(), 1, 0, 0, 0);
    // // vkCmdDraw(m_CmdBuffers[m_GraphicsContext->GetSwapchain()->GetCurrentFrameIndex()]->Raw(), 6, 1, 0, 0);

    for (const auto& mesh_data : meshes) {

        if (!mesh_data.mesh)
            continue;

        VkPipelineBindPoint bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;

        std::vector<VkDescriptorSet> raw_set;

        for (auto& descriptor : ShareAs<VulkanMaterial>(mesh_data.mesh->GetMaterial())->GetDescriptorSets()) {
            raw_set.push_back(descriptor->Raw());
        }

        VkBuffer vertexBuffers[] = { ShareAs<VulkanShaderBuffer>(mesh_data.mesh->GetVertexBuffer())->Raw() };
        VkDeviceSize offsets[] = { 0 };

        vkCmdBindPipeline(m_CmdBuffers[m_GraphicsContext->GetSwapchain()->GetCurrentFrameIndex()]->Raw(), VK_PIPELINE_BIND_POINT_GRAPHICS, ShareAs<VulkanShader>(mesh_data.mesh->GetMaterial()->GetShader())->GetPipeline()->Raw());
        vkCmdBindVertexBuffers(m_CmdBuffers[m_GraphicsContext->GetSwapchain()->GetCurrentFrameIndex()]->Raw(), 0, 1, vertexBuffers, offsets);

        vkCmdBindIndexBuffer(m_CmdBuffers[m_GraphicsContext->GetSwapchain()->GetCurrentFrameIndex()]->Raw(), ShareAs<VulkanShaderBuffer>(mesh_data.mesh->GetIndexBuffer())->Raw(), 0, VK_INDEX_TYPE_UINT16);

        vkCmdBindDescriptorSets(m_CurrentCmdBuffer->Raw(), bind_point, ShareAs<VulkanShader>(mesh_data.mesh->GetMaterial()->GetShader())->GetPipeline()->RawLayout(), 0, raw_set.size(), raw_set.data(), 0, nullptr);

        vkCmdDrawIndexed(m_CmdBuffers[m_GraphicsContext->GetSwapchain()->GetCurrentFrameIndex()]->Raw(), mesh_data.mesh->TotalIndexCount(), 1, 0, 0, 0);
        // vkCmdDraw(m_CmdBuffers[m_GraphicsContext->GetSwapchain()->GetCurrentFrameIndex()]->Raw(), 6, 1, 0, 0);
    }

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
