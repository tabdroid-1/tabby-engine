#pragma once

#include <Tabby/Renderer/UI/ImGuiRenderer.h>
#include <Drivers/Vulkan/VulkanImage.h>

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

    std::vector<Shared<VulkanImage>> GetFrameImages() { return m_FrameImages; }

private:
    ImFont* m_MainFont;
    std::vector<Shared<VulkanImage>> m_FrameImages;
};
}
