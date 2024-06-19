#pragma once

#include <Tabby.h>

namespace Tabby {

class MapElement;

class Map {
public:
    Map();
    static std::vector<uint8_t> Serialize(Map map);
    static Map Deserialize(const std::vector<uint8_t>& data);

private:
    UUID m_UUID;
    std::string m_Name;

    std::unordered_map<UUID, Shared<MapElement>> m_ElementRegistry;
};

}
