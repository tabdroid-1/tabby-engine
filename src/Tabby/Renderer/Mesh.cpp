#include <Tabby/Renderer/Mesh.h>

namespace Tabby {

Shared<Mesh> Mesh::Create(const MeshSpecification& spec)
{
    return CreateShared<Mesh>(spec);
}

Mesh::Mesh(const MeshSpecification& spec)
    : m_Specification(spec)
    , m_Transform(Matrix4(1.0f))
{
}

Mesh::~Mesh()
{
    bgfx::destroy(m_Specification.vertex_buffer_handle);
    bgfx::destroy(m_Specification.index_buffer_handle);
    for (auto& buffer : m_Specification.uniform_buffer_handles)
        bgfx::destroy(buffer.second);
}

}
