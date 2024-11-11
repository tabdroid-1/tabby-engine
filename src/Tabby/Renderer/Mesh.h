#pragma once

#include <Tabby/Foundation/Types.h>
#include <bgfx/bgfx.h>
#include <bx/bounds.h>

namespace Tabby {

struct MeshSpecification {
    std::string name = "Mesh";
    bgfx::ProgramHandle program_handle;
    bgfx::IndexBufferHandle index_buffer_handle;
    bgfx::VertexBufferHandle vertex_buffer_handle;
    std::unordered_map<std::string, bgfx::UniformHandle> uniform_buffer_handles;
    uint64_t state = 0;
};

class Mesh {
public:
    static Shared<Mesh> Create(const MeshSpecification& spec);

    Mesh(const MeshSpecification& spec);
    ~Mesh();

    void SetTransform(const Matrix4& matrix) { m_Transform = matrix; }
    const Matrix4& GetTransform() { return m_Transform; }

    const bgfx::UniformHandle& GetUniform(const std::string& uniform_name) const
    {
        auto it = m_Specification.uniform_buffer_handles.find(uniform_name);
        std::string message = "'" + uniform_name + "' not found for mesh '" + m_Specification.name + "'";
        TB_CORE_ASSERT_TAGGED(it == m_Specification.uniform_buffer_handles.end(), message);

        return it->second;
    }

private:
    MeshSpecification m_Specification;
    Matrix4 m_Transform;

    friend class Renderer;
};

}
