#include <Tabby/Renderer/ShaderBuffer.h>
#include <Tabby/Renderer/Material.h>
#include <Tabby/Foundation/Types.h>
#include <Tabby/Renderer/Shader.h>
#include <Tabby/Renderer/Mesh.h>

namespace Tabby {

Mesh::Mesh(const MeshSpecification& spec, AssetHandle handle)
    : m_Specification(spec)
{

    Handle = handle;
    Type = AssetType::TABBY_MESH;
    m_Specification = spec;

    ShaderBufferSpecification buffer_spec = {};
    buffer_spec.buffer_usage = ShaderBufferUsage::VERTEX_BUFFER;
    buffer_spec.heap = ShaderBufferMemoryHeap::DEVICE;
    buffer_spec.memory_usage = ShaderBufferMemoryUsage::NO_HOST_ACCESS;
    buffer_spec.size = m_Specification.vertex_data_size;

    m_VertexBuffer = ShaderBuffer::Create(buffer_spec, m_Specification.vertex_data, m_Specification.vertex_data_size);

    buffer_spec.buffer_usage = ShaderBufferUsage::INDEX_BUFFER;
    buffer_spec.heap = ShaderBufferMemoryHeap::DEVICE;
    buffer_spec.memory_usage = ShaderBufferMemoryUsage::NO_HOST_ACCESS;
    buffer_spec.size = m_Specification.index_data_size;
    buffer_spec.flags |= (uint64_t)ShaderBufferFlags::INDEX_TYPE_UINT16;

    m_IndexBuffer = ShaderBuffer::Create(buffer_spec, m_Specification.index_data, m_Specification.index_data_size);

    if (spec.index_type == MeshIndexType::UINT8)
        m_TotalIndexCount = spec.index_data_size / sizeof(uint8_t);
    else if (spec.index_type == MeshIndexType::UINT16)
        m_TotalIndexCount = spec.index_data_size / sizeof(uint16_t);
    else if (spec.index_type == MeshIndexType::UINT32)
        m_TotalIndexCount = spec.index_data_size / sizeof(uint32_t);

    if (!m_Specification.material)
        return;

    if (m_Specification.vertex_data_size % m_Specification.material->GetShader()->GetVertexBufferSize() != 0) {
        TB_CORE_WARN("Vertex buffer size of shader that is from of material being set to mesh \"{}\", is not divisible by the mesh's vertex buffer size. Vertex data might be not suitable for the shader.", m_Specification.name);
    }
}

void Mesh::Destroy()
{
    m_Specification.material->Destroy();
    m_VertexBuffer->Destroy();
    m_IndexBuffer->Destroy();
}

void Mesh::SetMaterial(Shared<Material> material)
{
    m_Specification.material = material;
    if (m_Specification.vertex_data_size % material->GetShader()->GetVertexBufferSize() != 0) {
        TB_CORE_WARN("Vertex buffer size of shader that is from of material being set to mesh \"{}\", is not divisible by the mesh's vertex buffer size. Vertex data might be not suitable for the shader.", m_Specification.name);
    }
}

Shared<Material> Mesh::GetMaterial() { return m_Specification.material; }
const std::string& Mesh::GetName() { return m_Specification.name; }
Shared<ShaderBuffer> Mesh::GetVertexBuffer() { return m_VertexBuffer; }
Shared<ShaderBuffer> Mesh::GetIndexBuffer() { return m_IndexBuffer; }
uint32_t Mesh::TotalIndexCount() { return m_TotalIndexCount; }

}
