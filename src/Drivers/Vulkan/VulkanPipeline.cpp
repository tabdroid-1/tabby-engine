#include <Drivers/Vulkan/VulkanGraphicsContext.h>
#include <Drivers/Vulkan/VulkanRenderPass.h>
#include <Drivers/Vulkan/VulkanSwapchain.h>
#include <Drivers/Vulkan/VulkanPipeline.h>
#include <Drivers/Vulkan/VulkanDevice.h>
#include <Drivers/Vulkan/VulkanShader.h>
#include <Drivers/Vulkan/VulkanShader.h>
#include <Drivers/Vulkan/VulkanImage.h>

namespace Tabby {

// static VkRenderPass createDummyRenderPass(const std::vector<ImageFormat>& output_attachments_formats)
// {
//     std::vector<VkAttachmentReference> color_attachment_references;
//     std::vector<VkAttachmentDescription> attachments;
//     VkAttachmentReference depth_attachment_reference = {};
//
//     for (int i = 0; i < output_attachments_formats.size(); i++) {
//
//         if (output_attachments_formats[i] == ImageFormat::D32_S8) {
//         } else {
//             auto& color_attachment = attachments.emplace_back();
//             color_attachment.format = convert(output_attachments_formats[i]);
//             color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
//             color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//             color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//             color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//             color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
//             color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
//             color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
//
//             auto& color_attachment_reference = color_attachment_references.emplace_back();
//             color_attachment_reference.attachment = i;
//             color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
//         }
//     }
//
//     // HACK: I asume all render passes has depth buffer and all of them are the last attachment
//     {
//         auto& depth_attachment = attachments.emplace_back();
//         depth_attachment.format = VK_FORMAT_D32_SFLOAT_S8_UINT;
//         depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
//         depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
//         depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//         depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
//         depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
//         depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
//         depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
//
//         depth_attachment_reference.attachment = color_attachment_references.size();
//         depth_attachment_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
//     }
//
//     VkSubpassDescription subpass = {};
//     subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
//     subpass.colorAttachmentCount = color_attachment_references.size();
//     subpass.pColorAttachments = color_attachment_references.data();
//     subpass.pDepthStencilAttachment = &depth_attachment_reference;
//
//     VkSubpassDependency dependency {};
//     dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
//     dependency.dstSubpass = 0;
//     dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
//     dependency.srcAccessMask = 0;
//     dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
//     dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
//
//     VkRenderPassCreateInfo render_pass_info = {};
//     render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
//     render_pass_info.attachmentCount = attachments.size();
//     render_pass_info.pAttachments = attachments.data();
//     render_pass_info.subpassCount = 1;
//     render_pass_info.pSubpasses = &subpass;
//     render_pass_info.dependencyCount = 1;
//     render_pass_info.pDependencies = &dependency;
//
//     VkRenderPass dummy_render_pass;
//
//     VK_CHECK_RESULT(vkCreateRenderPass(VulkanGraphicsContext::Get()->GetDevice()->Raw(), &render_pass_info, nullptr, &dummy_render_pass))
//
//     return dummy_render_pass;
// }

#pragma region converts
constexpr VkFormat convert(const ShaderDataType& type)
{
    switch (type) {
    case ShaderDataType::INT:
        return VK_FORMAT_R32_SINT;
    case ShaderDataType::INT2:
        return VK_FORMAT_R32G32_SINT;
    case ShaderDataType::INT3:
        return VK_FORMAT_R32G32B32_SINT;
    case ShaderDataType::INT4:
        return VK_FORMAT_R32G32B32A32_SINT;
    case ShaderDataType::FLOAT:
        return VK_FORMAT_R32_SFLOAT;
    case ShaderDataType::FLOAT2:
        return VK_FORMAT_R32G32_SFLOAT;
    case ShaderDataType::FLOAT3:
        return VK_FORMAT_R32G32B32_SFLOAT;
    case ShaderDataType::FLOAT4:
        return VK_FORMAT_R32G32B32A32_SFLOAT;
    case ShaderDataType::IMAT3:
        return VK_FORMAT_R32G32B32_SINT;
    case ShaderDataType::IMAT4:
        return VK_FORMAT_R32G32B32A32_SINT;
    case ShaderDataType::MAT3:
        return VK_FORMAT_R32G32B32_SFLOAT;
    case ShaderDataType::MAT4:
        return VK_FORMAT_R32G32B32A32_SFLOAT;
    default:
        TB_CORE_ASSERT(false);
        return VK_FORMAT_R32_SINT;
    }
}

constexpr VkPrimitiveTopology convert(const PipelineTopology& topology)
{
    switch (topology) {
    case PipelineTopology::TRIANGLES:
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    case PipelineTopology::LINES:
        return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    case PipelineTopology::POINTS:
        return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    default:
        TB_CORE_ASSERT(false);
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    }
}

constexpr VkPolygonMode convert(const PipelineFillMode& mode)
{
    switch (mode) {
    case PipelineFillMode::FILL:
        return VK_POLYGON_MODE_FILL;
    case PipelineFillMode::EDGE_ONLY:
        return VK_POLYGON_MODE_LINE;
    default:
        TB_CORE_ASSERT(false);
        return VK_POLYGON_MODE_FILL;
    }
}

constexpr VkCullModeFlagBits convert(const PipelineCullingMode& mode)
{
    switch (mode) {
    case PipelineCullingMode::BACK:
        return VK_CULL_MODE_BACK_BIT;
    case PipelineCullingMode::FRONT:
        return VK_CULL_MODE_FRONT_BIT;
    case PipelineCullingMode::NONE:
        return VK_CULL_MODE_NONE;
    default:
        TB_CORE_ASSERT(false);
        return VK_CULL_MODE_BACK_BIT;
    }
}

constexpr VkFrontFace convert(const PipelineFrontFace face)
{
    switch (face) {
    case PipelineFrontFace::CLOCKWISE:
        return VK_FRONT_FACE_CLOCKWISE;
    case PipelineFrontFace::COUNTER_CLOCKWISE:
        return VK_FRONT_FACE_COUNTER_CLOCKWISE;
    default:
        TB_CORE_ASSERT(false);
        return VK_FRONT_FACE_CLOCKWISE;
    }
}

#pragma endregion

VulkanPipeline::VulkanPipeline(const VulkanPipelineSpecification& spec)
    : m_Specification(spec)
    , m_Pipeline(VK_NULL_HANDLE)
    , m_CurrentRenderPass(nullptr)
{
}

void VulkanPipeline::Create(Shared<VulkanRenderPass> current_render_pass)
{
    switch (m_Specification.type) {
    case PipelineType::GRAPHICS:
        CreateGraphics(current_render_pass);
        break;
    case PipelineType::COMPUTE:
        CreateCompute();
        break;
    default:
        break;
    }
}

VulkanPipeline::~VulkanPipeline()
{
    auto device = VulkanGraphicsContext::Get()->GetDevice();
    vkDestroyPipelineLayout(device->Raw(), m_PipelineLayout, nullptr);
    vkDestroyPipeline(device->Raw(), m_Pipeline, nullptr);
}

void VulkanPipeline::Destroy()
{
    auto device = VulkanGraphicsContext::Get()->GetDevice();
    vkDestroyPipeline(device->Raw(), m_Pipeline, nullptr);
    vkDestroyPipelineLayout(device->Raw(), m_PipelineLayout, nullptr);
}

void VulkanPipeline::CreateGraphics(Shared<VulkanRenderPass> render_pass)
{
    auto device = VulkanGraphicsContext::Get()->GetDevice();

    // ==================
    // Vertex input state

    VkVertexInputBindingDescription vertex_input_binding = {};
    vertex_input_binding.binding = 0;
    vertex_input_binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    vertex_input_binding.stride = m_Specification.input_layout.GetStride();
    std::vector<VkVertexInputAttributeDescription> vertex_input_attributes;

    VkPipelineVertexInputStateCreateInfo vertex_input_state = {};
    vertex_input_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    if (m_Specification.input_layout.GetStride()) {

        // Computing locations
        int previous_location_width = 0;
        for (const auto& element : m_Specification.input_layout.GetElements()) {
            uint32_t location;
            float location_compute_result = (float)element.size / 16.0f;
            if (location_compute_result <= 1.0f) {
                location = previous_location_width;
                previous_location_width += 1;
            } else {
                location = previous_location_width;
                previous_location_width += (int)location_compute_result;
            }

            vertex_input_attributes.push_back({ .location = location,
                .binding = 0,
                .format = convert(element.format),
                .offset = element.offset });
        }

        vertex_input_state.vertexBindingDescriptionCount = 1;
        vertex_input_state.pVertexBindingDescriptions = &vertex_input_binding;
        vertex_input_state.pVertexAttributeDescriptions = vertex_input_attributes.data();
        vertex_input_state.vertexAttributeDescriptionCount = (uint32_t)vertex_input_attributes.size();
    } else {
        vertex_input_state.vertexBindingDescriptionCount = 0;
        vertex_input_state.pVertexBindingDescriptions = nullptr;
        vertex_input_state.vertexAttributeDescriptionCount = 0;
        vertex_input_state.pVertexAttributeDescriptions = nullptr;
    }

    VkPipelineInputAssemblyStateCreateInfo input_assembly_state = {};
    input_assembly_state.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly_state.topology = convert(m_Specification.topology);
    input_assembly_state.primitiveRestartEnable = m_Specification.primitive_restart_enable;

    VkDynamicState dynamic_states[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamic_state = {};
    dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state.pDynamicStates = dynamic_states;
    dynamic_state.dynamicStateCount = 2;

    VkPipelineRasterizationStateCreateInfo rasterization_state = {};
    rasterization_state.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterization_state.depthClampEnable = VK_FALSE;
    rasterization_state.rasterizerDiscardEnable = VK_FALSE;
    rasterization_state.polygonMode = m_Specification.fill_mode == PipelineFillMode::FILL ? VK_POLYGON_MODE_FILL : VK_POLYGON_MODE_LINE;
    rasterization_state.lineWidth = m_Specification.line_width;
    rasterization_state.cullMode = convert(m_Specification.culling_mode);
    rasterization_state.frontFace = convert(m_Specification.front_face);

    Vector2 current_swapchain_extent = (Vector2)VulkanGraphicsContext::Get()->GetSwapchain()->GetSpecification().extent;
    VkViewport viewport = {};
    viewport = { 0, current_swapchain_extent.y, current_swapchain_extent.x, -current_swapchain_extent.y, 0.0f, 1.0f };

    VkRect2D scissor = {};
    scissor.extent = { (uint32_t)current_swapchain_extent.x, (uint32_t)current_swapchain_extent.y };
    scissor.offset = { 0, 0 };

    VkPipelineViewportStateCreateInfo viewport_state = {};
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = 1;
    viewport_state.pViewports = &viewport;
    viewport_state.scissorCount = 1;
    viewport_state.pScissors = &scissor;

    std::vector<VkPipelineColorBlendAttachmentState> blend_states(m_Specification.output_attachments_formats.size());
    for (auto& state : blend_states) {
        state.blendEnable = m_Specification.color_blending_enable;
        state.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        state.colorBlendOp = VK_BLEND_OP_ADD;
        state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        state.alphaBlendOp = VK_BLEND_OP_ADD;
        state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    }

    VkPipelineColorBlendStateCreateInfo color_blend_state = {};
    color_blend_state.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blend_state.attachmentCount = blend_states.size();
    color_blend_state.pAttachments = blend_states.data();
    color_blend_state.logicOpEnable = VK_FALSE;
    color_blend_state.logicOp = VK_LOGIC_OP_COPY;

    VkPipelineDepthStencilStateCreateInfo depth_stencil_state = {};
    depth_stencil_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil_state.depthTestEnable = m_Specification.depth_test_enable;
    depth_stencil_state.depthWriteEnable = m_Specification.depth_test_enable;
    depth_stencil_state.depthCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL;
    depth_stencil_state.depthBoundsTestEnable = VK_FALSE;
    depth_stencil_state.stencilTestEnable = VK_FALSE;

    uint32_t supported_multisampling_flags = device->GetPhysicalDevice()->GetProperties().properties.limits.framebufferColorSampleCounts;

    if (!(supported_multisampling_flags & m_Specification.sample_count)) {
        uint32_t max_supported_multisampling = 1;
        for (uint32_t i = (supported_multisampling_flags >> 1); i != 0; i >>= 1)
            max_supported_multisampling *= 2;

        TB_CORE_WARN("Requested to create pipeline \"{0}\" with multisample count \"{1}\". Setting maximum supported multisample count ({2})",
            m_Specification.debug_name, m_Specification.sample_count, max_supported_multisampling);

        m_Specification.sample_count = max_supported_multisampling;
    }

    VkPipelineMultisampleStateCreateInfo multisample_state = {};
    multisample_state.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample_state.sampleShadingEnable = m_Specification.multisampling_enable;
    multisample_state.rasterizationSamples = (VkSampleCountFlagBits)m_Specification.sample_count;

    auto* vk_shader = (VulkanShader*)m_Specification.shader;
    std::vector<VkDescriptorSetLayout> descriptor_set_layouts = vk_shader->GetLayouts();
    std::vector<VkPushConstantRange> push_constant_ranges = vk_shader->GetRanges();

    VkPipelineLayoutCreateInfo pipeline_layout_create_info = {};
    pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_create_info.setLayoutCount = descriptor_set_layouts.size();
    pipeline_layout_create_info.pSetLayouts = descriptor_set_layouts.data();
    pipeline_layout_create_info.pushConstantRangeCount = push_constant_ranges.size();
    pipeline_layout_create_info.pPushConstantRanges = push_constant_ranges.data();

    VK_CHECK_RESULT(vkCreatePipelineLayout(device->Raw(), &pipeline_layout_create_info, nullptr, &m_PipelineLayout));

    std::vector<VkFormat> formats;
    for (const auto& format : m_Specification.output_attachments_formats)
        formats.push_back(convert(format));

    std::vector<VkPipelineShaderStageCreateInfo> stage_infos = vk_shader->GetCreateInfos();

    VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = {};
    graphics_pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphics_pipeline_create_info.pStages = stage_infos.data();
    graphics_pipeline_create_info.stageCount = stage_infos.size();
    graphics_pipeline_create_info.pVertexInputState = &vertex_input_state;
    graphics_pipeline_create_info.pInputAssemblyState = &input_assembly_state;
    graphics_pipeline_create_info.pDynamicState = &dynamic_state;
    graphics_pipeline_create_info.pRasterizationState = &rasterization_state;
    graphics_pipeline_create_info.pColorBlendState = &color_blend_state;
    graphics_pipeline_create_info.pDepthStencilState = &depth_stencil_state;
    graphics_pipeline_create_info.pMultisampleState = &multisample_state;
    graphics_pipeline_create_info.layout = m_PipelineLayout;
    graphics_pipeline_create_info.pViewportState = &viewport_state;
    graphics_pipeline_create_info.renderPass = render_pass->Raw();

    VkResult result = vkCreateGraphicsPipelines(device->Raw(), VK_NULL_HANDLE, 1, &graphics_pipeline_create_info, nullptr, &m_Pipeline);
    if (result != VK_SUCCESS) {
        TB_CORE_ERROR("Failed to create pipeline \"{0}\".", m_Specification.debug_name);
        return;
    }

    TB_CORE_TRACE("Pipeline \"{0}\" created successfully", m_Specification.debug_name);
    m_CurrentRenderPass = render_pass;
}

void VulkanPipeline::CreateCompute()
{
    auto device = VulkanGraphicsContext::Get()->GetDevice();

    auto* vk_shader = (VulkanShader*)m_Specification.shader;
    std::vector<VkDescriptorSetLayout> descriptor_set_layouts = vk_shader->GetLayouts();
    std::vector<VkPushConstantRange> push_constant_ranges = vk_shader->GetRanges();

    VkPipelineLayoutCreateInfo pipeline_layout_create_info = {};
    pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_create_info.setLayoutCount = descriptor_set_layouts.size();
    pipeline_layout_create_info.pSetLayouts = descriptor_set_layouts.data();
    pipeline_layout_create_info.pushConstantRangeCount = push_constant_ranges.size();
    pipeline_layout_create_info.pPushConstantRanges = push_constant_ranges.data();

    VK_CHECK_RESULT(vkCreatePipelineLayout(device->Raw(), &pipeline_layout_create_info, nullptr, &m_PipelineLayout));

    std::vector<VkPipelineShaderStageCreateInfo> stage_create_info = vk_shader->GetCreateInfos();

    VkComputePipelineCreateInfo compute_pipeline_create_info = {};
    compute_pipeline_create_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    compute_pipeline_create_info.layout = m_PipelineLayout;
    compute_pipeline_create_info.stage = stage_create_info[0];

    VkResult result = vkCreateComputePipelines(device->Raw(), VK_NULL_HANDLE, 1, &compute_pipeline_create_info, nullptr, &m_Pipeline);
    if (result != VK_SUCCESS) {
        TB_CORE_ERROR("Failed to create pipeline \"{0}\".", m_Specification.debug_name);
        return;
    }

    TB_CORE_TRACE("Pipeline \"{0}\" created successfully", m_Specification.debug_name);
}

// VulkanPipeline::VulkanPipeline(const VulkanPipelineSpecification& spec)
//     : m_Specification(spec)
// {
//     switch (spec.type) {
//     case PipelineType::GRAPHICS:
//         CreateGraphics();
//         break;
//     case PipelineType::COMPUTE:
//         CreateCompute();
//         break;
//     default:
//         TB_CORE_ASSERT(false);
//         break;
//     }
// }
//
// VulkanPipeline::~VulkanPipeline()
// {
//     if (m_Pipeline != VK_NULL_HANDLE)
//         Destroy();
// }
//
// void VulkanPipeline::Destroy()
// {
//     auto device = VulkanGraphicsContext::Get()->GetDevice();
//     vkDestroyPipeline(device->Raw(), m_Pipeline, nullptr);
//     vkDestroyPipelineLayout(device->Raw(), m_PipelineLayout, nullptr);
//     m_Pipeline = VK_NULL_HANDLE;
//     m_PipelineLayout = VK_NULL_HANDLE;
// }
//
// void VulkanPipeline::CreateGraphics()
// {
//     auto device = VulkanGraphicsContext::Get()->GetDevice();
//
//     // ==================
//     // Vertex input state
//
//     VkVertexInputBindingDescription vertex_input_binding = {};
//     vertex_input_binding.binding = 0;
//     vertex_input_binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
//     vertex_input_binding.stride = m_Specification.input_layout.GetStride();
//     std::vector<VkVertexInputAttributeDescription> vertex_input_attributes;
//
//     VkPipelineVertexInputStateCreateInfo vertex_input_state = {};
//     vertex_input_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
//
//     if (m_Specification.input_layout.GetStride()) {
//
//         // Computing locations
//         int previous_location_width = 0;
//         for (const auto& element : m_Specification.input_layout.GetElements()) {
//             uint32_t location;
//             float location_compute_result = (float)element.size / 16.0f;
//             if (location_compute_result <= 1.0f) {
//                 location = previous_location_width;
//                 previous_location_width += 1;
//             } else {
//                 location = previous_location_width;
//                 previous_location_width += (int)location_compute_result;
//             }
//
//             vertex_input_attributes.push_back({ .location = location,
//                 .binding = 0,
//                 .format = convert(element.format),
//                 .offset = element.offset });
//         }
//
//         vertex_input_state.vertexBindingDescriptionCount = 1;
//         vertex_input_state.pVertexBindingDescriptions = &vertex_input_binding;
//         vertex_input_state.pVertexAttributeDescriptions = vertex_input_attributes.data();
//         vertex_input_state.vertexAttributeDescriptionCount = (uint32_t)vertex_input_attributes.size();
//     } else {
//         vertex_input_state.vertexBindingDescriptionCount = 0;
//         vertex_input_state.pVertexBindingDescriptions = nullptr;
//         vertex_input_state.vertexAttributeDescriptionCount = 0;
//         vertex_input_state.pVertexAttributeDescriptions = nullptr;
//     }
//
//     VkPipelineInputAssemblyStateCreateInfo input_assembly_state = {};
//     input_assembly_state.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
//     input_assembly_state.topology = convert(m_Specification.topology);
//     input_assembly_state.primitiveRestartEnable = m_Specification.primitive_restart_enable;
//
//     VkDynamicState dynamic_states[] = {
//         VK_DYNAMIC_STATE_VIEWPORT,
//         VK_DYNAMIC_STATE_SCISSOR
//     };
//
//     VkPipelineDynamicStateCreateInfo dynamic_state = {};
//     dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
//     dynamic_state.pDynamicStates = dynamic_states;
//     dynamic_state.dynamicStateCount = 2;
//
//     VkPipelineRasterizationStateCreateInfo rasterization_state = {};
//     rasterization_state.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
//     rasterization_state.depthClampEnable = VK_FALSE;
//     rasterization_state.rasterizerDiscardEnable = VK_FALSE;
//     rasterization_state.polygonMode = m_Specification.fill_mode == PipelineFillMode::FILL ? VK_POLYGON_MODE_FILL : VK_POLYGON_MODE_LINE;
//     rasterization_state.lineWidth = m_Specification.line_width;
//     rasterization_state.cullMode = convert(m_Specification.culling_mode);
//     rasterization_state.frontFace = convert(m_Specification.front_face);
//
//     // Vector2 current_swapchain_extent = { VulkanGraphicsContext::Get()->GetSwapchain()->RawExtend().width,
//     //     VulkanGraphicsContext::Get()->GetSwapchain()->GetSpecification().extent.};
//
//     UIntVector2 current_swapchain_extent = VulkanGraphicsContext::Get()->GetSwapchain()->GetSpecification().extent;
//
//     VkViewport viewport = {};
//     viewport = { 0, (float)current_swapchain_extent.y, (float)current_swapchain_extent.x, (float)-current_swapchain_extent.y, 0.0f, 1.0f };
//
//     VkRect2D scissor = {};
//     scissor.extent = { (uint32_t)current_swapchain_extent.x, (uint32_t)current_swapchain_extent.y };
//     scissor.offset = { 0, 0 };
//
//     VkPipelineViewportStateCreateInfo viewport_state = {};
//     viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
//     viewport_state.viewportCount = 1;
//     viewport_state.pViewports = &viewport;
//     viewport_state.scissorCount = 1;
//     viewport_state.pScissors = &scissor;
//
//     std::vector<VkPipelineColorBlendAttachmentState> blend_states(m_Specification.output_attachments_formats.size());
//     for (auto& state : blend_states) {
//         state.blendEnable = m_Specification.color_blending_enable;
//         state.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
//         state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
//         state.colorBlendOp = VK_BLEND_OP_ADD;
//         state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
//         state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
//         state.alphaBlendOp = VK_BLEND_OP_ADD;
//         state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
//     }
//
//     VkPipelineColorBlendStateCreateInfo color_blend_state = {};
//     color_blend_state.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
//     color_blend_state.attachmentCount = blend_states.size();
//     color_blend_state.pAttachments = blend_states.data();
//     color_blend_state.logicOpEnable = VK_FALSE;
//     color_blend_state.logicOp = VK_LOGIC_OP_COPY;
//     color_blend_state.blendConstants[0] = 0.0f;
//     color_blend_state.blendConstants[1] = 0.0f;
//     color_blend_state.blendConstants[2] = 0.0f;
//     color_blend_state.blendConstants[3] = 0.0f;
//
//     VkPipelineDepthStencilStateCreateInfo depth_stencil_state = {};
//     depth_stencil_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
//     depth_stencil_state.depthTestEnable = m_Specification.depth_test_enable;
//     depth_stencil_state.depthWriteEnable = m_Specification.depth_test_enable;
//     depth_stencil_state.depthCompareOp = VK_COMPARE_OP_LESS;
//     depth_stencil_state.depthBoundsTestEnable = VK_FALSE;
//     depth_stencil_state.stencilTestEnable = VK_FALSE;
//
//     uint32_t supported_multisampling_flags = device->GetPhysicalDevice()->GetProperties().properties.limits.framebufferColorSampleCounts;
//
//     if (!(supported_multisampling_flags & m_Specification.sample_count)) {
//         uint32_t max_supported_multisampling = 1;
//         for (uint32_t i = (supported_multisampling_flags >> 1); i != 0; i >>= 1)
//             max_supported_multisampling *= 2;
//
//         TB_CORE_WARN("Requested to create pipeline \"{0}\" with multisample count \"{1}\". Setting maximum supported multisample count ({2})",
//             m_Specification.debug_name, m_Specification.sample_count, max_supported_multisampling);
//
//         m_Specification.sample_count = max_supported_multisampling;
//     }
//
//     VkPipelineMultisampleStateCreateInfo multisample_state = {};
//     multisample_state.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
//     multisample_state.sampleShadingEnable = m_Specification.multisampling_enable;
//     multisample_state.rasterizationSamples = (VkSampleCountFlagBits)m_Specification.sample_count;
//
//     VulkanShader* vk_shader = (VulkanShader*)m_Specification.shader;
//     std::vector<VkDescriptorSetLayout> descriptor_set_layouts = vk_shader->GetLayouts();
//     std::vector<VkPushConstantRange> push_constant_ranges = vk_shader->GetRanges();
//
//     VkPipelineLayoutCreateInfo pipeline_layout_create_info = {};
//     pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
//     pipeline_layout_create_info.setLayoutCount = descriptor_set_layouts.size();
//     pipeline_layout_create_info.pSetLayouts = descriptor_set_layouts.data();
//     pipeline_layout_create_info.pushConstantRangeCount = push_constant_ranges.size();
//     pipeline_layout_create_info.pPushConstantRanges = push_constant_ranges.data();
//
//     VK_CHECK_RESULT(vkCreatePipelineLayout(device->Raw(), &pipeline_layout_create_info, nullptr, &m_PipelineLayout));
//
//     std::vector<VkFormat> formats;
//     for (const auto& format : m_Specification.output_attachments_formats)
//         formats.push_back(convert(format));
//
//     VkPipelineRenderingCreateInfo pipeline_rendering = {};
//     pipeline_rendering.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
//     pipeline_rendering.colorAttachmentCount = formats.size();
//     pipeline_rendering.pColorAttachmentFormats = formats.data();
//     pipeline_rendering.depthAttachmentFormat = VK_FORMAT_D32_SFLOAT_S8_UINT;
//
//     std::vector<VkPipelineShaderStageCreateInfo> stage_infos = vk_shader->GetCreateInfos();
//     VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = {};
//     graphics_pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
//     graphics_pipeline_create_info.pNext = &pipeline_rendering;
//     graphics_pipeline_create_info.pStages = stage_infos.data();
//     graphics_pipeline_create_info.stageCount = stage_infos.size();
//     graphics_pipeline_create_info.pVertexInputState = &vertex_input_state;
//     graphics_pipeline_create_info.pInputAssemblyState = &input_assembly_state;
//     graphics_pipeline_create_info.pDynamicState = &dynamic_state;
//     graphics_pipeline_create_info.pRasterizationState = &rasterization_state;
//     graphics_pipeline_create_info.pColorBlendState = &color_blend_state;
//     graphics_pipeline_create_info.pDepthStencilState = &depth_stencil_state;
//     graphics_pipeline_create_info.pMultisampleState = &multisample_state;
//     graphics_pipeline_create_info.layout = m_PipelineLayout;
//     graphics_pipeline_create_info.pViewportState = &viewport_state;
//     graphics_pipeline_create_info.renderPass = VK_NULL_HANDLE;
//
//     // VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = {};
//     // graphics_pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
//     // graphics_pipeline_create_info.pNext = &pipeline_rendering;
//     // graphics_pipeline_create_info.pStages = stage_infos.data();
//     // graphics_pipeline_create_info.stageCount = stage_infos.size();
//     // graphics_pipeline_create_info.pVertexInputState = &vertex_input_state;
//     // graphics_pipeline_create_info.pInputAssemblyState = &input_assembly_state;
//     // graphics_pipeline_create_info.pDynamicState = &dynamic_state;
//     // graphics_pipeline_create_info.pRasterizationState = &rasterization_state;
//     // graphics_pipeline_create_info.pColorBlendState = &color_blend_state;
//     // graphics_pipeline_create_info.pDepthStencilState = &depth_stencil_state;
//     // graphics_pipeline_create_info.pMultisampleState = &multisample_state;
//     // graphics_pipeline_create_info.layout = m_PipelineLayout;
//     // graphics_pipeline_create_info.pViewportState = &viewport_state;
//     // // graphics_pipeline_create_info.renderPass = VulkanGraphicsContext::Get()->GetRenderPass()->Raw();
//     // // graphics_pipeline_create_info.subpass = 0;
//     // graphics_pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
//
//     VkResult result = vkCreateGraphicsPipelines(device->Raw(), VK_NULL_HANDLE, 1, &graphics_pipeline_create_info, nullptr, &m_Pipeline);
//     if (result != VK_SUCCESS) {
//         TB_CORE_ERROR("Failed to create pipeline \"{0}\".", m_Specification.debug_name);
//         return;
//     }
//
//     TB_CORE_TRACE("Pipeline \"{0}\" created successfully", m_Specification.debug_name);
// }
//
// void VulkanPipeline::CreateCompute()
// {
//     auto device = VulkanGraphicsContext::Get()->GetDevice();
//
//     VulkanShader* vk_shader = (VulkanShader*)m_Specification.shader;
//     std::vector<VkDescriptorSetLayout> descriptor_set_layouts = vk_shader->GetLayouts();
//     std::vector<VkPushConstantRange> push_constant_ranges = vk_shader->GetRanges();
//
//     VkPipelineLayoutCreateInfo pipeline_layout_create_info = {};
//     pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
//     pipeline_layout_create_info.setLayoutCount = descriptor_set_layouts.size();
//     pipeline_layout_create_info.pSetLayouts = descriptor_set_layouts.data();
//     pipeline_layout_create_info.pushConstantRangeCount = push_constant_ranges.size();
//     pipeline_layout_create_info.pPushConstantRanges = push_constant_ranges.data();
//
//     VK_CHECK_RESULT(vkCreatePipelineLayout(device->Raw(), &pipeline_layout_create_info, nullptr, &m_PipelineLayout));
//
//     std::vector<VkPipelineShaderStageCreateInfo> stage_create_info = vk_shader->GetCreateInfos();
//
//     VkComputePipelineCreateInfo compute_pipeline_create_info = {};
//     compute_pipeline_create_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
//     compute_pipeline_create_info.layout = m_PipelineLayout;
//     compute_pipeline_create_info.stage = stage_create_info[0];
//
//     VkResult result = vkCreateComputePipelines(device->Raw(), VK_NULL_HANDLE, 1, &compute_pipeline_create_info, nullptr, &m_Pipeline);
//     if (result != VK_SUCCESS) {
//         TB_CORE_ERROR("Failed to create pipeline \"{0}\".", m_Specification.debug_name);
//         return;
//     }
//
//     TB_CORE_TRACE("Pipeline \"{0}\" created successfully", m_Specification.debug_name);
// }

}
