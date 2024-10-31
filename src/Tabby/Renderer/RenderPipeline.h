#pragma once

#include <Tabby/Renderer/RenderPass.h>
#include <Tabby/Renderer/Camera.h>
#include <Tabby/Renderer/Image.h>

namespace Tabby {

class RenderPipeline {
public:
    virtual ~RenderPipeline() { }

    virtual void Begin(const Camera& camera) = 0;
    virtual void End() = 0;

    virtual Shared<Image> GetFinalImage() = 0;
    virtual Shared<RenderPass> GetRenderPass() = 0;
};

}
