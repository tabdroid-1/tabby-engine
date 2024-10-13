#pragma once

namespace Tabby {

class GraphicsContext {
public:
    static Shared<GraphicsContext> Create();

    virtual void Destroy() = 0;
};

}
