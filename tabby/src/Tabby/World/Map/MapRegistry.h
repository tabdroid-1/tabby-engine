#pragma once

namespace Tabby {

class Map;

class MapRegistry {
public:
    MapRegistry();
    MapRegistry(const std::string& path);

private:
    std::unordered_map<UUID, Shared<Map>> m_Maps;
};

}
