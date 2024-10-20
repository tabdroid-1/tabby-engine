#include <Drivers/Vulkan/UI/VulkanImGuiRenderer.h>
#include <Drivers/Vulkan/VulkanDeviceCmdBuffer.h>
#include <Drivers/Vulkan/VulkanGraphicsContext.h>
#include <Drivers/Vulkan/VulkanSwapchain.h>
#include <Drivers/Vulkan/VulkanDevice.h>
#include <Drivers/Vulkan/VulkanImage.h>

#include "backends/imgui_impl_vulkan.h"
#include "backends/imgui_impl_sdl2.h"
#include <Tabby/Core/Application.h>
#include <ImGuizmo.h>

namespace Tabby {

static VkDescriptorPool pool = VK_NULL_HANDLE;

static std::unordered_map<UUID, VkDescriptorSet> imgui_image_descriptor_sets;

static VkRenderPass createDummyRenderPass()
{
    VkAttachmentDescription color_attachment;
    color_attachment.format = VK_FORMAT_R8G8B8A8_UNORM;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref = {};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo render_pass_info = {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = 1;
    render_pass_info.pAttachments = &color_attachment;
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    render_pass_info.dependencyCount = 1;
    render_pass_info.pDependencies = &dependency;

    VkRenderPass dummy_render_pass = nullptr;

    VK_CHECK_RESULT(vkCreateRenderPass(VulkanGraphicsContext::Get()->GetDevice()->Raw(), &render_pass_info, nullptr, &dummy_render_pass))

    return dummy_render_pass;
}

VulkanImGuiRenderer::VulkanImGuiRenderer()
{
}

VulkanImGuiRenderer::~VulkanImGuiRenderer()
{
}

void VulkanImGuiRenderer::Launch(void* window_handle)
{
    TB_CORE_INFO("Launching ImGui renderer...");
    auto device = VulkanGraphicsContext::Get()->GetDevice();
    auto context = VulkanGraphicsContext::Get();
    auto swapchain = VulkanGraphicsContext::Get()->GetSwapchain();
    auto sdl_window = (SDL_Window*)window_handle;

    VkDescriptorPoolSize pool_sizes[] = {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
    };

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000;
    pool_info.poolSizeCount = std::size(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;

    VK_CHECK_RESULT(vkCreateDescriptorPool(device->Raw(), &pool_info, nullptr, &pool));

    // Created ImGui renderer descriptor pool

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    // Created ImGui context

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
    style.Alpha = 1.0f;
    style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.08f, 0.50f, 0.72f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
    style.Colors[ImGuiCol_Separator] = style.Colors[ImGuiCol_Border];
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.41f, 0.42f, 0.44f, 1.00f);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.29f, 0.30f, 0.31f, 0.67f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    style.Colors[ImGuiCol_Tab] = ImVec4(0.08f, 0.08f, 0.09f, 0.83f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.33f, 0.34f, 0.36f, 0.83f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.23f, 0.23f, 0.24f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
    style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
    style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    style.GrabRounding = style.FrameRounding = 2.3f;

    ImGui_ImplSDL2_InitForVulkan(sdl_window);

    VkRenderPass dummy_render_pass = createDummyRenderPass();

    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = context->GetVulkanInstance();
    init_info.PhysicalDevice = device->GetPhysicalDevice()->Raw();
    init_info.Device = device->Raw();
    init_info.QueueFamily = device->GetPhysicalDevice()->GetQueueFamilyIndices().graphics.value();
    init_info.Queue = device->GetGraphicsQueue();
    init_info.DescriptorPool = pool;
    init_info.MinImageCount = Renderer::GetConfig().frames_in_flight;
    init_info.ImageCount = Renderer::GetConfig().frames_in_flight;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.UseDynamicRendering = false;
    init_info.RenderPass = dummy_render_pass;

    ImGui_ImplVulkan_Init(&init_info);

    vkDestroyRenderPass(device->Raw(), dummy_render_pass, nullptr);

    // m_MainFont = io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Regular.ttf", 16);

    ImGui_ImplVulkan_CreateFontsTexture();

    m_FrameImages.resize(VulkanGraphicsContext::GetFramesInFlight());

    for (int i = 0; i < m_FrameImages.size(); i++) {

        ImageSpecification attachment_spec = ImageSpecification::Default();
        attachment_spec.usage = ImageUsage::RENDER_TARGET;
        attachment_spec.extent = { Application::GetWindow().GetWidth(), Application::GetWindow().GetHeight(), 1 };
        attachment_spec.format = ImageFormat::RGBA32_UNORM;

        m_FrameImages[i] = ShareAs<VulkanImage>(Image::Create(attachment_spec));
    }
}

void VulkanImGuiRenderer::Destroy()
{
    auto device = VulkanGraphicsContext::Get()->GetDevice();

    vkDeviceWaitIdle(device->Raw());

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    vkDestroyDescriptorPool(device->Raw(), pool, nullptr);
}

void VulkanImGuiRenderer::BeginFrame()
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
}

void VulkanImGuiRenderer::EndFrame()
{
    OnRender();
    Renderer::Submit([]() {
        ImGuiIO& io = ImGui::GetIO();
        (void)io;
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    });
}

void VulkanImGuiRenderer::OnRender()
{
    Renderer::RenderImGui();
}

namespace UI {
    void UnregisterImage(Shared<Image> image)
    {
        imgui_image_descriptor_sets.erase(image->Handle);
    }

    void RenderImage(Shared<Image> image, Shared<ImageSampler> sampler, ImVec2 size, uint32_t image_layer, bool flip)
    {
        Shared<VulkanImage> vk_image = ShareAs<VulkanImage>(image);
        Shared<VulkanImageSampler> vk_sampler = ShareAs<VulkanImageSampler>(sampler);
        if (imgui_image_descriptor_sets.find(image->Handle) == imgui_image_descriptor_sets.end()) {

            VkDescriptorSet imgui_image_id = ImGui_ImplVulkan_AddTexture(
                vk_sampler->Raw(),
                vk_image->RawView(),
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            imgui_image_descriptor_sets.emplace(image->Handle, imgui_image_id);
        }
        ImGui::Image((ImTextureID)imgui_image_descriptor_sets[image->Handle], size, { 0, (float)!flip }, { 1, (float)flip });
    };

    bool RenderImageButton(Shared<Image> image, Shared<ImageSampler> sampler, ImVec2 size, uint32_t image_layer, bool flip)
    {
        Shared<VulkanImage> vk_image = ShareAs<VulkanImage>(image);
        Shared<VulkanImageSampler> vk_sampler = ShareAs<VulkanImageSampler>(sampler);
        if (imgui_image_descriptor_sets.find(image->Handle) == imgui_image_descriptor_sets.end()) {

            VkDescriptorSet imgui_image_id = ImGui_ImplVulkan_AddTexture(
                vk_sampler->Raw(),
                vk_image->RawView(),
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            imgui_image_descriptor_sets.emplace(image->Handle, imgui_image_id);
        }
        return ImGui::ImageButton("", (ImTextureID)imgui_image_descriptor_sets[image->Handle], size, { 0, (float)!flip }, { 1, (float)flip });
    }

}

}
