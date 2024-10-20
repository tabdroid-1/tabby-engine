#pragma once

#include <Tabby/Foundation/Types.h>
#include <Tabby/Renderer/Material.h>

namespace Tabby {
struct RendererConfig;

class Mesh;
class Image;
class Shader;
class ShaderBuffer;

using RenderFunction = std::function<void()>;

class RendererAPI {
public:
    RendererAPI() {};
    virtual ~RendererAPI() {};

    virtual RendererConfig GetConfig() const = 0;
    virtual uint32_t GetCurrentFrameIndex() const = 0;
    // virtual uint32 GetDeviceMinimalUniformBufferAlignment() const = 0;
    // virtual uint32 GetDeviceMinimalStorageBufferAlignment() const = 0;
    // virtual uint32 GetDeviceOptimalTaskWorkGroupSize() const = 0;
    // virtual uint32 GetDeviceOptimalMeshWorkGroupSize() const = 0;
    // virtual uint32 GetDeviceOptimalComputeWorkGroupSize() const = 0;
    // virtual Shared<Swapchain> GetSwapchain() = 0;
    // virtual Shared<DeviceCmdBuffer> GetCmdBuffer() = 0;
    //

    void Submit(RenderFunction func) { m_FunctionList.push_back(func); };

    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;
    virtual void BeginRender(std::vector<Shared<Image>> attachments, UIntVector3 render_area, IntVector2 offset, Vector4 clear_value) = 0;
    virtual void EndRender(Shared<Image> target) = 0;
    virtual void WaitDevice() = 0;
    // virtual void BindSet(Shared<DescriptorSet> set, Shared<Pipeline> pipeline, uint8 index) = 0;
    // virtual void CopyToSwapchain(Shared<Image> image) = 0;
    // virtual void InsertBarrier(const PipelineBarrierInfo& barrier) = 0;

    virtual void BeginCommandRecord() = 0;
    virtual void EndCommandRecord() = 0;
    virtual void ExecuteCurrentCommands() = 0;

    virtual void ClearImage(Shared<Image> image, const Vector4& value) = 0;
    virtual void RenderTasks(Shared<Mesh>, std::vector<MaterialData> elements, Buffer data = Buffer()) = 0;
    virtual void RenderTasks(Shared<Shader> shader, uint32_t vertex_count, Buffer data = Buffer()) = 0;
    // virtual void RenderQuad(Shared<Pipeline> pipeline, MiscData data) = 0;
    // virtual void RenderQuad(Shared<Pipeline> pipeline, uint32 amount, MiscData data) = 0;
    // virtual void DispatchCompute(Shared<Pipeline> pipeline, const glm::uvec3& dimensions, MiscData data) = 0;
    // virtual void RenderUnindexed(Shared<Pipeline> pipeline, Shared<DeviceBuffer> vertex_buffer, MiscData data) = 0;

    virtual void Render() = 0;
    virtual void RenderImGui() = 0;

protected:
    std::vector<RenderFunction> m_FunctionList;
};

};
