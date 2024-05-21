#pragma once

#include <Tabby.h>

namespace Tabby {

class MapElement {
public:
    MapElement(const std::string& elementName);

private:
    UUID m_ID;
    std::string m_ElementName;
};
}
