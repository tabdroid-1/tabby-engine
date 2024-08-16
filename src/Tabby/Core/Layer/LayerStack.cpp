#include "tbpch.h"
#include <Tabby/Core/Layer/LayerStack.h>

namespace Tabby {

LayerStack::~LayerStack()
{
    TB_PROFILE_SCOPE_NAME("Tabby::LayerStack::Destructor");

    for (Layer* layer : m_Layers) {
        layer->OnDetach();
        delete layer;
    }
}

void LayerStack::PushLayer(Layer* layer)
{
    TB_PROFILE_SCOPE_NAME("Tabby::LayerStack::PushLayer");

    m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
    m_LayerInsertIndex++;
}

void LayerStack::PushOverlay(Layer* overlay)
{
    TB_PROFILE_SCOPE_NAME("Tabby::LayerStack::PushOverlay");

    m_Layers.emplace_back(overlay);
}

void LayerStack::PopLayer(Layer* layer)
{
    TB_PROFILE_SCOPE_NAME("Tabby::LayerStack::PopLayer");

    auto it = std::find(m_Layers.begin(), m_Layers.begin() + m_LayerInsertIndex, layer);
    if (it != m_Layers.begin() + m_LayerInsertIndex) {
        layer->OnDetach();
        m_Layers.erase(it);
        m_LayerInsertIndex--;
    }
}

void LayerStack::PopOverlay(Layer* overlay)
{
    TB_PROFILE_SCOPE_NAME("Tabby::LayerStack::PushOverlay");

    auto it = std::find(m_Layers.begin() + m_LayerInsertIndex, m_Layers.end(), overlay);
    if (it != m_Layers.end()) {
        overlay->OnDetach();
        m_Layers.erase(it);
    }
}

}
