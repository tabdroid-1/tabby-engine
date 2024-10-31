#pragma once

#include <Tabby/Renderer/Image.h>

namespace Tabby {

struct RenderPassSpecification {
    std::vector<Shared<Image>> attachments;
    Vector4 clear_color;
    UIntVector3 extent;

    bool operator==(const RenderPassSpecification& a) const
    {
        return (this->attachments == a.attachments && this->extent == a.extent && this->clear_color == a.clear_color) ? true : false;
    }
};

class RenderPass {
public:
    static Shared<RenderPass> Create(const RenderPassSpecification& spec);
    virtual ~RenderPass() { }

    virtual void Refresh(const RenderPassSpecification& spec) = 0;
    virtual void Destroy() = 0;

    virtual const RenderPassSpecification& GetSpecification() const = 0;

    // virtual void Begin() = 0;
    // virtual void End() = 0;
};

}
