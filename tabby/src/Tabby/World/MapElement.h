#pragma once

#include <Tabby.h>

namespace Tabby {

class MapElement {
public:
    MapElement(const std::string& elementName)
        : m_ElementName(elementName)
        , m_UUID(UUID())
    {
    }

    UUID GetUUID() { m_UUID; }
    const std::string& GetElementName() { return m_ElementName; }

private:
    UUID m_UUID;
    std::string m_ElementName;
};

class PrefabSpawner : public MapElement {
public:
    PrefabSpawner(const std::string& elementName)
        : MapElement(elementName)
    {
    }

private:
    glm::vec3 Position;
    glm::vec3 Rotation;
};

}
