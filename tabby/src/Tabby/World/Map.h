#pragma once

#include <Tabby.h>
#include <Tabby/World/MapElement.h>

namespace Tabby {

class Map {
public:
    Map(const std::string& mapName);

private:
    std::unordered_map<UUID, MapElement> m_MapElements;
};

}
