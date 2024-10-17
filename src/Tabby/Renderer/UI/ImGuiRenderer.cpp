#include <misc/cpp/imgui_stdlib.cpp>
#if defined(TB_PLATFORM_ANDROID)
#define IMGUI_IMPL_OPENGL_ES3
#endif
#define IMGUI_IMPL_OPENGL_LOADER_GLAD_2
#include <backends/imgui_impl_opengl3.cpp>
#include <backends/imgui_impl_vulkan.cpp>
#include <backends/imgui_impl_sdl2.cpp>

#include <Tabby/Renderer/UI/ImGuiRenderer.h>

#include <Drivers/Vulkan/UI/VulkanImGuiRenderer.h>

namespace Tabby {

ImGuiRenderer* ImGuiRenderer::Create()
{
    return new VulkanImGuiRenderer;
}

}
