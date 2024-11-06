#pragma once

#include <imgui.h>

namespace Tabby {

class ImGuiRenderer {
public:
    void Launch(void* window_handle);
    void Destroy();

    void BeginFrame();
    void EndFrame();
    void OnRender();
};

}
