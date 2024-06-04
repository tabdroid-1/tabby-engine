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

class

    class PrefabSpawnerElement : public MapElement {
public:
    PrefabSpawnerElement(const std::string& elementName)
        : MapElement(elementName)
    {
    }

private:
    glm::vec3 Position;
    glm::vec3 Rotation;
};

class TriggerElement : public MapElement {
public:
    TriggerElement(const std::string& elementName)
        : MapElement(elementName)
    {
    }

    enum Type {
        Box = 0,
        Circle,
        Capcule,
        Line,
    };

private:
    glm::vec3 Position;
    glm::vec3 Rotation;
};

class AudioSouceElement : public MapElement {
public:
    AudioSouceElement(const std::string& elementName)
        : MapElement(elementName)
    {
    }

    enum class Type {
        Spatial,
        NonSpatial
    };

private:
    glm::vec3 Position;
};

}
