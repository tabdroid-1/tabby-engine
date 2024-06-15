#pragma once

#include <Tabby/Renderer/VertexArray.h>
#include <Tabby/Foundation/Types.h>
#include <Tabby/Renderer/Material.h>

namespace Tabby {

class Mesh {
public:
    Mesh();
    ~Mesh();

    struct Vertex {
        glm::vec4 Position;
        glm::vec2 TexCoords;
        glm::vec3 Normal;
    };

    enum DrawMode {
        TRIANGLE,
        LINE
    };

    enum class PrimitiveType {
        // LINE,
        // QUAD,
        // CUBE,
        // SPHERE,
        // CYLINDER,
        // CONE,
        // MODEL
        Points = 0,
        Lines = 1,
        LineLoop = 2,
        LineStrip = 3,
        Triangles = 4,
        TriangleStrip = 5,
        TriangleFan = 6,
    };

    void SetName(const std::string& name);
    void SetTransform(const glm::mat4& transform);
    void SetPrimitiveType(PrimitiveType primitiveType);
    void SetMaterial(Shared<Material> material);
    void SetVertices(std::vector<Vertex> vertices);
    void SetIndices(std::vector<uint32_t> indices);

    void AddVertex(Vertex vertex);
    void AddIndex(uint32_t index);

    void CloneMesh(Shared<Mesh> mesh);

    /// <summary>
    /// 1. Sets draw mode(Triangle/Line)
    /// 2. Creates vertex array
    /// 3. Create vertex buffer and sets layout for it
    /// 4. Sets vertex buffer in created vertex array
    /// 5. Creates index buffer and sets that in vertex array
    /// </summary>
    /// <param name="drawMode"></param>
    void Create(DrawMode drawMode = DrawMode::TRIANGLE);

    void Render();

    void Destroy();

    // void ApplyTransform(const glm::mat4& matrix);
    // void ApplyTransform(Vector3 position = Vector3(0, 0, 0), Vector3 eulerAngles = Vector3(0, 0, 0), Vector3 scale = Vector3(1, 1, 1));
    const std::string GetName() const { return m_Name; }
    std::vector<Vertex> GetVertices() { return m_Vertices; }
    std::vector<Vertex> GetWorldSpaceVertices(const glm::mat4& matrix);
    std::vector<Vertex> GetWorldSpaceVertices(Vector3 position, Vector3 eulerAngles, Vector3 scale);
    std::vector<uint32_t> GetIndices() { return m_Indices; }
    Shared<Material> GetMaterial() const { return m_Material; }
    PrimitiveType GetPrimitiveType() { return m_PrimitiveType; }

private:
    std::string m_Name;
    glm::mat4 m_Transform;
    PrimitiveType m_PrimitiveType;
    std::vector<Vertex> m_Vertices;
    std::vector<uint32_t> m_Indices;
    Shared<VertexArray> m_VertexArray;
    bool m_StatsRegistered;
    DrawMode m_DrawMode;
    Shared<Material> m_Material;
};

}
