#pragma once

#include <Tabby/Foundation/Types.h>

namespace Tabby {

class Image;

class Renderer {
public:
    Renderer() { }

    static void BeginRender(std::vector<Shared<Image>> render_targets, Shared<Image> depth_buffer, UIntVector2 render_area, IntVector2 offset, Vector4 clear_value);
    static void EndRender();

    static void Frame();

private:
};

}
