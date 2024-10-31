#pragma once

#include <Drivers/Vulkan/VulkanRenderPass.h>
#include <Drivers/Vulkan/VulkanImage.h>

#include <Tabby/Renderer/UI/ImGuiRenderer.h>

#include <imgui.h>

namespace Tabby {

class VulkanImGuiRenderer : public ImGuiRenderer {
public:
    VulkanImGuiRenderer();
    ~VulkanImGuiRenderer();

    void Launch(void* window_handle) override;
    void Destroy() override;

    void BeginFrame() override;
    void EndFrame() override;
    void OnRender() override;

    Shared<RenderPass> GetRenderPass() override { return m_RenderPass; }
    std::vector<Shared<VulkanImage>> GetFrameImages() { return m_FrameImages; }

private:
    ImFont* m_MainFont;
    std::vector<Shared<VulkanImage>> m_FrameImages;
    Shared<RenderPass> m_RenderPass;
};
}
