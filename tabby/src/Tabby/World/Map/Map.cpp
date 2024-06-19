#include <Tabby/World/Map/Map.h>
#include <Tabby/World/Map/MapElement.h>

namespace Tabby {

Map::Map()
{
}

std::vector<uint8_t> Map::Serialize(Map map)
{
    std::vector<uint8_t> serializedData;

    // Serialize UUID
    size_t uuidSize = sizeof(UUID);
    serializedData.resize(uuidSize);
    std::memcpy(serializedData.data(), &map.m_UUID, uuidSize);

    // Serialize Map name
    size_t nameSize = map.m_Name.size();
    serializedData.insert(serializedData.end(), reinterpret_cast<const uint8_t*>(&nameSize), reinterpret_cast<const uint8_t*>(&nameSize) + sizeof(size_t));
    serializedData.insert(serializedData.end(), map.m_Name.begin(), map.m_Name.end());

    // Serialize number of Elements
    size_t numOfElements = map.m_ElementRegistry.size();
    serializedData.insert(serializedData.end(), reinterpret_cast<const uint8_t*>(&numOfElements), reinterpret_cast<const uint8_t*>(&numOfElements) + sizeof(size_t));
    for (const auto& element : map.m_ElementRegistry) {
        switch (element.second->GetElementType()) {
        case MapElement::Type::PrefabSpawner:
            const auto prefabElement = ShareAs<PrefabSpawnerElement>(element.second);

            // Serialize Element UUID
            size_t uuidSize = sizeof(UUID);
            std::memcpy(serializedData.data(), &element.first, uuidSize);

            // Serialize Element Type
            size_t elementTypeSize = sizeof(MapElement::Type);
            std::memcpy(serializedData.data(), reinterpret_cast<const uint8_t*>(element.second->GetElementType()), elementTypeSize);

            // Serialize Element name
            size_t elementNameSize = prefabElement->GetElementName().size();
            serializedData.insert(serializedData.end(), reinterpret_cast<const uint8_t*>(&elementNameSize), reinterpret_cast<const uint8_t*>(&elementNameSize) + sizeof(size_t));
            serializedData.insert(serializedData.end(), prefabElement->GetElementName().begin(), prefabElement->GetElementName().end());

            // Serialize Element Prefab Name
            size_t prefabNameSize = prefabElement->GetPrefabName().size();
            serializedData.insert(serializedData.end(), reinterpret_cast<const uint8_t*>(&prefabNameSize), reinterpret_cast<const uint8_t*>(&nameSize) + sizeof(size_t));
            serializedData.insert(serializedData.end(), prefabElement->GetPrefabName().begin(), prefabElement->GetPrefabName().end());

            // Serialize Element Position
            size_t vector3Size = sizeof(Vector3);
            serializedData.insert(serializedData.end(), reinterpret_cast<const uint8_t*>(&prefabNameSize), reinterpret_cast<const uint8_t*>(&nameSize) + sizeof(size_t));
            serializedData.insert(serializedData.end(), prefabElement->GetPrefabName().begin(), prefabElement->GetPrefabName().end());

            break;
        case MapElement::Type::ColliderElement:
            break;
        case MapElement::Type::AudioSouce:
            break;
        }
    }
}

Map Map::Deserialize(const std::vector<uint8_t>& data)
{
}

}
