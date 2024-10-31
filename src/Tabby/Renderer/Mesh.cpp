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
    if (spec.flags & (uint64_t)MeshFlags::INDEX_TYPE_UINT8) {

        buffer_spec.flags |= (uint64_t)MeshFlags::INDEX_TYPE_UINT8;
        m_TotalIndexCount = spec.index_data_size / sizeof(uint8_t);

    } else if (spec.flags & (uint64_t)MeshFlags::INDEX_TYPE_UINT16) {

        buffer_spec.flags |= (uint64_t)MeshFlags::INDEX_TYPE_UINT16;
        m_TotalIndexCount = spec.index_data_size / sizeof(uint16_t);

    } else if (spec.flags & (uint64_t)MeshFlags::INDEX_TYPE_UINT32) {

        buffer_spec.flags |= (uint64_t)MeshFlags::INDEX_TYPE_UINT32;
        m_TotalIndexCount = spec.index_data_size / sizeof(uint32_t);
    }

    m_IndexBuffer = ShaderBuffer::Create(buffer_spec, m_Specification.index_data, m_Specification.index_data_size);

    if (!m_Specification.material)
        return;

    // if (m_Specification.vertex_data_size % m_Specification.material->GetShader()->GetVertexBufferSize() != 0) {
    //     TB_CORE_WARN("Vertex buffer size of shader that is from of material being set to mesh \"{}\", is not divisible by the mesh's vertex buffer size. Vertex data might be not suitable for the shader.", m_Specification.name);
    // }
}

void Mesh::Destroy()
{
    if (m_Specification.material)
        m_Specification.material->Destroy();
    m_VertexBuffer->Destroy();
    m_IndexBuffer->Destroy();
}

void Mesh::SetMaterial(Shared<Material> material)
{
    m_Specification.material = material;
    // if (m_Specification.vertex_data_size % material->GetShader()->GetVertexBufferSize() != 0) {
    //     TB_CORE_WARN("Vertex buffer size of shader that is from of material being set to mesh \"{}\", is not divisible by the mesh's vertex buffer size. Vertex data might be not suitable for the shader.", m_Specification.name);
    // }
}

Shared<Material> Mesh::GetMaterial() { return m_Specification.material; }
const std::string& Mesh::GetName() { return m_Specification.name; }
const MeshSpecification& Mesh::GetSpecification() { return m_Specification; }
Shared<ShaderBuffer> Mesh::GetVertexBuffer() { return m_VertexBuffer; }
Shared<ShaderBuffer> Mesh::GetIndexBuffer() { return m_IndexBuffer; }
uint32_t Mesh::TotalIndexCount() { return m_TotalIndexCount; }

}
// Allocating device image:
// 	Allocated size: 6.48(MiB)
// 	Currently allocated: 6.48(MiB)
// Allocating device image:
// 	Allocated size: 1.497(MiB)
// 	Currently allocated: 7.977(MiB)
// Allocating device buffer:
// 	Requested size: 1.497(MiB)
// 	Allocated size: 1.497(MiB)
// 	Currently allocated: 9.475(MiB)
// Destroying device buffer:
// 	Freed memory: 1.497(MiB)
// 	Currently allocated: 7.977(MiB)
// Allocating device buffer:
// 	Requested size: 483.2(KiB)
// 	Allocated size: 483.2(KiB)
// 	Currently allocated: 8.461(MiB)
// Allocating device buffer:
// 	Requested size: 483.2(KiB)
// 	Allocated size: 483.2(KiB)
// 	Currently allocated: 8.944(MiB)
// Allocating device buffer:
// 	Requested size: 377.2(KiB)
// 	Allocated size: 377.2(KiB)
// 	Currently allocated: 9.321(MiB)
// Allocating device buffer:
// 	Requested size: 377.2(KiB)
// 	Allocated size: 377.2(KiB)
// 	Currently allocated: 9.698(MiB)
// Allocating device buffer:
// 	Requested size: 78.64(KiB)
// 	Allocated size: 78.64(KiB)
// 	Currently allocated: 9.777(MiB)
// Allocating device buffer:
// 	Requested size: 78.64(KiB)
// 	Allocated size: 78.64(KiB)
// 	Currently allocated: 9.856(MiB)
// Allocating device buffer:
// 	Requested size: 41.66(KiB)
// 	Allocated size: 41.66(KiB)
// 	Currently allocated: 9.897(MiB)
// Allocating device buffer:
// 	Requested size: 41.66(KiB)
// 	Allocated size: 41.66(KiB)
// 	Currently allocated: 9.939(MiB)
// Allocating device buffer:
// 	Requested size: 197(KiB)
// 	Allocated size: 197(KiB)
// 	Currently allocated: 10.14(MiB)
// Allocating device buffer:
// 	Requested size: 197(KiB)
// 	Allocated size: 197(KiB)
// 	Currently allocated: 10.33(MiB)
// Allocating device buffer:
// 	Requested size: 212.3(KiB)
// 	Allocated size: 212.3(KiB)
// 	Currently allocated: 10.55(MiB)
// Allocating device buffer:
// 	Requested size: 212.3(KiB)
// 	Allocated size: 212.3(KiB)
// 	Currently allocated: 10.76(MiB)
// Allocating device buffer:
// 	Requested size: 45.18(KiB)
// 	Allocated size: 45.18(KiB)
// 	Currently allocated: 10.8(MiB)
// Allocating device buffer:
// 	Requested size: 45.18(KiB)
// 	Allocated size: 45.18(KiB)
// 	Currently allocated: 10.85(MiB)
// Allocating device buffer:
// 	Requested size: 49.03(KiB)
// 	Allocated size: 49.03(KiB)
// 	Currently allocated: 10.9(MiB)
// Allocating device buffer:
// 	Requested size: 49.03(KiB)
// 	Allocated size: 49.03(KiB)
// 	Currently allocated: 10.95(MiB)
// Allocating device buffer:
// 	Requested size: 21.76(KiB)
// 	Allocated size: 21.76(KiB)
// 	Currently allocated: 10.97(MiB)
// Allocating device buffer:
// 	Requested size: 21.76(KiB)
// 	Allocated size: 21.76(KiB)
// 	Currently allocated: 10.99(MiB)
// Allocating device buffer:
// 	Requested size: 9.552(KiB)
// 	Allocated size: 9.552(KiB)
// 	Currently allocated: 11(MiB)
// Allocating device buffer:
// 	Requested size: 9.552(KiB)
// 	Allocated size: 9.552(KiB)
// 	Currently allocated: 11.01(MiB)
// Allocating device buffer:
// 	Requested size: 227.3(KiB)
// 	Allocated size: 227.3(KiB)
// 	Currently allocated: 11.24(MiB)
// Allocating device buffer:
// 	Requested size: 227.3(KiB)
// 	Allocated size: 227.3(KiB)
// 	Currently allocated: 11.46(MiB)
// Allocating device buffer:
// 	Requested size: 122(KiB)
// 	Allocated size: 122(KiB)
// 	Currently allocated: 11.59(MiB)
// Allocating device buffer:
// 	Requested size: 122(KiB)
// 	Allocated size: 122(KiB)
// 	Currently allocated: 11.71(MiB)
// Allocating device buffer:
// 	Requested size: 101.3(KiB)
// 	Allocated size: 101.3(KiB)
// 	Currently allocated: 11.81(MiB)
// Allocating device buffer:
// 	Requested size: 101.3(KiB)
// 	Allocated size: 101.3(KiB)
// 	Currently allocated: 11.91(MiB)
// Allocating device buffer:
// 	Requested size: 70.51(KiB)
// 	Allocated size: 70.51(KiB)
// 	Currently allocated: 11.98(MiB)
// Allocating device buffer:
// 	Requested size: 70.51(KiB)
// 	Allocated size: 70.51(KiB)
// 	Currently allocated: 12.05(MiB)
// Allocating device buffer:
// 	Requested size: 70.64(KiB)
// 	Allocated size: 70.64(KiB)
// 	Currently allocated: 12.12(MiB)
// Allocating device buffer:
// 	Requested size: 70.64(KiB)
// 	Allocated size: 70.64(KiB)
// 	Currently allocated: 12.19(MiB)
// Allocating device buffer:
// 	Requested size: 33.79(KiB)
// 	Allocated size: 33.79(KiB)
// 	Currently allocated: 12.23(MiB)
// Allocating device buffer:
// 	Requested size: 33.79(KiB)
// 	Allocated size: 33.79(KiB)
// 	Currently allocated: 12.26(MiB)
// Allocating device buffer:
// 	Requested size: 460(B)
// 	Allocated size: 460(B)
// 	Currently allocated: 12.26(MiB)
// Allocating device buffer:
// 	Requested size: 460(B)
// 	Allocated size: 460(B)
// 	Currently allocated: 12.26(MiB)
// Allocating device buffer:
// 	Requested size: 252(B)
// 	Allocated size: 252(B)
// 	Currently allocated: 12.26(MiB)
// Allocating device buffer:
// 	Requested size: 252(B)
// 	Allocated size: 252(B)
// 	Currently allocated: 12.26(MiB)
// Allocating device buffer:
// 	Requested size: 283.2(KiB)
// 	Allocated size: 283.2(KiB)
// 	Currently allocated: 12.54(MiB)
// Allocating device buffer:
// 	Requested size: 283.2(KiB)
// 	Allocated size: 283.2(KiB)
// 	Currently allocated: 12.83(MiB)
// Allocating device buffer:
// 	Requested size: 85.06(KiB)
// 	Allocated size: 85.06(KiB)
// 	Currently allocated: 12.91(MiB)
// Allocating device buffer:
// 	Requested size: 85.06(KiB)
// 	Allocated size: 85.06(KiB)
// 	Currently allocated: 13(MiB)
// Allocating device buffer:
// 	Requested size: 23.38(KiB)
// 	Allocated size: 23.38(KiB)
// 	Currently allocated: 13.02(MiB)
// Allocating device buffer:
// 	Requested size: 23.38(KiB)
// 	Allocated size: 23.38(KiB)
// 	Currently allocated: 13.04(MiB)
// Allocating device buffer:
// 	Requested size: 10.56(KiB)
// 	Allocated size: 10.56(KiB)
// 	Currently allocated: 13.05(MiB)
// Allocating device buffer:
// 	Requested size: 10.56(KiB)
// 	Allocated size: 10.56(KiB)
// 	Currently allocated: 13.07(MiB)
// Allocating device buffer:
// 	Requested size: 466.5(KiB)
// 	Allocated size: 466.5(KiB)
// 	Currently allocated: 13.53(MiB)
// Allocating device buffer:
// 	Requested size: 466.5(KiB)
// 	Allocated size: 466.5(KiB)
// 	Currently allocated: 14(MiB)
// Allocating device buffer:
// 	Requested size: 278.5(KiB)
// 	Allocated size: 278.5(KiB)
// 	Currently allocated: 14.28(MiB)
// Allocating device buffer:
// 	Requested size: 278.5(KiB)
// 	Allocated size: 278.5(KiB)
// 	Currently allocated: 14.56(MiB)
// Allocating device buffer:
// 	Requested size: 720(B)
// 	Allocated size: 720(B)
// 	Currently allocated: 14.56(MiB)
// Allocating device buffer:
// 	Requested size: 720(B)
// 	Allocated size: 720(B)
// 	Currently allocated: 14.56(MiB)
// Allocating device buffer:
// 	Requested size: 216(B)
// 	Allocated size: 216(B)
// 	Currently allocated: 14.56(MiB)
// Allocating device buffer:
// 	Requested size: 216(B)
// 	Allocated size: 216(B)
// 	Currently allocated: 14.56(MiB)
// Allocating device buffer:
// 	Requested size: 248.3(KiB)
// 	Allocated size: 248.3(KiB)
// 	Currently allocated: 14.81(MiB)
// Allocating device buffer:
// 	Requested size: 248.3(KiB)
// 	Allocated size: 248.3(KiB)
// 	Currently allocated: 15.05(MiB)
// Allocating device buffer:
// 	Requested size: 198(KiB)
// 	Allocated size: 198(KiB)
// 	Currently allocated: 15.25(MiB)
// Allocating device buffer:
// 	Requested size: 198(KiB)
// 	Allocated size: 198(KiB)
// 	Currently allocated: 15.45(MiB)
// Allocating device buffer:
// 	Requested size: 199.6(KiB)
// 	Allocated size: 199.6(KiB)
// 	Currently allocated: 15.65(MiB)
// Allocating device buffer:
// 	Requested size: 199.6(KiB)
// 	Allocated size: 199.6(KiB)
// 	Currently allocated: 15.85(MiB)
// Allocating device buffer:
// 	Requested size: 198.1(KiB)
// 	Allocated size: 198.1(KiB)
// 	Currently allocated: 16.05(MiB)
// Allocating device buffer:
// 	Requested size: 198.1(KiB)
// 	Allocated size: 198.1(KiB)
// 	Currently allocated: 16.25(MiB)
// Allocating device buffer:
// 	Requested size: 199.5(KiB)
// 	Allocated size: 199.5(KiB)
// 	Currently allocated: 16.44(MiB)
// Allocating device buffer:
// 	Requested size: 199.5(KiB)
// 	Allocated size: 199.5(KiB)
// 	Currently allocated: 16.64(MiB)
// Allocating device buffer:
// 	Requested size: 198.1(KiB)
// 	Allocated size: 198.1(KiB)
// 	Currently allocated: 16.84(MiB)
// Allocating device buffer:
// 	Requested size: 198.1(KiB)
// 	Allocated size: 198.1(KiB)
// 	Currently allocated: 17.04(MiB)
// Allocating device buffer:
// 	Requested size: 132.9(KiB)
// 	Allocated size: 132.9(KiB)
// 	Currently allocated: 17.17(MiB)
// Allocating device buffer:
// 	Requested size: 132.9(KiB)
// 	Allocated size: 132.9(KiB)
// 	Currently allocated: 17.31(MiB)
// Allocating device buffer:
// 	Requested size: 132.1(KiB)
// 	Allocated size: 132.1(KiB)
// 	Currently allocated: 17.44(MiB)
// Allocating device buffer:
// 	Requested size: 132.1(KiB)
// 	Allocated size: 132.1(KiB)
// 	Currently allocated: 17.57(MiB)
// Allocating device buffer:
// 	Requested size: 154.8(KiB)
// 	Allocated size: 154.8(KiB)
// 	Currently allocated: 17.73(MiB)
// Allocating device buffer:
// 	Requested size: 154.8(KiB)
// 	Allocated size: 154.8(KiB)
// 	Currently allocated: 17.88(MiB)
// Allocating device buffer:
// 	Requested size: 172(KiB)
// 	Allocated size: 172(KiB)
// 	Currently allocated: 18.05(MiB)
// Allocating device buffer:
// 	Requested size: 172(KiB)
// 	Allocated size: 172(KiB)
// 	Currently allocated: 18.22(MiB)
// Allocating device buffer:
// 	Requested size: 205(KiB)
// 	Allocated size: 205(KiB)
// 	Currently allocated: 18.43(MiB)
// Allocating device buffer:
// 	Requested size: 205(KiB)
// 	Allocated size: 205(KiB)
// 	Currently allocated: 18.63(MiB)
// Allocating device buffer:
// 	Requested size: 227.3(KiB)
// 	Allocated size: 227.3(KiB)
// 	Currently allocated: 18.86(MiB)
// Allocating device buffer:
// 	Requested size: 227.3(KiB)
// 	Allocated size: 227.3(KiB)
// 	Currently allocated: 19.09(MiB)
// Allocating device buffer:
// 	Requested size: 154.8(KiB)
// 	Allocated size: 154.8(KiB)
// 	Currently allocated: 19.24(MiB)
// Allocating device buffer:
// 	Requested size: 154.8(KiB)
// 	Allocated size: 154.8(KiB)
// 	Currently allocated: 19.4(MiB)
// Allocating device buffer:
// 	Requested size: 172(KiB)
// 	Allocated size: 172(KiB)
// 	Currently allocated: 19.57(MiB)
// Allocating device buffer:
// 	Requested size: 172(KiB)
// 	Allocated size: 172(KiB)
// 	Currently allocated: 19.74(MiB)
// Allocating device buffer:
// 	Requested size: 1.48(KiB)
// 	Allocated size: 1.48(KiB)
// 	Currently allocated: 19.74(MiB)
// Allocating device buffer:
// 	Requested size: 1.48(KiB)
// 	Allocated size: 1.48(KiB)
// 	Currently allocated: 19.75(MiB)
// Allocating device buffer:
// 	Requested size: 384(B)
// 	Allocated size: 384(B)
// 	Currently allocated: 19.75(MiB)
// Allocating device buffer:
// 	Requested size: 384(B)
// 	Allocated size: 384(B)
// 	Currently allocated: 19.75(MiB)
// Allocating device buffer:
// 	Requested size: 236.6(KiB)
// 	Allocated size: 236.6(KiB)
// 	Currently allocated: 19.98(MiB)
// Allocating device buffer:
// 	Requested size: 236.6(KiB)
// 	Allocated size: 236.6(KiB)
// 	Currently allocated: 20.22(MiB)
// Allocating device buffer:
// 	Requested size: 237.9(KiB)
// 	Allocated size: 237.9(KiB)
// 	Currently allocated: 20.46(MiB)
// Allocating device buffer:
// 	Requested size: 237.9(KiB)
// 	Allocated size: 237.9(KiB)
// 	Currently allocated: 20.69(MiB)
// Allocating device buffer:
// 	Requested size: 106.2(KiB)
// 	Allocated size: 106.2(KiB)
// 	Currently allocated: 20.8(MiB)
// Allocating device buffer:
// 	Requested size: 106.2(KiB)
// 	Allocated size: 106.2(KiB)
// 	Currently allocated: 20.91(MiB)
// Allocating device buffer:
// 	Requested size: 110.2(KiB)
// 	Allocated size: 110.2(KiB)
// 	Currently allocated: 21.02(MiB)
// Allocating device buffer:
// 	Requested size: 110.2(KiB)
// 	Allocated size: 110.2(KiB)
// 	Currently allocated: 21.13(MiB)
// Allocating device buffer:
// 	Requested size: 34.64(KiB)
// 	Allocated size: 34.64(KiB)
// 	Currently allocated: 21.16(MiB)
// Allocating device buffer:
// 	Requested size: 34.64(KiB)
// 	Allocated size: 34.64(KiB)
// 	Currently allocated: 21.2(MiB)
// Allocating device buffer:
// 	Requested size: 36.5(KiB)
// 	Allocated size: 36.5(KiB)
// 	Currently allocated: 21.23(MiB)
// Allocating device buffer:
// 	Requested size: 36.5(KiB)
// 	Allocated size: 36.5(KiB)
// 	Currently allocated: 21.27(MiB)
// Allocating device buffer:
// 	Requested size: 238.7(KiB)
// 	Allocated size: 238.7(KiB)
// 	Currently allocated: 21.51(MiB)
// Allocating device buffer:
// 	Requested size: 238.7(KiB)
// 	Allocated size: 238.7(KiB)
// 	Currently allocated: 21.75(MiB)
// Allocating device buffer:
// 	Requested size: 173.8(KiB)
// 	Allocated size: 173.8(KiB)
// 	Currently allocated: 21.92(MiB)
// Allocating device buffer:
// 	Requested size: 173.8(KiB)
// 	Allocated size: 173.8(KiB)
// 	Currently allocated: 22.09(MiB)
// Destroying device buffer:
// 	Freed memory: 483.2(KiB)
// 	Currently allocated: 21.61(MiB)
// Destroying device buffer:
// 	Freed memory: 377.2(KiB)
// 	Currently allocated: 21.23(MiB)
// Destroying device buffer:
// 	Freed memory: 78.64(KiB)
// 	Currently allocated: 21.16(MiB)
// Destroying device buffer:
// 	Freed memory: 41.66(KiB)
// 	Currently allocated: 21.11(MiB)
// Destroying device buffer:
// 	Freed memory: 197(KiB)
// 	Currently allocated: 20.92(MiB)
// Destroying device buffer:
// 	Freed memory: 212.3(KiB)
// 	Currently allocated: 20.7(MiB)
// Destroying device buffer:
// 	Freed memory: 45.18(KiB)
// 	Currently allocated: 20.66(MiB)
// Destroying device buffer:
// 	Freed memory: 49.03(KiB)
// 	Currently allocated: 20.61(MiB)
// Destroying device buffer:
// 	Freed memory: 21.76(KiB)
// 	Currently allocated: 20.59(MiB)
// Destroying device buffer:
// 	Freed memory: 9.552(KiB)
// 	Currently allocated: 20.58(MiB)
// Destroying device buffer:
// 	Freed memory: 227.3(KiB)
// 	Currently allocated: 20.35(MiB)
// Destroying device buffer:
// 	Freed memory: 122(KiB)
// 	Currently allocated: 20.23(MiB)
// Destroying device buffer:
// 	Freed memory: 101.3(KiB)
// 	Currently allocated: 20.13(MiB)
// Destroying device buffer:
// 	Freed memory: 70.51(KiB)
// 	Currently allocated: 20.06(MiB)
// Destroying device buffer:
// 	Freed memory: 70.64(KiB)
// 	Currently allocated: 19.99(MiB)
// Destroying device buffer:
// 	Freed memory: 33.79(KiB)
// 	Currently allocated: 19.95(MiB)
// Destroying device buffer:
// 	Freed memory: 460(B)
// 	Currently allocated: 19.95(MiB)
// Destroying device buffer:
// 	Freed memory: 252(B)
// 	Currently allocated: 19.95(MiB)
// Destroying device buffer:
// 	Freed memory: 283.2(KiB)
// 	Currently allocated: 19.67(MiB)
// Destroying device buffer:
// 	Freed memory: 85.06(KiB)
// 	Currently allocated: 19.58(MiB)
// Destroying device buffer:
// 	Freed memory: 23.38(KiB)
// 	Currently allocated: 19.56(MiB)
// Destroying device buffer:
// 	Freed memory: 10.56(KiB)
// 	Currently allocated: 19.55(MiB)
// Destroying device buffer:
// 	Freed memory: 466.5(KiB)
// 	Currently allocated: 19.08(MiB)
// Destroying device buffer:
// 	Freed memory: 278.5(KiB)
// 	Currently allocated: 18.81(MiB)
// Destroying device buffer:
// 	Freed memory: 720(B)
// 	Currently allocated: 18.81(MiB)
// Destroying device buffer:
// 	Freed memory: 216(B)
// 	Currently allocated: 18.8(MiB)
// Destroying device buffer:
// 	Freed memory: 248.3(KiB)
// 	Currently allocated: 18.56(MiB)
// Destroying device buffer:
// 	Freed memory: 198(KiB)
// 	Currently allocated: 18.36(MiB)
// Destroying device buffer:
// 	Freed memory: 199.6(KiB)
// 	Currently allocated: 18.16(MiB)
// Destroying device buffer:
// 	Freed memory: 198.1(KiB)
// 	Currently allocated: 17.96(MiB)
// Destroying device buffer:
// 	Freed memory: 199.5(KiB)
// 	Currently allocated: 17.76(MiB)
// Destroying device buffer:
// 	Freed memory: 198.1(KiB)
// 	Currently allocated: 17.56(MiB)
// Destroying device buffer:
// 	Freed memory: 132.9(KiB)
// 	Currently allocated: 17.43(MiB)
// Destroying device buffer:
// 	Freed memory: 132.1(KiB)
// 	Currently allocated: 17.3(MiB)
// Destroying device buffer:
// 	Freed memory: 154.8(KiB)
// 	Currently allocated: 17.14(MiB)
// Destroying device buffer:
// 	Freed memory: 172(KiB)
// 	Currently allocated: 16.97(MiB)
// Destroying device buffer:
// 	Freed memory: 205(KiB)
// 	Currently allocated: 16.77(MiB)
// Destroying device buffer:
// 	Freed memory: 227.3(KiB)
// 	Currently allocated: 16.54(MiB)
// Destroying device buffer:
// 	Freed memory: 154.8(KiB)
// 	Currently allocated: 16.38(MiB)
// Destroying device buffer:
// 	Freed memory: 172(KiB)
// 	Currently allocated: 16.21(MiB)
// Destroying device buffer:
// 	Freed memory: 1.48(KiB)
// 	Currently allocated: 16.21(MiB)
// Destroying device buffer:
// 	Freed memory: 384(B)
// 	Currently allocated: 16.21(MiB)
// Destroying device buffer:
// 	Freed memory: 236.6(KiB)
// 	Currently allocated: 15.97(MiB)
// Destroying device buffer:
// 	Freed memory: 237.9(KiB)
// 	Currently allocated: 15.74(MiB)
// Destroying device buffer:
// 	Freed memory: 106.2(KiB)
// 	Currently allocated: 15.63(MiB)
// Destroying device buffer:
// 	Freed memory: 110.2(KiB)
// 	Currently allocated: 15.52(MiB)
// Destroying device buffer:
// 	Freed memory: 34.64(KiB)
// 	Currently allocated: 15.49(MiB)
// Destroying device buffer:
// 	Freed memory: 36.5(KiB)
// 	Currently allocated: 15.45(MiB)
// Destroying device buffer:
// 	Freed memory: 238.7(KiB)
// 	Currently allocated: 15.21(MiB)
// Destroying device buffer:
// 	Freed memory: 173.8(KiB)
// 	Currently allocated: 15.04(MiB)
// Destroying device image:
// 	Freed memory: 1.497(MiB)
// 	Currently allocated: 13.54(MiB)
// Destroying device image:
// 	Freed memory: 6.48(MiB)
// 	Currently allocated: 7.059(MiB)
// Destroying vulkan memory allocator:
// 	Total memory allocated: 23.59(MiB)
// 	Total memory freed: 16.53(MiB)
// 	In use at the moment: 7.059(MiB)
