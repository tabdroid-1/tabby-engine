#include <Drivers/Vulkan/UI/VulkanImGuiRenderer.h>
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
#include <Drivers/Vulkan/VulkanImage.h>

#include <Tabby/Renderer/ShaderLibrary.h>
#include <Tabby/Asset/AssetManager.h>
#include <Tabby/Core/Application.h>
#include <Tabby/Asset/GLTFLoader.h>
#include <Tabby/Core/Input/Input.h>
#include <Tabby/Renderer/Mesh.h>

#include <imgui.h>

#include "backends/imgui_impl_vulkan.h"

namespace Tabby {

VulkanRendererAPI::VulkanRendererAPI(const RendererConfig& config)
    : m_Config(config)
{

    m_GraphicsContext = std::make_shared<VulkanGraphicsContext>(config);
    m_Device = m_GraphicsContext->GetDevice();

    m_Swapchain = m_GraphicsContext->GetSwapchain();

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
}

VulkanRendererAPI::~VulkanRendererAPI()
{
    vkDeviceWaitIdle(m_Device->Raw());

    for (auto& cmd_buffer : m_CmdBuffers)
        cmd_buffer->Destroy();

    vkDestroyDescriptorPool(m_Device->Raw(), s_DescriptorPool, nullptr);
}
uint32_t VulkanRendererAPI::GetCurrentFrameIndex() const { return m_Swapchain->GetCurrentFrameIndex(); }

void VulkanRendererAPI::BeginFrame()
{
    m_Swapchain->BeginFrame();
    m_CurrentCmdBuffer = m_CmdBuffers[m_Swapchain->GetCurrentFrameIndex()];
    this->BeginCommandRecord();
}

void VulkanRendererAPI::EndFrame()
{
    m_Swapchain->EndFrame();
}

void VulkanRendererAPI::BeginCommandRecord()
{
    Submit([=]() mutable {
        m_CurrentCmdBuffer->Reset();
        m_CurrentCmdBuffer->Begin();
    });
}

void VulkanRendererAPI::EndCommandRecord()
{
    Submit([=]() mutable {
        m_CurrentCmdBuffer->End();
    });
}

void VulkanRendererAPI::ExecuteCurrentCommands()
{
    Submit(
        [=]() mutable {
            // Execution code
            VkPipelineStageFlags stagemasks[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

            Shared<VulkanDeviceCmdBuffer> vk_cmd_buffer = ShareAs<VulkanDeviceCmdBuffer>(m_CurrentCmdBuffer);
            VkCommandBuffer raw_buffer = vk_cmd_buffer->Raw();
            auto semaphores = m_Swapchain->GetSemaphores();

            VkSubmitInfo submitinfo = {};
            submitinfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitinfo.commandBufferCount = 1;
            submitinfo.pCommandBuffers = &raw_buffer;
            submitinfo.signalSemaphoreCount = 1;
            submitinfo.pSignalSemaphores = &semaphores.render_complete;
            submitinfo.waitSemaphoreCount = 1;
            submitinfo.pWaitSemaphores = &semaphores.present_complete;
            submitinfo.pWaitDstStageMask = stagemasks;

            m_Mutex.lock();
            VK_CHECK_RESULT(vkQueueSubmit(m_Device->GetGraphicsQueue(), 1, &submitinfo, m_Swapchain->GetCurrentFence()));
            m_Mutex.unlock();
        });
}

void VulkanRendererAPI::ClearImage(Shared<Image> image, const Vector4& value)
{
    Submit([=]() mutable {
        Shared<VulkanImage> vk_image = ShareAs<VulkanImage>(image);

        vk_image->SetLayout(
            m_CurrentCmdBuffer,
            ImageLayout::TRANSFER_DST,
            PipelineStage::TOP_OF_PIPE,
            PipelineStage::TRANSFER);

        VkClearColorValue clear_color_value = {};
        clear_color_value.float32[0] = value.r;
        clear_color_value.float32[1] = value.g;
        clear_color_value.float32[2] = value.b;
        clear_color_value.float32[3] = value.a;

        VkImageSubresourceRange range = {};
        range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        range.baseMipLevel = 0;
        range.levelCount = 1;
        range.baseArrayLayer = 0;
        range.layerCount = 1;

        vkCmdClearColorImage(m_CurrentCmdBuffer->Raw(), vk_image->Raw(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clear_color_value, 1, &range);

        vk_image->SetLayout(m_CurrentCmdBuffer,
            ImageLayout::PRESENT_SRC,
            PipelineStage::TRANSFER,
            PipelineStage::BOTTOM_OF_PIPE);
    });
}

void VulkanRendererAPI::RenderTasks(Shared<Mesh> mesh, std::vector<MaterialData> elements, Buffer data)
{
    Submit([=]() mutable {
        if (!mesh)
            return;

        auto pipeline = ShareAs<VulkanShader>(mesh->GetMaterial()->GetShader())->GetPipeline();
        auto vertex_buffer = ShareAs<VulkanShaderBuffer>(mesh->GetVertexBuffer());
        auto index_buffer = ShareAs<VulkanShaderBuffer>(mesh->GetIndexBuffer());
        auto material = ShareAs<VulkanMaterial>(mesh->GetMaterial());

        if (pipeline->GetCurrentRenderPass() != m_CurrentRenderPass.render_pass || pipeline->Raw() == VK_NULL_HANDLE)
            pipeline->Create(m_CurrentRenderPass.render_pass);

        for (auto& element : elements) {
            if (element.type == MaterialData::Type::Uniform) {
                material->UploadData(element.name, element.offset, element.data, element.data_size);
            } else if (element.type == MaterialData::Type::Image) {
                material->UploadData(element.name, element.array_element, element.image, element.sampler);
            }
        }

        VkDeviceSize offsets[] = { 0 };
        VkBuffer raw_vertex_buffer = vertex_buffer->Raw();
        std::vector<VkDescriptorSet> raw_set;
        raw_set.reserve(material->GetDescriptorSets().size());
        for (auto& descriptor : material->GetDescriptorSets()) {
            raw_set.push_back(descriptor->Raw());
        }

        if (data.Size) {
            vkCmdPushConstants(m_CurrentCmdBuffer->Raw(), pipeline->RawLayout(), VK_SHADER_STAGE_ALL, 0, data.Size, data.Data);
            delete[] data.Data;
        }

        VkIndexType index_type;

        if (mesh->GetSpecification().flags & (uint64_t)MeshFlags::INDEX_TYPE_UINT8)
            index_type = VK_INDEX_TYPE_UINT8_EXT;
        if (mesh->GetSpecification().flags & (uint64_t)MeshFlags::INDEX_TYPE_UINT16)
            index_type = VK_INDEX_TYPE_UINT16;
        if (mesh->GetSpecification().flags & (uint64_t)MeshFlags::INDEX_TYPE_UINT32)
            index_type = VK_INDEX_TYPE_UINT32;

        vkCmdBindPipeline(m_CurrentCmdBuffer->Raw(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->Raw());
        vkCmdBindVertexBuffers(m_CurrentCmdBuffer->Raw(), 0, 1, &raw_vertex_buffer, offsets);
        vkCmdBindIndexBuffer(m_CurrentCmdBuffer->Raw(), index_buffer->Raw(), 0, index_type);
        vkCmdBindDescriptorSets(m_CurrentCmdBuffer->Raw(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->RawLayout(), 0, raw_set.size(), raw_set.data(), 0, nullptr);
        vkCmdDrawIndexed(m_CurrentCmdBuffer->Raw(), mesh->TotalIndexCount(), 1, 0, 0, 0);
    });
}

void VulkanRendererAPI::RenderTasks(Shared<Shader> shader, uint32_t vertex_count, Buffer data)
{
    Submit([=]() mutable {
        auto vk_shader = ShareAs<VulkanShader>(shader);

        if (data.Size) {
            vkCmdPushConstants(m_CurrentCmdBuffer->Raw(), vk_shader->GetPipeline()->RawLayout(), VK_SHADER_STAGE_ALL, 0, data.Size, data.Data);
            delete[] data.Data;
        }

        vkCmdBindPipeline(m_CurrentCmdBuffer->Raw(), VK_PIPELINE_BIND_POINT_GRAPHICS, vk_shader->GetPipeline()->Raw());
        vkCmdDraw(m_CurrentCmdBuffer->Raw(), vertex_count, 1, 0, 0);
    });
}
void VulkanRendererAPI::BeginRender(std::vector<Shared<Image>> attachments, UIntVector3 render_area, IntVector2 render_offset, Vector4 clear_color)
{

    Submit([=]() mutable {
        Shared<VulkanRenderPass> render_pass;
        for (auto& render_pass_info : m_RenderPassInfos) {
            if (render_pass_info.spec.attachments != attachments)
                continue;

            if (render_pass_info.spec.clear_color != clear_color)
                continue;

            if (render_pass_info.spec.extent != render_area)
                continue;

            render_pass = render_pass_info.render_pass;
        }

        if (!render_pass) {
            VulkanRendererAPIRenderPassInfo render_pass_info;
            render_pass_info.spec.attachments = attachments;
            render_pass_info.spec.clear_color = clear_color;
            render_pass_info.spec.extent = render_area;
            render_pass_info.render_pass = CreateShared<VulkanRenderPass>();
            render_pass_info.render_pass->Create(render_pass_info.spec);

            m_RenderPassInfos.push_back(render_pass_info);
            render_pass = render_pass_info.render_pass;
            m_CurrentRenderPass = render_pass_info;
        }

        std::vector<VkClearValue> clear_values;
        clear_values.resize(attachments.size());
        for (int i = 0; i < attachments.size(); i++) {
            if (attachments[i]->GetSpecification().usage == ImageUsage::RENDER_TARGET)
                clear_values[i].color = { clear_color.r, clear_color.g, clear_color.b, clear_color.a };
            else if (attachments[i]->GetSpecification().usage == ImageUsage::DEPTH_BUFFER)
                clear_values[i].depthStencil = { 1.0f, 0 };
        }

        VkRenderPassBeginInfo render_pass_info {};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_info.renderPass = render_pass->Raw();
        render_pass_info.framebuffer = render_pass->RawFramebuffer();
        render_pass_info.renderArea.offset = { render_offset.x, render_offset.y };
        render_pass_info.renderArea.extent = { render_area.x, render_area.y };
        render_pass_info.clearValueCount = clear_values.size();
        render_pass_info.pClearValues = clear_values.data();

        VkRect2D scissor = { { 0, 0 }, { render_area.x, render_area.y } };
        VkViewport viewport = { 0, (float)render_area.y, (float)render_area.x, -(float)render_area.y, 0.0f, 1.0f };
        vkCmdSetScissor(m_CurrentCmdBuffer->Raw(), 0, 1, &scissor);
        vkCmdSetViewport(m_CurrentCmdBuffer->Raw(), 0, 1, &viewport);
        vkCmdBeginRenderPass(m_CurrentCmdBuffer->Raw(), &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
    });
}

void VulkanRendererAPI::EndRender(Shared<Image> target)
{
    Submit([=]() mutable {
        vkCmdEndRenderPass(m_CurrentCmdBuffer->Raw());
    });
}

void VulkanRendererAPI::Render()
{
    Submit([=]() mutable {
        auto swapchain_image = m_Swapchain->GetCurrentImage();
        swapchain_image->SetLayout(
            m_CurrentCmdBuffer,
            ImageLayout::PRESENT_SRC,
            PipelineStage::COLOR_ATTACHMENT_OUTPUT,
            PipelineStage::ALL_COMMANDS,
            (BitMask)PipelineAccess::COLOR_ATTACHMENT_WRITE,
            (BitMask)PipelineAccess::MEMORY_READ);
    });
    this->EndCommandRecord();
    this->ExecuteCurrentCommands();

    auto function_list = std::move(m_FunctionList);

    for (auto& func : function_list) {
        func();
    }
}

void VulkanRendererAPI::RenderImGui()
{
    auto imgui_renderer = (VulkanImGuiRenderer*)Application::GetImGuiRenderer();

    BeginRender(
        { imgui_renderer->GetFrameImages()[m_Swapchain->GetCurrentFrameIndex()] },
        imgui_renderer->GetFrameImages()[m_Swapchain->GetCurrentFrameIndex()]->GetSpecification().extent,
        { 0, 0 },
        { 0.0f, 0.0f, 0.0f, 1.0f });

    Submit([=]() mutable {
        ImGui::Render();
        ImDrawData* draw_data = ImGui::GetDrawData();
        ImGui_ImplVulkan_RenderDrawData(draw_data, m_CurrentCmdBuffer->Raw());
    });
    EndRender(imgui_renderer->GetFrameImages()[m_Swapchain->GetCurrentFrameIndex()]);
    CopyToSwapchain(imgui_renderer->GetFrameImages()[m_Swapchain->GetCurrentFrameIndex()]);
}

void VulkanRendererAPI::CopyToSwapchain(Shared<Image> image)
{
    Renderer::Submit([=]() mutable {
        Shared<VulkanImage> vk_image = ShareAs<VulkanImage>(image);
        Shared<Image> swapchain_image = m_Swapchain->GetCurrentImage();
        UIntVector3 swapchain_resolution = swapchain_image->GetSpecification().extent;
        UIntVector3 src_image_resolution = image->GetSpecification().extent;

        VkImageBlit image_blit = {};
        image_blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_blit.srcSubresource.mipLevel = 0;
        image_blit.srcSubresource.baseArrayLayer = 0;
        image_blit.srcSubresource.layerCount = 1;
        image_blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_blit.dstSubresource.mipLevel = 0;
        image_blit.dstSubresource.baseArrayLayer = 0;
        image_blit.dstSubresource.layerCount = 1;
        image_blit.srcOffsets[0] = { 0, 0, 0 };
        image_blit.srcOffsets[1] = { (int)src_image_resolution.x, (int)src_image_resolution.y, 1 };
        image_blit.dstOffsets[0] = { 0, 0, 0 };
        image_blit.dstOffsets[1] = { (int)swapchain_resolution.x, (int)swapchain_resolution.y, 1 };

        vk_image->SetLayout(
            m_CurrentCmdBuffer,
            ImageLayout::TRANSFER_DST,
            PipelineStage::COLOR_ATTACHMENT_OUTPUT,
            PipelineStage::TRANSFER,
            (BitMask)PipelineAccess::COLOR_ATTACHMENT_WRITE,
            (BitMask)PipelineAccess::TRANSFER_READ);

        vkCmdBlitImage(
            m_CurrentCmdBuffer->Raw(),
            vk_image->Raw(),
            (VkImageLayout)vk_image->GetCurrentLayout(),
            ShareAs<VulkanImage>(image)->Raw(),
            (VkImageLayout)VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &image_blit,
            VK_FILTER_LINEAR);
    });
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
