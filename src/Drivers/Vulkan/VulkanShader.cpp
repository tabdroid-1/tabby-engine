#include <Drivers/Vulkan/VulkanDescriptorSet.h>
#include <Drivers/Vulkan/VulkanSwapchain.h>
#include <Drivers/Vulkan/VulkanPipeline.h>
#include <Drivers/Vulkan/VulkanShader.h>
#include <Drivers/Vulkan/VulkanDevice.h>
#include <Drivers/Vulkan/VulkanImage.h>

#include <Tabby/Renderer/Pipeline.h>
#include "VulkanGraphicsContext.h"

#include <spirv_reflect.c>

namespace Tabby {

#pragma region converts
VkShaderStageFlagBits shaderstage_to_vkshaderstage(const ShaderStage& stage)
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

const VkDescriptorType convert(SpvReflectDescriptorType type)
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

const VulkanDescriptorBindingType convert(VkDescriptorType type)
{
    switch (type) {

    case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
        return VulkanDescriptorBindingType::SAMPLED_IMAGE;
    case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
        return VulkanDescriptorBindingType::STORAGE_IMAGE;
    case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
        return VulkanDescriptorBindingType::UNIFORM_BUFFER;
    case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
        return VulkanDescriptorBindingType::STORAGE_BUFFER;
    default:
        TB_CORE_ASSERT(false);
        return VulkanDescriptorBindingType::UNIFORM_BUFFER;
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

const ShaderDataType convert(const SpvReflectFormat& type)
{

    switch (type) {
    case SPV_REFLECT_FORMAT_R32_SINT:
        return ShaderDataType::INT;
    case SPV_REFLECT_FORMAT_R32G32_SINT:
        return ShaderDataType::INT2;
    case SPV_REFLECT_FORMAT_R32G32B32_SINT:
        return ShaderDataType::INT3;
    case SPV_REFLECT_FORMAT_R32G32B32A32_SINT:
        return ShaderDataType::INT4;
    case SPV_REFLECT_FORMAT_R32_SFLOAT:
        return ShaderDataType::FLOAT;
    case SPV_REFLECT_FORMAT_R32G32_SFLOAT:
        return ShaderDataType::FLOAT2;
    case SPV_REFLECT_FORMAT_R32G32B32_SFLOAT:
        return ShaderDataType::FLOAT3;
    case SPV_REFLECT_FORMAT_R32G32B32A32_SFLOAT:
        return ShaderDataType::FLOAT4;
    // case SPV_REFLECT_FORMAT_R32G32B32_SINT:
    //     return ShaderDataType::IMAT3;
    // case SPV_REFLECT_FORMAT_R32G32B32A32_SINT:
    //     return ShaderDataType::IMAT4;
    // case SPV_REFLECT_FORMAT_R32G32B32_SFLOAT:
    //     return ShaderDataType::MAT3;
    // case SPV_REFLECT_FORMAT_R32G32B32A32_SFLOAT:
    //     return ShaderDataType::MAT4;
    default:
        TB_CORE_ASSERT(false);
        return ShaderDataType::INT;
    }
}

const VkPrimitiveTopology convert(const PipelineTopology& topology)
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

const VkPolygonMode convert(const PipelineFillMode& mode)
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

const VkCullModeFlagBits convert(const PipelineCullingMode& mode)
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

const VkFrontFace convert(const PipelineFrontFace face)
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

uint32_t spv_format_size(const SpvReflectFormat& format)
{
    switch (format) {
    case SPV_REFLECT_FORMAT_R16_UINT:
    case SPV_REFLECT_FORMAT_R16_SINT:
    case SPV_REFLECT_FORMAT_R16_SFLOAT:
        return 2;

    case SPV_REFLECT_FORMAT_R16G16_UINT:
    case SPV_REFLECT_FORMAT_R16G16_SINT:
    case SPV_REFLECT_FORMAT_R16G16_SFLOAT:
        return 4;

    case SPV_REFLECT_FORMAT_R16G16B16_UINT:
    case SPV_REFLECT_FORMAT_R16G16B16_SINT:
    case SPV_REFLECT_FORMAT_R16G16B16_SFLOAT:
        return 6;

    case SPV_REFLECT_FORMAT_R16G16B16A16_UINT:
    case SPV_REFLECT_FORMAT_R16G16B16A16_SINT:
    case SPV_REFLECT_FORMAT_R16G16B16A16_SFLOAT:
        return 8;

    case SPV_REFLECT_FORMAT_R32_UINT:
    case SPV_REFLECT_FORMAT_R32_SINT:
    case SPV_REFLECT_FORMAT_R32_SFLOAT:
        return 4;

    case SPV_REFLECT_FORMAT_R32G32_UINT:
    case SPV_REFLECT_FORMAT_R32G32_SINT:
    case SPV_REFLECT_FORMAT_R32G32_SFLOAT:
        return 8;

    case SPV_REFLECT_FORMAT_R32G32B32_UINT:
    case SPV_REFLECT_FORMAT_R32G32B32_SINT:
    case SPV_REFLECT_FORMAT_R32G32B32_SFLOAT:
        return 12;

    case SPV_REFLECT_FORMAT_R32G32B32A32_UINT:
    case SPV_REFLECT_FORMAT_R32G32B32A32_SINT:
    case SPV_REFLECT_FORMAT_R32G32B32A32_SFLOAT:
        return 16;

    case SPV_REFLECT_FORMAT_R64_UINT:
    case SPV_REFLECT_FORMAT_R64_SINT:
    case SPV_REFLECT_FORMAT_R64_SFLOAT:
        return 8;

    case SPV_REFLECT_FORMAT_R64G64_UINT:
    case SPV_REFLECT_FORMAT_R64G64_SINT:
    case SPV_REFLECT_FORMAT_R64G64_SFLOAT:
        return 16;

    case SPV_REFLECT_FORMAT_R64G64B64_UINT:
    case SPV_REFLECT_FORMAT_R64G64B64_SINT:
    case SPV_REFLECT_FORMAT_R64G64B64_SFLOAT:
        return 24;

    case SPV_REFLECT_FORMAT_R64G64B64A64_UINT:
    case SPV_REFLECT_FORMAT_R64G64B64A64_SINT:
    case SPV_REFLECT_FORMAT_R64G64B64A64_SFLOAT:
        return 32;

    case SPV_REFLECT_FORMAT_UNDEFINED:
    default:
        TB_CORE_ASSERT_TAGGED(false, "Shader format not supported");
        return 0;
    }
}
#pragma endregion

VulkanShader::VulkanShader(std::map<ShaderStage, std::vector<uint32_t>> binaries, const std::string& path)
{
    auto device = VulkanGraphicsContext::Get()->GetDevice();

    // set id and its bindings

    std::vector<ShaderBufferLayoutElement> input_elements;
    std::map<uint32_t, std::vector<VkDescriptorSetLayoutBinding>> bindings;
    VkPushConstantRange push_constant_range = {};

    // TB_CORE_TRACE("=======================");
    TB_CORE_TRACE("Reflecting shader - {0}", path);
    // TB_CORE_TRACE("=======================");

    for (auto& stage_data : binaries) {
        VkShaderModule shader_module;

        VkShaderModuleCreateInfo shader_module_create_info = {};
        shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shader_module_create_info.pCode = stage_data.second.data();
        shader_module_create_info.codeSize = stage_data.second.size() * 4;

        VK_CHECK_RESULT(vkCreateShaderModule(device->Raw(), &shader_module_create_info, nullptr, &shader_module));

        VkPipelineShaderStageCreateInfo shader_stage_create_info = {};
        shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stage_create_info.stage = shaderstage_to_vkshaderstage(stage_data.first);
        shader_stage_create_info.pName = "main";
        shader_stage_create_info.module = shader_module;

        m_StageCreateInfos.push_back(shader_stage_create_info);

        SpvReflectShaderModule reflect_module;
        if (spvReflectCreateShaderModule(stage_data.second.size() * 4, stage_data.second.data(), &reflect_module) != SPV_REFLECT_RESULT_SUCCESS) {
            TB_CORE_ERROR("Failed to reflect shader {}", path);
            m_Dirty = true;
            return;
        }

        uint32_t set_count = 0;
        spvReflectEnumerateDescriptorSets(&reflect_module, &set_count, nullptr);

        std::vector<SpvReflectDescriptorSet*> reflect_descriptor_sets(set_count);
        spvReflectEnumerateDescriptorSets(&reflect_module, &set_count, reflect_descriptor_sets.data());

        m_DescriptorInfos.resize(reflect_descriptor_sets.size());
        // Create info of uniforms.
        for (auto& reflect_set : reflect_descriptor_sets) {
            if (bindings.find(reflect_set->set) == bindings.end()) {
                bindings.emplace(reflect_set->set, std::vector<VkDescriptorSetLayoutBinding>());
            }

            VulkanDescriptorInfo& descriptor_info = m_DescriptorInfos[reflect_set->set];

            for (int i = 0; i < reflect_set->binding_count; i++) {
                SpvReflectDescriptorBinding* reflect_binding = reflect_set->bindings[i];

                VkDescriptorSetLayoutBinding layout_binding = {};
                layout_binding.binding = reflect_binding->binding;
                layout_binding.descriptorType = convert(reflect_binding->descriptor_type);
                layout_binding.descriptorCount = reflect_binding->count;
                layout_binding.stageFlags = stage_data.first == ShaderStage::COMPUTE ? VK_SHADER_STAGE_COMPUTE_BIT : VK_SHADER_STAGE_ALL;

                uint32_t size = 0;
                if (reflect_binding->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER || reflect_binding->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER || reflect_binding->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC || reflect_binding->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC)
                    size = reflect_binding->block.size;

                bool skip_binding = false;

                for (auto& set_binding : bindings[reflect_set->set]) {
                    if (set_binding.binding == layout_binding.binding) {
                        skip_binding = true;
                        break;
                    }
                }

                if (!skip_binding) {
                    bindings[reflect_set->set].push_back(layout_binding);

                    VulkanDescriptorBinding binding;
                    binding.type = convert(convert(reflect_binding->descriptor_type));
                    binding.binding = reflect_binding->binding;
                    if (reflect_binding->array.dims_count)
                        binding.array_count = reflect_binding->array.dims_count;
                    else
                        binding.array_count = 1;
                    binding.flags = 0;

                    descriptor_info.bindings.push_back(binding);
                    descriptor_info.binding_sizes.push_back(size);
                    descriptor_info.binding_names.push_back(reflect_binding->name);
                }
            }

            m_DescriptorInfos[reflect_set->set] = descriptor_info;
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

        // Create info of vertex input.
        if (stage_data.first == ShaderStage::VERTEX) {

            uint32_t input_variable_count = reflect_module.input_variable_count;
            SpvReflectInterfaceVariable** input_variables = reflect_module.input_variables;

            for (uint32_t i = 0; i < input_variable_count; ++i) {
                SpvReflectInterfaceVariable* input_var = input_variables[i];

                auto calculate_input_size = [](auto&& self, const SpvReflectInterfaceVariable* member) -> uint32_t {
                    uint32_t size = 0;

                    if (member->member_count > 0) {
                        for (uint32_t i = 0; i < member->member_count; ++i) {
                            SpvReflectInterfaceVariable* member_member = &member->members[i];
                            size += self(self, member_member); // :3
                        }
                    } else {
                        size = spv_format_size(member->format);
                    }

                    return size;
                };

                m_VertexBufferSize += calculate_input_size(calculate_input_size, input_var);

                input_elements.push_back(ShaderBufferLayoutElement(input_var->name, convert(input_var->format)));
            }
        }

        spvReflectDestroyShaderModule(&reflect_module);
    }

    if (push_constant_range.size)
        m_Ranges.push_back(push_constant_range);

    // #if TB_DEBUG
    //     for (auto& set : bindings) {
    //         TB_CORE_TRACE("\tSet #{0}: ", set.first);
    //         for (auto& binding : set.second) {
    //             TB_CORE_TRACE("\t\t Binding #{0}: {1}[{2}]", binding.binding, DescriptorToString(binding.descriptorType), binding.descriptorCount);
    //         }
    //     }
    // #endif

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

                // VkDescriptorSetLayoutBindingFlagsCreateInfo layout_bindings_flags = {};
                // layout_bindings_flags.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
                // layout_bindings_flags.bindingCount = binding_flags.size();
                // layout_bindings_flags.pBindingFlags = binding_flags.data();

                VkDescriptorSetLayoutCreateInfo layout_create_info = {};
                layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
                // layout_create_info.pNext = &layout_bindings_flags;
                layout_create_info.bindingCount = bindings[i].size();
                layout_create_info.pBindings = bindings[i].data();

                VkDescriptorSetLayout vk_descriptor_set_layout = VK_NULL_HANDLE;
                vkCreateDescriptorSetLayout(device->Raw(), &layout_create_info, nullptr, &vk_descriptor_set_layout);
                m_SetLayouts.push_back(vk_descriptor_set_layout);
            }
        }
    }

    m_BufferLayout = ShaderBufferLayout(input_elements);
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
}
