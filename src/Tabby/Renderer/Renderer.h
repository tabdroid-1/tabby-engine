#pragma once

#include <Tabby/Renderer/RenderPipeline.h>
#include <Tabby/Renderer/RendererAPI.h>
#include <Tabby/Renderer/Camera.h>
#include <Tabby/Foundation/Types.h>
#include <Tabby/Core/Window.h>

namespace Tabby {

struct RendererConfig {
    RenderPipeline* render_pipeline;
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

    // ----- For Render Pipeline use -----
    static void BeginFrame();
    static void EndFrame();
    static void BeginRender(Shared<RenderPass> render_pass, const std::vector<Shared<Image>>& attachments, UIntVector3 render_area, IntVector2 offset, Vector4 clear_value);
    static void EndRender(Shared<Image> target);
    // ----------------------------

    static void BeginRenderPipeline(const Camera& camera);
    static void EndRenderPipeline();
    static Shared<RenderPass> GetRenderPipelineRenderPass() { return s_RenderPipeline->GetRenderPass(); }
    static Shared<Image> GetRenderPipelineFinalImage() { return s_RenderPipeline->GetFinalImage(); }

    static void WaitDevice(); // to be used ONLY while shutting down the engine.

    static void CopyToSwapchain(Shared<Image> image);

    static void ClearImage(Shared<Image> image, const Vector4& value);
    static void RenderTasks(Shared<Mesh> mesh, std::vector<MaterialData> elements, Buffer data = Buffer());
    static void RenderTasks(Shared<Pipeline> pipeline, uint32_t vertex_count, Buffer data = Buffer());
    static void RenderQuad(Shared<Pipeline> pipeline, Buffer data = Buffer());
    static void RenderQuad(Shared<Pipeline> pipeline, uint32_t amount, Buffer data = Buffer());

    static void Render();
    static void RenderImGui();

    static API GetAPI() { return s_API; }
    static const RendererAPI* GetRendererAPI() { return s_RendererAPI; }

private:
    inline static RenderPipeline* s_RenderPipeline;
    inline static RendererAPI* s_RendererAPI;
    inline static API s_API;

    friend class Application;
};

}
