#pragma once

#include <Tabby.h>

namespace Tabby {

class MapElement {
public:
    enum Type {
        PrefabSpawner = 0,
        ColliderElement,
        AudioSouce,
    };

    UUID GetUUID() { m_UUID; }
    Type GetElementType() { m_MapElementType; }
    const std::string& GetElementName() { return m_ElementName; }

protected:
    MapElement(const std::string& elementName, Type elementType)
        : m_ElementName(elementName)
        , m_UUID(UUID())
        , m_MapElementType(elementType)
    {
    }

private:
    UUID m_UUID;
    Type m_MapElementType;
    std::string m_ElementName;
};

class PrefabSpawnerElement : public MapElement {
public:
    PrefabSpawnerElement(const std::string& elementName)
        : MapElement(elementName, MapElement::Type::PrefabSpawner)
    {
    }

    void SetPrefabName(const std::string& prefabName) { m_PrefabName; }
    void SetPostition(const Vector3& position) { m_Position; }
    void SetRotation(const Vector3& rotation) { m_Rotation; }
    const std::string& GetPrefabName() { return m_PrefabName; }
    const Vector3& GetPostition() { return m_Position; }
    const Vector3& GetRotation() { return m_Rotation; }

private:
    std::string m_PrefabName;
    Vector3 m_Position;
    Vector3 m_Rotation;
};

class ColliderElement : public MapElement {
public:
    ColliderElement(const std::string& elementName)
        : MapElement(elementName, MapElement::Type::ColliderElement)
    {
    }

    enum Type {
        Polygon = 0,
        Box,
        Circle,
        Capcule,
        Line,
    };

private:
    bool m_IsTrigger;
    Vector3 m_Position;
    Vector3 m_Rotation;
};

class AudioSouceElement : public MapElement {
public:
    AudioSouceElement(const std::string& elementName)
        : MapElement(elementName, MapElement::Type::AudioSouce)
    {
    }

    enum class Type {
        Spatial,
        NonSpatial
    };

private:
    Vector3 m_Position;
};

}
