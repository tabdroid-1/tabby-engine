#pragma once

#include <Tabby/Foundation/Types.h>
#include <Tabby/Renderer/Image.h>

namespace Tabby {

class WorldRenderer {
public:
    static Shared<WorldRenderer> Create();

    WorldRenderer();
    ~WorldRenderer();

    void Destroy();

    // void BeginScene(Shared<Camera> camera);
    void BeginScene();
    void EndScene();

    Shared<Image> GetFinalImage();

private:
    std::vector<Shared<Image>> m_RendererOutputs;
    std::vector<Shared<Image>> m_DepthAttachments;
    Shared<Image> m_CurrectMainRenderTarget;
    Shared<Image> m_CurrentDepthAttachment;

    std::shared_mutex m_Mutex;
};

}
