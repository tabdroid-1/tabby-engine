#include <Tabby/Renderer/Mesh.h>
#include <tinyobjloader/tiny_obj_loader.h>

namespace Tabby {

Mesh::Mesh(Shared<VertexArray> vertexArray, Shared<VertexBuffer> vertexBuffer)
{
}

// Shared<Mesh> Mesh::Create(const std::string& filePath)
// {
//     tinyobj::attrib_t attrib;
//     std::vector<tinyobj::shape_t> shapes;
//     std::string warn;
//     std::string err;
//     std::vector<tinyobj::material_t> materials;
//     bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str());
//
//     if (!err.empty()) {
//         TB_CORE_ERROR("{0}", err);
//     }
//
//     return CreateShared<Mesh>();
// }

}
