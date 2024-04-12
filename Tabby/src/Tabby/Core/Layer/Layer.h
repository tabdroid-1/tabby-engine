#pragma once

#include "Tabby/Core/Base.h"
#include "Tabby/Core/Events/Event.h"
#include "Tabby/Core/Time/Timestep.h"

namespace Tabby {

class Layer {
public:
    Layer(const std::string& name = "Layer");
    virtual ~Layer() = default;

    virtual void OnAttach() { }
    virtual void OnDetach() { }
    virtual void OnUpdate(Timestep ts) { }
    virtual void OnImGuiRender() { }
    virtual void OnEvent(Event& event) { }

    const std::string& GetName() const { return m_DebugName; }

protected:
    std::string m_DebugName;
};

}
