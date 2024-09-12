#include "VulkanSwapchain.h"
#include "VulkanShader.h"
#include "VulkanDevice.h"
#include "VulkanImage.h"

#include "VulkanGraphicsContext.h"

#include <spirv_reflect.c>

namespace Tabby {

#pragma region converts
constexpr VkShaderStageFlagBits convert(const ShaderStage& stage)
{
    switch (stage) {
    case ShaderStage::VERTEX:
        return VK_SHADER_STAGE_VERTEX_BIT;
    case ShaderStage::FRAGMENT:
        return VK_SHADER_STAGE_FRAGMENT_BIT;
    case ShaderStage::COMPUTE:
        return VK_SHADER_STAGE_COMPUTE_BIT;
    case ShaderStage::TASK:
        return VK_SHADER_STAGE_TASK_BIT_EXT;
    case ShaderStage::MESH:
        return VK_SHADER_STAGE_MESH_BIT_EXT;
    default:
        return (VkShaderStageFlagBits)0;
    }
}

constexpr VkDescriptorType convert(SpvReflectDescriptorType type)
{
    switch (type) {
    case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:
        return VK_DESCRIPTOR_TYPE_SAMPLER;
    case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
        return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
        return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:
        return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
        return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
    case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
        return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
    case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:
        return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
        return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
    case SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
        return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
    case SPV_REFLECT_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:
        return VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
    default:
        TB_CORE_ASSERT(false);
        return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    }
}

const std::string DescriptorToString(VkDescriptorType type)
{
    switch (type) {
    case VK_DESCRIPTOR_TYPE_SAMPLER:
        return "sampler";
    case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
        return "combined image sampler";
    case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
        return "sampled image";
    case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
        return "storage image";
    case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
        return "uniform texel buffer";
    case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
        return "storage texel buffer";
    case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
        return "uniform buffer";
    case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
        return "storage buffer";
    default:
        TB_CORE_ASSERT(false);
        return "sampler";
    }
}
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

VulkanShader::VulkanShader(const ShaderSpecification& spec, std::map<ShaderStage, std::vector<uint32_t>> binaries)
    : m_Specification(spec)
{
    auto device = VulkanGraphicsContext::Get()->GetDevice();

    // set id and its bindings
    std::map<uint32_t, std::vector<VkDescriptorSetLayoutBinding>> bindings;
    VkPushConstantRange push_constant_range = {};

    TB_CORE_TRACE("=======================");
    TB_CORE_TRACE("Reflecting shader - {0}", spec.path.filename().string());
    TB_CORE_TRACE("=======================");

    for (auto& stage_data : binaries) {
        VkShaderModule shader_module;

        VkShaderModuleCreateInfo shader_module_create_info = {};
        shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shader_module_create_info.pCode = stage_data.second.data();
        shader_module_create_info.codeSize = stage_data.second.size() * 4;

        VK_CHECK_RESULT(vkCreateShaderModule(device->Raw(), &shader_module_create_info, nullptr, &shader_module));

        VkPipelineShaderStageCreateInfo shader_stage_create_info = {};
        shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stage_create_info.stage = convert(stage_data.first);
        shader_stage_create_info.pName = "main";
        shader_stage_create_info.module = shader_module;

        m_StageCreateInfos.push_back(shader_stage_create_info);

        SpvReflectShaderModule reflect_module;
        if (spvReflectCreateShaderModule(stage_data.second.size() * 4, stage_data.second.data(), &reflect_module) != SPV_REFLECT_RESULT_SUCCESS) {
            TB_CORE_ERROR("Failed to reflect shader {}", spec.path.filename().string());
            m_Dirty = true;
            return;
        }

        uint32_t set_count = 0;
        spvReflectEnumerateDescriptorSets(&reflect_module, &set_count, nullptr);

        std::vector<SpvReflectDescriptorSet*> reflect_descriptor_sets(set_count);
        spvReflectEnumerateDescriptorSets(&reflect_module, &set_count, reflect_descriptor_sets.data());

        for (auto& reflect_set : reflect_descriptor_sets) {
            if (bindings.find(reflect_set->set) == bindings.end()) {
                bindings.emplace(reflect_set->set, std::vector<VkDescriptorSetLayoutBinding>());
            }

            for (int i = 0; i < reflect_set->binding_count; i++) {
                SpvReflectDescriptorBinding* reflect_binding = reflect_set->bindings[i];

                VkDescriptorSetLayoutBinding layout_binding = {};
                layout_binding.binding = reflect_binding->binding;
                layout_binding.descriptorType = convert(reflect_binding->descriptor_type);
                layout_binding.descriptorCount = reflect_binding->count;
                layout_binding.stageFlags = stage_data.first == ShaderStage::COMPUTE ? VK_SHADER_STAGE_COMPUTE_BIT : VK_SHADER_STAGE_ALL;

                bool skip_binding = false;

                for (auto& set_binding : bindings[reflect_set->set]) {
                    if (set_binding.binding == layout_binding.binding) {
                        skip_binding = true;
                        break;
                    }
                }

                if (!skip_binding)
                    bindings[reflect_set->set].push_back(layout_binding);
            }
        }

        uint32_t push_constant_range_count = 0;
        spvReflectEnumeratePushConstantBlocks(&reflect_module, &push_constant_range_count, nullptr);

        std::vector<SpvReflectBlockVariable*> reflect_push_constant_ranges(push_constant_range_count);
        spvReflectEnumeratePushConstantBlocks(&reflect_module, &push_constant_range_count, reflect_push_constant_ranges.data());

        if (push_constant_range_count) {

            for (int i = 0; i < reflect_push_constant_ranges[0]->member_count; i++) {
                auto& member = reflect_push_constant_ranges[0]->members[i];
            }
        }

        for (auto& reflect_range : reflect_push_constant_ranges) {
            push_constant_range.size += reflect_range->size;
            push_constant_range.offset = 0;
            push_constant_range.stageFlags = VK_SHADER_STAGE_ALL;
        }

        spvReflectDestroyShaderModule(&reflect_module);
    }

    if (push_constant_range.size)
        m_Ranges.push_back(push_constant_range);

#if TB_DEBUG
    for (auto& set : bindings) {
        TB_CORE_TRACE("\tSet #{0}: ", set.first);
        for (auto& binding : set.second) {
            TB_CORE_TRACE("\t\t Binding #{0}: {1}[{2}]", binding.binding, DescriptorToString(binding.descriptorType), binding.descriptorCount);
        }
    }
    printf("\n");
#endif

    if (bindings.size()) {
        uint32_t highest_set_index = bindings.rbegin()->first;
        for (int i = 0; i <= highest_set_index; i++) {
            if (bindings.find(i) == bindings.end())
                m_SetLayouts.push_back(VK_NULL_HANDLE);
            else {
                std::vector<VkDescriptorBindingFlags> binding_flags;
                for (auto& binding : bindings[i]) {
                    if (binding.descriptorCount != 1)
                        binding_flags.push_back(VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT);
                    else
                        binding_flags.push_back(0);
                }

                VkDescriptorSetLayoutBindingFlagsCreateInfo layout_bindings_flags = {};
                layout_bindings_flags.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
                layout_bindings_flags.bindingCount = binding_flags.size();
                layout_bindings_flags.pBindingFlags = binding_flags.data();

                VkDescriptorSetLayoutCreateInfo layout_create_info = {};
                layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
                layout_create_info.pNext = &layout_bindings_flags;
                layout_create_info.bindingCount = bindings[i].size();
                layout_create_info.pBindings = bindings[i].data();

                VkDescriptorSetLayout vk_descriptor_set_layout = VK_NULL_HANDLE;
                vkCreateDescriptorSetLayout(device->Raw(), &layout_create_info, nullptr, &vk_descriptor_set_layout);
                m_SetLayouts.push_back(vk_descriptor_set_layout);
            }
        }
    }

    if (binaries.find(ShaderStage::VERTEX) != binaries.end() || binaries.find(ShaderStage::FRAGMENT) != binaries.end()) {
        CreateGraphicsPipeline();
    } else {
        CreateComputePipeline();
    }
}

VulkanShader::~VulkanShader()
{
}

void VulkanShader::Destroy()
{
    auto device = VulkanGraphicsContext::Get()->GetDevice();

    for (auto& stage : m_StageCreateInfos)
        if (stage.module != VK_NULL_HANDLE)
            vkDestroyShaderModule(device->Raw(), stage.module, nullptr);

    for (auto& layout : m_SetLayouts)
        vkDestroyDescriptorSetLayout(device->Raw(), layout, nullptr);

    m_SetLayouts.clear();
    for (auto& stage : m_StageCreateInfos)
        stage.module = VK_NULL_HANDLE;
}

void VulkanShader::CreateGraphicsPipeline()
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
                previous_location_width += (int32_t)location_compute_result;
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

    VkExtent2D current_swapchain_extent = VulkanGraphicsContext::Get()->GetSwapchain()->RawExtend();
    VkViewport viewport = {};
    viewport = { 0, (float)current_swapchain_extent.height, (float)current_swapchain_extent.width, (float)-current_swapchain_extent.height, 0.0f, 1.0f };

    VkRect2D scissor = {};
    scissor.extent = current_swapchain_extent;
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
        state.colorWriteMask = VK_COLOR_COMPONENT_FLAG_BITS_MAX_ENUM;
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
            m_Specification.pipeline_debug_name, m_Specification.sample_count, max_supported_multisampling);

        m_Specification.sample_count = max_supported_multisampling;
    }

    VkPipelineMultisampleStateCreateInfo multisample_state = {};
    multisample_state.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample_state.sampleShadingEnable = m_Specification.multisampling_enable;
    multisample_state.rasterizationSamples = (VkSampleCountFlagBits)m_Specification.sample_count;

    std::vector<VkDescriptorSetLayout> descriptor_set_layouts = GetLayouts();
    std::vector<VkPushConstantRange> push_constant_ranges = GetRanges();

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

    VkPipelineRenderingCreateInfo pipeline_rendering = {};
    pipeline_rendering.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    pipeline_rendering.colorAttachmentCount = formats.size();
    pipeline_rendering.pColorAttachmentFormats = formats.data();
    // HACK: I assume that D32_SFLOAT is chosen format. More proper way to do it is to request depth image format from pipeline spec or swapchain
    pipeline_rendering.depthAttachmentFormat = VK_FORMAT_D32_SFLOAT;

    std::vector<VkPipelineShaderStageCreateInfo> stage_infos = GetCreateInfos();

    VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = {};
    graphics_pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphics_pipeline_create_info.pNext = &pipeline_rendering;
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
    graphics_pipeline_create_info.renderPass = VK_NULL_HANDLE;

    VkResult result = vkCreateGraphicsPipelines(device->Raw(), VK_NULL_HANDLE, 1, &graphics_pipeline_create_info, nullptr, &m_Pipeline);
    if (result != VK_SUCCESS) {
        TB_CORE_ERROR("Failed to create pipeline \"{0}\".", m_Specification.pipeline_debug_name);
        return;
    }

    TB_CORE_TRACE("Pipeline \"{0}\" created successfully", m_Specification.pipeline_debug_name);
}

void VulkanShader::CreateComputePipeline()
{
    auto device = VulkanGraphicsContext::Get()->GetDevice();

    std::vector<VkDescriptorSetLayout> descriptor_set_layouts = GetLayouts();
    std::vector<VkPushConstantRange> push_constant_ranges = GetRanges();

    VkPipelineLayoutCreateInfo pipeline_layout_create_info = {};
    pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_create_info.setLayoutCount = descriptor_set_layouts.size();
    pipeline_layout_create_info.pSetLayouts = descriptor_set_layouts.data();
    pipeline_layout_create_info.pushConstantRangeCount = push_constant_ranges.size();
    pipeline_layout_create_info.pPushConstantRanges = push_constant_ranges.data();

    VK_CHECK_RESULT(vkCreatePipelineLayout(device->Raw(), &pipeline_layout_create_info, nullptr, &m_PipelineLayout));

    std::vector<VkPipelineShaderStageCreateInfo> stage_create_info = GetCreateInfos();

    VkComputePipelineCreateInfo compute_pipeline_create_info = {};
    compute_pipeline_create_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    compute_pipeline_create_info.layout = m_PipelineLayout;
    compute_pipeline_create_info.stage = stage_create_info[0];

    VkResult result = vkCreateComputePipelines(device->Raw(), VK_NULL_HANDLE, 1, &compute_pipeline_create_info, nullptr, &m_Pipeline);
    if (result != VK_SUCCESS) {
        TB_CORE_ERROR("Failed to create pipeline \"{0}\".", m_Specification.pipeline_debug_name);
        return;
    }

    TB_CORE_TRACE("Pipeline \"{0}\" created successfully", m_Specification.pipeline_debug_name);
}

// void VulkanShader::RestoreShaderModule(std::filesystem::path path)
// {
// }

}

// #include "VulkanRenderPass.h"
// #include "VulkanShader.h"
// #include "VulkanDevice.h"
//
// #include "VulkanGraphicsContext.h"
//
// namespace Tabby {
//
// constexpr VkShaderStageFlagBits convert(const ShaderStage& stage)
// {
//     switch (stage) {
//     case ShaderStage::VERTEX:
//         return VK_SHADER_STAGE_VERTEX_BIT;
//     case ShaderStage::FRAGMENT:
//         return VK_SHADER_STAGE_FRAGMENT_BIT;
//     case ShaderStage::COMPUTE:
//         return VK_SHADER_STAGE_COMPUTE_BIT;
//     case ShaderStage::TASK:
//         return VK_SHADER_STAGE_TASK_BIT_EXT;
//     case ShaderStage::MESH:
//         return VK_SHADER_STAGE_MESH_BIT_EXT;
//     default:
//         return (VkShaderStageFlagBits)0;
//     }
// }
//
// VulkanShader::VulkanShader(std::map<ShaderStage, std::vector<uint8_t>> binaries)
// {
//     auto device = VulkanGraphicsContext::Get()->GetDevice();
//
//     for (const auto& binary : binaries) {
//         VkShaderModuleCreateInfo createInfo {};
//         createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
//         createInfo.codeSize = binary.second.size();
//         createInfo.pCode = reinterpret_cast<const uint32_t*>(binary.second.data());
//
//         VkShaderModule shaderModule;
//         if (vkCreateShaderModule(device->Raw(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
//             throw std::runtime_error("failed to create shader module!");
//         }
//
//         VkPipelineShaderStageCreateInfo vertShaderStageInfo {};
//         vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//         vertShaderStageInfo.stage = convert(binary.first);
//         vertShaderStageInfo.module = shaderModule;
//         vertShaderStageInfo.pName = "main";
//
//         m_StageCreateInfos.push_back(vertShaderStageInfo);
//     }
//
//     VkPipelineVertexInputStateCreateInfo vertexInputInfo {};
//     vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
//     vertexInputInfo.vertexBindingDescriptionCount = 0;
//     vertexInputInfo.vertexAttributeDescriptionCount = 0;
//
//     VkPipelineInputAssemblyStateCreateInfo inputAssembly {};
//     inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
//     inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
//     inputAssembly.primitiveRestartEnable = VK_FALSE;
//
//     VkPipelineViewportStateCreateInfo viewportState {};
//     viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
//     viewportState.viewportCount = 1;
//     viewportState.scissorCount = 1;
//
//     VkPipelineRasterizationStateCreateInfo rasterizer {};
//     rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
//     rasterizer.depthClampEnable = VK_FALSE;
//     rasterizer.rasterizerDiscardEnable = VK_FALSE;
//     rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
//     rasterizer.lineWidth = 1.0f;
//     rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
//     rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
//     rasterizer.depthBiasEnable = VK_FALSE;
//
//     VkPipelineMultisampleStateCreateInfo multisampling {};
//     multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
//     multisampling.sampleShadingEnable = VK_FALSE;
//     multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
//
//     VkPipelineColorBlendAttachmentState colorBlendAttachment {};
//     colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
//     colorBlendAttachment.blendEnable = VK_FALSE;
//
//     VkPipelineColorBlendStateCreateInfo colorBlending {};
//     colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
//     colorBlending.logicOpEnable = VK_FALSE;
//     colorBlending.logicOp = VK_LOGIC_OP_COPY;
//     colorBlending.attachmentCount = 1;
//     colorBlending.pAttachments = &colorBlendAttachment;
//     colorBlending.blendConstants[0] = 0.0f;
//     colorBlending.blendConstants[1] = 0.0f;
//     colorBlending.blendConstants[2] = 0.0f;
//     colorBlending.blendConstants[3] = 0.0f;
//
//     std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT,
//         VK_DYNAMIC_STATE_SCISSOR };
//     VkPipelineDynamicStateCreateInfo dynamicState {};
//     dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
//     dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
//     dynamicState.pDynamicStates = dynamicStates.data();
//
//     VkPipelineLayoutCreateInfo pipelineLayoutInfo {};
//     pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
//     pipelineLayoutInfo.setLayoutCount = 0;
//     pipelineLayoutInfo.pushConstantRangeCount = 0;
//
//     if (vkCreatePipelineLayout(device->Raw(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout)
//         != VK_SUCCESS) {
//         throw std::runtime_error("failed to create pipeline layout!");
//     }
//
//     VkGraphicsPipelineCreateInfo pipelineInfo {};
//     pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
//     pipelineInfo.stageCount = 2;
//     pipelineInfo.pStages = m_StageCreateInfos.data();
//     pipelineInfo.pVertexInputState = &vertexInputInfo;
//     pipelineInfo.pInputAssemblyState = &inputAssembly;
//     pipelineInfo.pViewportState = &viewportState;
//     pipelineInfo.pRasterizationState = &rasterizer;
//     pipelineInfo.pMultisampleState = &multisampling;
//     pipelineInfo.pColorBlendState = &colorBlending;
//     pipelineInfo.pDynamicState = &dynamicState;
//     pipelineInfo.layout = m_PipelineLayout;
//     pipelineInfo.renderPass = VulkanGraphicsContext::Get()->GetRenderPass()->Raw();
//     pipelineInfo.subpass = 0;
//     pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
//
//     if (vkCreateGraphicsPipelines(device->Raw(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline) != VK_SUCCESS) {
//         throw std::runtime_error("failed to create graphics pipeline!");
//     }
//
//     // // set id and its bindings
//     // std::map<uint32, std::vector<VkDescriptorSetLayoutBinding>> bindings;
//     // VkPushConstantRange push_constant_range = {};
//     //
//     // TB_CORE_TRACE("=======================");
//     // TB_CORE_TRACE("Reflecting shader - {0}", path.filename().string());
//     // TB_CORE_TRACE("=======================");
//
//     // for (auto& stage_data : binaries) {
//     //     VkShaderModule shader_module;
//     //
//     //     VkShaderModuleCreateInfo shader_module_create_info = {};
//     //     shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
//     //     shader_module_create_info.pCode = stage_data.second.data();
//     //     shader_module_create_info.codeSize = stage_data.second.size() * 4;
//     //
//     //     VK_CHECK_RESULT(vkCreateShaderModule(device->Raw(), &shader_module_create_info, nullptr, &shader_module));
//     //
//     //     VkPipelineShaderStageCreateInfo shader_stage_create_info = {};
//     //     shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//     //     shader_stage_create_info.stage = convert(stage_data.first);
//     //     shader_stage_create_info.pName = "main";
//     //     shader_stage_create_info.module = shader_module;
//     //
//     //     m_StageCreateInfos.push_back(shader_stage_create_info);
//     //
//     //     SpvReflectShaderModule reflect_module;
//     //     if (spvReflectCreateShaderModule(stage_data.second.size() * 4, stage_data.second.data(), &reflect_module) != SPV_REFLECT_RESULT_SUCCESS) {
//     //         TB_CORE_ERROR("Failed to reflect shader {}", path.filename().string());
//     //         m_Dirty = true;
//     //         return;
//     //     }
//     //
//     //     uint32 set_count = 0;
//     //     spvReflectEnumerateDescriptorSets(&reflect_module, &set_count, nullptr);
//     //
//     //     std::vector<SpvReflectDescriptorSet*> reflect_descriptor_sets(set_count);
//     //     spvReflectEnumerateDescriptorSets(&reflect_module, &set_count, reflect_descriptor_sets.data());
//     //
//     //     for (auto& reflect_set : reflect_descriptor_sets) {
//     //         if (!bindings.contains(reflect_set->set)) {
//     //             bindings.emplace(reflect_set->set, std::vector<VkDescriptorSetLayoutBinding>());
//     //         }
//     //
//     //         for (int i = 0; i < reflect_set->binding_count; i++) {
//     //             SpvReflectDescriptorBinding* reflect_binding = reflect_set->bindings[i];
//     //
//     //             VkDescriptorSetLayoutBinding layout_binding = {};
//     //             layout_binding.binding = reflect_binding->binding;
//     //             layout_binding.descriptorType = convert(reflect_binding->descriptor_type);
//     //             layout_binding.descriptorCount = reflect_binding->count;
//     //             layout_binding.stageFlags = stage_data.first == ShaderStage::COMPUTE ? VK_SHADER_STAGE_COMPUTE_BIT : VK_SHADER_STAGE_ALL;
//     //
//     //             bool skip_binding = false;
//     //
//     //             for (auto& set_binding : bindings[reflect_set->set]) {
//     //                 if (set_binding.binding == layout_binding.binding) {
//     //                     skip_binding = true;
//     //                     break;
//     //                 }
//     //             }
//     //
//     //             if (!skip_binding)
//     //                 bindings[reflect_set->set].push_back(layout_binding);
//     //         }
//     //     }
//     //
//     //     uint32 push_constant_range_count = 0;
//     //     spvReflectEnumeratePushConstantBlocks(&reflect_module, &push_constant_range_count, nullptr);
//     //
//     //     std::vector<SpvReflectBlockVariable*> reflect_push_constant_ranges(push_constant_range_count);
//     //     spvReflectEnumeratePushConstantBlocks(&reflect_module, &push_constant_range_count, reflect_push_constant_ranges.data());
//     //
//     //     if (push_constant_range_count) {
//     //
//     //         for (int i = 0; i < reflect_push_constant_ranges[0]->member_count; i++) {
//     //             auto& member = reflect_push_constant_ranges[0]->members[i];
//     //         }
//     //     }
//     //
//     //     for (auto& reflect_range : reflect_push_constant_ranges) {
//     //         push_constant_range.size += reflect_range->size;
//     //         push_constant_range.offset = 0;
//     //         push_constant_range.stageFlags = VK_SHADER_STAGE_ALL;
//     //     }
//     //
//     //     spvReflectDestroyShaderModule(&reflect_module);
//     // }
//     //
//     // if (push_constant_range.size)
//     //     m_Ranges.push_back(push_constant_range);
//     //
//     // if (TB_BUILD_CONFIG == TB_DEBUG_CONFIG) {
//     //     for (auto& set : bindings) {
//     //         TB_CORE_TRACE("\tSet #{0}: ", set.first);
//     //         for (auto& binding : set.second) {
//     //             TB_CORE_TRACE("\t\t Binding #{0}: {1}[{2}]", binding.binding, DescriptorToString(binding.descriptorType), binding.descriptorCount);
//     //         }
//     //     }
//     //     printf("\n");
//     // }
//     //
//     // if (bindings.size()) {
//     //     uint32 highest_set_index = bindings.rbegin()->first;
//     //     for (int i = 0; i <= highest_set_index; i++) {
//     //         if (bindings.find(i) == bindings.end())
//     //             m_SetLayouts.push_back(VK_NULL_HANDLE);
//     //         else {
//     //             std::vector<VkDescriptorBindingFlags> binding_flags;
//     //             for (auto& binding : bindings[i]) {
//     //                 if (binding.descriptorCount != 1)
//     //                     binding_flags.push_back(VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT);
//     //                 else
//     //                     binding_flags.push_back(0);
//     //             }
//     //
//     //             VkDescriptorSetLayoutBindingFlagsCreateInfo layout_bindings_flags = {};
//     //             layout_bindings_flags.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
//     //             layout_bindings_flags.bindingCount = binding_flags.size();
//     //             layout_bindings_flags.pBindingFlags = binding_flags.data();
//     //
//     //             VkDescriptorSetLayoutCreateInfo layout_create_info = {};
//     //             layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
//     //             layout_create_info.pNext = &layout_bindings_flags;
//     //             layout_create_info.bindingCount = bindings[i].size();
//     //             layout_create_info.pBindings = bindings[i].data();
//     //
//     //             VkDescriptorSetLayout vk_descriptor_set_layout = VK_NULL_HANDLE;
//     //             vkCreateDescriptorSetLayout(device->Raw(), &layout_create_info, nullptr, &vk_descriptor_set_layout);
//     //             m_SetLayouts.push_back(vk_descriptor_set_layout);
//     //         }
//     //     }
//     // }
// }
//
// VulkanShader::~VulkanShader()
// {
// }
//
// void VulkanShader::Destroy()
// {
//     auto device = VulkanGraphicsContext::Get()->GetDevice();
//
//     for (auto& stage : m_StageCreateInfos)
//         if (stage.module != VK_NULL_HANDLE)
//             vkDestroyShaderModule(device->Raw(), stage.module, nullptr);
//
//     vkDestroyPipeline(device->Raw(), m_Pipeline, nullptr);
//     vkDestroyPipelineLayout(device->Raw(), m_PipelineLayout, nullptr);
//     // for (auto& layout : m_SetLayouts)
//     //     vkDestroyDescriptorSetLayout(device->Raw(), layout, nullptr);
//     //
//     // m_SetLayouts.clear();
//     // for (auto& stage : m_StageCreateInfos)
//     //     stage.module = VK_NULL_HANDLE;
// }
//
// // void VulkanShader::RestoreShaderModule(std::filesystem::path path)
// // {
// // }
// // }
// }
