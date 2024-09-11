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
    // static uint32 GetCurrentFrameIndex();
    // static uint32 GetDeviceMinimalUniformBufferOffsetAlignment();
    // static uint32 GetDeviceMinimalStorageBufferOffsetAlignment();
    // static uint32 GetDeviceOptimalTaskWorkGroupSize();
    // static uint32 GetDeviceOptimalMeshWorkGroupSize();
    // static uint32 GetDeviceOptimalComputeWorkGroupSize();
    // static Shared<ImageSampler> GetNearestSampler();
    // static Shared<ImageSampler> GetLinearSampler();
    // static Shared<Image> GetSwapchainImage();
    // static Shared<DeviceCmdBuffer> GetCmdBuffer();

    static void LoadShaderPack();
    static void Submit(RendererAPI::RenderFunction func);

    // static void BeginFrame();
    // static void EndFrame();
    // static void BeginRender(const std::vector<Shared<Image>> attachments, UIntVector3 render_area, IntVector2 offset, Vector4 clear_value);
    // static void EndRender(Shared<Image> target);
    static void WaitDevice(); // to be used ONLY while shutting down the engine.
    // static void BindSet(Shared<DescriptorSet> set, Shared<Pipeline> pipeline, uint8 index);
    // static void CopyToSwapchain(Shared<Image> image);
    // static void InsertBarrier(const PipelineBarrierInfo& barrier_info);
    //
    // static void ClearImage(Shared<Image> image, const fvec4& value);
    // static void RenderMeshTasks(Shared<Pipeline> pipeline, const glm::uvec3 dimensions, MiscData data);
    // static void RenderMeshTasksIndirect(Shared<Pipeline> pipeline, Shared<DeviceBuffer> params, MiscData data);
    // static void RenderQuads(Shared<Pipeline> pipeline, MiscData data);
    // static void RenderQuads(Shared<Pipeline> pipeline, uint32 amount, MiscData data);
    // static void DispatchCompute(Shared<Pipeline> pipeline, const glm::uvec3& dimensions, MiscData data);
    // static void RenderUnindexed(Shared<Pipeline> pipeline, Shared<DeviceBuffer> vertex_buffer, MiscData data);
    //
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
