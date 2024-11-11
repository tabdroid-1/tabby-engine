#pragma once

#include <Tabby/Foundation/Types.h>
#include <Tabby/Renderer/Image.h>
#include <Tabby/Renderer/Mesh.h>

namespace Tabby {

struct ViewSpecification {
public:
    uint8_t view_id;
    std::vector<Shared<Image>> render_targets;
    Shared<Image> depth_buffer;
    UIntVector2 render_area;
    IntVector2 offset;
    RGBA32 clear_value;

    bool operator==(const ViewSpecification& a) const
    {
        return (this->view_id == a.view_id
                   && this->render_targets == a.render_targets
                   && this->depth_buffer == a.depth_buffer
                   && this->render_area == a.render_area
                   && this->offset == a.offset
                   && this->clear_value == a.clear_value)
            ? true
            : false;
    }
};

class Renderer {
public:
    Renderer() { }

    static void Init();
    static void Shutdown();

    // Sets attachment, clear color etc. for the specified view.
    // Could be called each frame, it will recreate if update is required.
    static void SetViewTarget(const ViewSpecification& spec);
    static void SetViewMatrix(uint8_t view_id, const Matrix4& view, const Matrix4& projection);
    static void UnsetViewTarget(uint8_t view_id);

    static void DrawMesh(uint8_t view_id, Shared<Mesh> mesh);

    static void Frame();

private:
};

}
