#pragma once

#include <imgui.h>

namespace Tabby {

class Image;
class ImageSampler;

class ImGuiRenderer {
public:
    static ImGuiRenderer* Create();

    virtual void Launch(void* window_handle) = 0;
    virtual void Destroy() = 0;

    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;
    virtual void OnRender() = 0;
};

namespace UI {
    void UnregisterImage(Shared<Image> image);
    void RenderImage(Shared<Image> image, Shared<ImageSampler> sampler, ImVec2 size, uint32_t image_layer = 0, bool flip = false);
    bool RenderImageButton(Shared<Image> image, Shared<ImageSampler> sampler, ImVec2 size, uint32_t image_layer = 0, bool flip = false);
}
}
