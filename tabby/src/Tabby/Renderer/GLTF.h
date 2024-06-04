#pragma once

// #include <Tabby/Renderer/Buffer.h>
// #include <Tabby/Renderer/VertexArray.h>

#include <tinygltf/tiny_gltf.h>

namespace Tabby {

class VertexArray;
class VertexBuffer;
class IndexBuffer;
struct BufferElement;

class GLTF {
public:
    GLTF(const std::string& filePath);

    void Draw();

private:
    void Initialize();
    void BindModelNodes(tinygltf::Node& node);
    void BindMesh(tinygltf::Mesh& mesh);

    void DrawNode(tinygltf::Node& nodes);
    void DrawMesh(tinygltf::Mesh& mesh);

private:
    // TODO: Move this to asset manager
    static tinygltf::TinyGLTF m_Loader;

    tinygltf::Model m_Model;
    Shared<VertexArray> m_VertexArray;
    Shared<VertexBuffer> m_VertexBuffer;
    std::unordered_map<int, Shared<IndexBuffer>> m_IndexBuffers;

    std::vector<BufferElement> m_BufferElements;
};

}
