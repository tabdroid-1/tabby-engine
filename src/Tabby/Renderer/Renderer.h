#pragma once

#include <Tabby/Renderer/RendererAPI.h>
#include <Tabby/Foundation/Types.h>
#include <Tabby/Core/Window.h>

namespace Tabby {

struct RendererConfig {
    Window* main_window;
    uint32_t frames_in_flight;
    bool vsync;
};

class Image;
class ImageSampler;

class Renderer {
public:
    enum class API {
        Null = 0,
        Vulkan = 1,
        OpenGL46 = 2,
        OpenGL33 = 3,
        OpenGLES3 = 4
    };

    static void Init(const RendererConfig& config);
    static void Shutdown();

    static RendererConfig GetConfig();
    static uint32_t GetCurrentFrameIndex();
    static Shared<ImageSampler> GetNearestSampler();
    static Shared<ImageSampler> GetLinearSampler();

    static void LoadShaderPack();

    static void Submit(RenderFunction func);

    static void BeginFrame();
    static void EndFrame();
    static void BeginRender(const std::vector<Shared<Image>>& attachments, UIntVector3 render_area, IntVector2 offset, Vector4 clear_value);
    static void EndRender(Shared<Image> target);
    static void WaitDevice(); // to be used ONLY while shutting down the engine.

    static void ClearImage(Shared<Image> image, const Vector4& value);
    static void RenderTasks(Shared<Mesh> mesh, std::vector<MaterialData> elements, Buffer data = Buffer());
    static void RenderTasks(Shared<Shader> shader, uint32_t vertex_count, Buffer data = Buffer());

    static void Render();
    static void RenderImGui();

    static API GetAPI() { return s_API; }
    static const RendererAPI* GetRendererAPI() { return s_RendererAPI; }

private:
    inline static RendererAPI* s_RendererAPI;
    inline static API s_API;

    friend class Application;
};

}
