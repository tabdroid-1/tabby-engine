#pragma once

#include "Pipeline.h"

namespace Tabby {

struct ShaderBufferLayoutElement {
    std::string name;
    ShaderDataType format;
    uint32_t size = 0;
    uint32_t offset = 0;

    ShaderBufferLayoutElement(const std::string& in_name, ShaderDataType format)
        : name(in_name)
        , format(format)
    {
    }

    ShaderBufferLayoutElement(ShaderBufferLayoutElement& other)
    {
        name = other.name;
        format = other.format;
        size = other.size;
        offset = other.offset;
    }

    ShaderBufferLayoutElement(const ShaderBufferLayoutElement& other)
    {
        name = other.name;
        format = other.format;
        size = other.size;
        offset = other.offset;
    }

    bool operator==(const ShaderBufferLayoutElement& other) const
    {
        bool result = true;
        result &= format == other.format;
        result &= size == other.size;
        result &= offset == other.offset;

        return result;
    }
};

class ShaderBufferLayout {
public:
    ShaderBufferLayout() { }
    ShaderBufferLayout(const std::vector<ShaderBufferLayoutElement>& list)
        : m_Elements(list)
    {
        for (auto& element : m_Elements) {
            uint32_t datasize = ShaderDataTypeSize(element.format);
            element.offset = m_Stride;
            element.size = datasize;
            m_Stride += datasize;
        }
    };

    ShaderBufferLayout(ShaderBufferLayout& other)
    {
        m_Stride = other.m_Stride;
        m_Elements = other.m_Elements;
    }

    ShaderBufferLayout(const ShaderBufferLayout& other)
    {
        m_Stride = other.m_Stride;
        m_Elements = other.m_Elements;
    }

    uint32_t GetStride() const { return m_Stride; }
    const std::vector<ShaderBufferLayoutElement>& GetElements() const { return m_Elements; }
    std::vector<ShaderBufferLayoutElement>::iterator begin() { return m_Elements.begin(); }
    std::vector<ShaderBufferLayoutElement>::iterator end() { return m_Elements.end(); }
    std::vector<ShaderBufferLayoutElement>::const_iterator begin() const { return m_Elements.begin(); }
    std::vector<ShaderBufferLayoutElement>::const_iterator end() const { return m_Elements.end(); }
    // const std::vector<ShaderBufferLayoutElement>::iterator& begin() { return m_Elements.begin(); }
    // const std::vector<ShaderBufferLayoutElement>::iterator& end() { return m_Elements.end(); }

private:
    std::vector<ShaderBufferLayoutElement> m_Elements;
    uint32_t m_Stride = 0;
};
}
