#pragma once

#include <Tabby/World/Entity.h>

namespace Tabby {

class PropertiesPanel {
public:
    PropertiesPanel()
    {
        m_IsOpen = true;
    };

    void OnImGuiRender();

    void SetEntity(Entity entity, bool selected);

private:
    void RecalculateTransform();

private:
    Entity m_Entity;
    bool m_Selected = false;

    bool m_IsOpen;
};

}
