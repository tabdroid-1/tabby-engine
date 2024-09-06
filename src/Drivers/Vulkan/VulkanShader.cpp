#include <Drivers/Vulkan/VulkanGraphicsContext.h>
#include <Drivers/Vulkan/VulkanSwapchain.h>
#include <Drivers/Vulkan/VulkanDevice.h>
#include <Drivers/Vulkan/VulkanShader.h>

namespace Tabby {

VulkanShader::VulkanShader(std::vector<std::filesystem::path> files)
{
    auto device = VulkanGraphicsContext::Get()->GetDevice();

    for (const auto& filePath : files) {

        if (filePath.extension() != ".spv")
            continue;

        std::ifstream file(filePath.string(), std::ios::ate | std::ios::binary);
        TB_CORE_ASSERT_TAGGED(file.is_open(), "Failed to open file");

        size_t fileSize = (size_t)file.tellg();
        std::vector<char> code(fileSize);

        file.seekg(0);
        file.read(code.data(), fileSize);

        file.close();

        VkShaderModule shader_module;
        VkShaderModuleCreateInfo shader_module_create_info {};
        shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shader_module_create_info.codeSize = code.size();
        shader_module_create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

        VK_CHECK_RESULT(vkCreateShaderModule(device->Raw(), &shader_module_create_info, nullptr, &shader_module));

        VkPipelineShaderStageCreateInfo shaderStageInfo {};
        shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        if (filePath.filename().string().find("vert") != std::string::npos)
            shaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        else
            shaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;

        shaderStageInfo.module = shader_module;
        shaderStageInfo.pName = "main";

        m_StageCreateInfos.push_back(shaderStageInfo);
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

    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineLayoutCreateInfo pipelineLayoutInfo {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pushConstantRangeCount = 0;

    VK_CHECK_RESULT(vkCreatePipelineLayout(device->Raw(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout));

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
    pipelineInfo.renderPass = VulkanGraphicsContext::Get()->GetSwapchain()->RawRenderPass();
    pipelineInfo.subpass = 0;

    VK_CHECK_RESULT(vkCreateGraphicsPipelines(device->Raw(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline));
}

VulkanShader::~VulkanShader()
{
}

void VulkanShader::Destroy()
{
    auto device = VulkanGraphicsContext::Get()->GetDevice();

    for (auto shaderModule : m_StageCreateInfos) {
        vkDestroyShaderModule(device->Raw(), shaderModule.module, nullptr);
    }

    vkDestroyPipeline(device->Raw(), m_GraphicsPipeline, nullptr);
    vkDestroyPipelineLayout(device->Raw(), m_PipelineLayout, nullptr);
}

}
