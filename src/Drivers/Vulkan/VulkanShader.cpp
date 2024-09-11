#include "VulkanRenderPass.h"
#include "VulkanShader.h"
#include "VulkanDevice.h"

#include "VulkanGraphicsContext.h"

namespace Tabby {

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

VulkanShader::VulkanShader(std::map<ShaderStage, std::vector<uint8_t>> binaries)
{
    auto device = VulkanGraphicsContext::Get()->GetDevice();

    for (const auto& binary : binaries) {
        VkShaderModuleCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = binary.second.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(binary.second.data());

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(device->Raw(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shader module!");
        }

        VkPipelineShaderStageCreateInfo vertShaderStageInfo {};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = convert(binary.first);
        vertShaderStageInfo.module = shaderModule;
        vertShaderStageInfo.pName = "main";

        m_StageCreateInfos.push_back(vertShaderStageInfo);
    }

    VkPipelineVertexInputStateCreateInfo vertexInputInfo {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewportState {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dynamicState {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineLayoutCreateInfo pipelineLayoutInfo {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pushConstantRangeCount = 0;

    if (vkCreatePipelineLayout(device->Raw(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout)
        != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    VkGraphicsPipelineCreateInfo pipelineInfo {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = m_StageCreateInfos.data();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = m_PipelineLayout;
    pipelineInfo.renderPass = VulkanGraphicsContext::Get()->GetRenderPass()->Raw();
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(device->Raw(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    // // set id and its bindings
    // std::map<uint32, std::vector<VkDescriptorSetLayoutBinding>> bindings;
    // VkPushConstantRange push_constant_range = {};
    //
    // OMNIFORCE_CORE_TRACE("=======================");
    // OMNIFORCE_CORE_TRACE("Reflecting shader - {0}", path.filename().string());
    // OMNIFORCE_CORE_TRACE("=======================");

    // for (auto& stage_data : binaries) {
    //     VkShaderModule shader_module;
    //
    //     VkShaderModuleCreateInfo shader_module_create_info = {};
    //     shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    //     shader_module_create_info.pCode = stage_data.second.data();
    //     shader_module_create_info.codeSize = stage_data.second.size() * 4;
    //
    //     VK_CHECK_RESULT(vkCreateShaderModule(device->Raw(), &shader_module_create_info, nullptr, &shader_module));
    //
    //     VkPipelineShaderStageCreateInfo shader_stage_create_info = {};
    //     shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    //     shader_stage_create_info.stage = convert(stage_data.first);
    //     shader_stage_create_info.pName = "main";
    //     shader_stage_create_info.module = shader_module;
    //
    //     m_StageCreateInfos.push_back(shader_stage_create_info);
    //
    //     SpvReflectShaderModule reflect_module;
    //     if (spvReflectCreateShaderModule(stage_data.second.size() * 4, stage_data.second.data(), &reflect_module) != SPV_REFLECT_RESULT_SUCCESS) {
    //         OMNIFORCE_CORE_ERROR("Failed to reflect shader {}", path.filename().string());
    //         m_Dirty = true;
    //         return;
    //     }
    //
    //     uint32 set_count = 0;
    //     spvReflectEnumerateDescriptorSets(&reflect_module, &set_count, nullptr);
    //
    //     std::vector<SpvReflectDescriptorSet*> reflect_descriptor_sets(set_count);
    //     spvReflectEnumerateDescriptorSets(&reflect_module, &set_count, reflect_descriptor_sets.data());
    //
    //     for (auto& reflect_set : reflect_descriptor_sets) {
    //         if (!bindings.contains(reflect_set->set)) {
    //             bindings.emplace(reflect_set->set, std::vector<VkDescriptorSetLayoutBinding>());
    //         }
    //
    //         for (int i = 0; i < reflect_set->binding_count; i++) {
    //             SpvReflectDescriptorBinding* reflect_binding = reflect_set->bindings[i];
    //
    //             VkDescriptorSetLayoutBinding layout_binding = {};
    //             layout_binding.binding = reflect_binding->binding;
    //             layout_binding.descriptorType = convert(reflect_binding->descriptor_type);
    //             layout_binding.descriptorCount = reflect_binding->count;
    //             layout_binding.stageFlags = stage_data.first == ShaderStage::COMPUTE ? VK_SHADER_STAGE_COMPUTE_BIT : VK_SHADER_STAGE_ALL;
    //
    //             bool skip_binding = false;
    //
    //             for (auto& set_binding : bindings[reflect_set->set]) {
    //                 if (set_binding.binding == layout_binding.binding) {
    //                     skip_binding = true;
    //                     break;
    //                 }
    //             }
    //
    //             if (!skip_binding)
    //                 bindings[reflect_set->set].push_back(layout_binding);
    //         }
    //     }
    //
    //     uint32 push_constant_range_count = 0;
    //     spvReflectEnumeratePushConstantBlocks(&reflect_module, &push_constant_range_count, nullptr);
    //
    //     std::vector<SpvReflectBlockVariable*> reflect_push_constant_ranges(push_constant_range_count);
    //     spvReflectEnumeratePushConstantBlocks(&reflect_module, &push_constant_range_count, reflect_push_constant_ranges.data());
    //
    //     if (push_constant_range_count) {
    //
    //         for (int i = 0; i < reflect_push_constant_ranges[0]->member_count; i++) {
    //             auto& member = reflect_push_constant_ranges[0]->members[i];
    //         }
    //     }
    //
    //     for (auto& reflect_range : reflect_push_constant_ranges) {
    //         push_constant_range.size += reflect_range->size;
    //         push_constant_range.offset = 0;
    //         push_constant_range.stageFlags = VK_SHADER_STAGE_ALL;
    //     }
    //
    //     spvReflectDestroyShaderModule(&reflect_module);
    // }
    //
    // if (push_constant_range.size)
    //     m_Ranges.push_back(push_constant_range);
    //
    // if (OMNIFORCE_BUILD_CONFIG == OMNIFORCE_DEBUG_CONFIG) {
    //     for (auto& set : bindings) {
    //         OMNIFORCE_CORE_TRACE("\tSet #{0}: ", set.first);
    //         for (auto& binding : set.second) {
    //             OMNIFORCE_CORE_TRACE("\t\t Binding #{0}: {1}[{2}]", binding.binding, DescriptorToString(binding.descriptorType), binding.descriptorCount);
    //         }
    //     }
    //     printf("\n");
    // }
    //
    // if (bindings.size()) {
    //     uint32 highest_set_index = bindings.rbegin()->first;
    //     for (int i = 0; i <= highest_set_index; i++) {
    //         if (bindings.find(i) == bindings.end())
    //             m_SetLayouts.push_back(VK_NULL_HANDLE);
    //         else {
    //             std::vector<VkDescriptorBindingFlags> binding_flags;
    //             for (auto& binding : bindings[i]) {
    //                 if (binding.descriptorCount != 1)
    //                     binding_flags.push_back(VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT);
    //                 else
    //                     binding_flags.push_back(0);
    //             }
    //
    //             VkDescriptorSetLayoutBindingFlagsCreateInfo layout_bindings_flags = {};
    //             layout_bindings_flags.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
    //             layout_bindings_flags.bindingCount = binding_flags.size();
    //             layout_bindings_flags.pBindingFlags = binding_flags.data();
    //
    //             VkDescriptorSetLayoutCreateInfo layout_create_info = {};
    //             layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    //             layout_create_info.pNext = &layout_bindings_flags;
    //             layout_create_info.bindingCount = bindings[i].size();
    //             layout_create_info.pBindings = bindings[i].data();
    //
    //             VkDescriptorSetLayout vk_descriptor_set_layout = VK_NULL_HANDLE;
    //             vkCreateDescriptorSetLayout(device->Raw(), &layout_create_info, nullptr, &vk_descriptor_set_layout);
    //             m_SetLayouts.push_back(vk_descriptor_set_layout);
    //         }
    //     }
    // }
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

    vkDestroyPipeline(device->Raw(), m_Pipeline, nullptr);
    vkDestroyPipelineLayout(device->Raw(), m_PipelineLayout, nullptr);
    // for (auto& layout : m_SetLayouts)
    //     vkDestroyDescriptorSetLayout(device->Raw(), layout, nullptr);
    //
    // m_SetLayouts.clear();
    // for (auto& stage : m_StageCreateInfos)
    //     stage.module = VK_NULL_HANDLE;
}

// void VulkanShader::RestoreShaderModule(std::filesystem::path path)
// {
// }
// }
}
