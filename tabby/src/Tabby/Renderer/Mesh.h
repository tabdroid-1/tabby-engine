#pragma once

#include <Tabby/Renderer/VertexArray.h>

namespace Tabby {

class Mesh {

public:
    struct MeshVertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
    };

    Mesh(Shared<VertexArray> vertexArray, Shared<VertexBuffer> vertexBuffer);

    // Shared<Mesh> Create(const std::string& filePath);

private:
    Shared<VertexArray> m_VertexArray;
    Shared<VertexBuffer> m_VertexBuffer;
};

}
