#include <Tabby/Renderer/GLTF.h>

#include <Tabby/Renderer/Buffer.h>
#include <Tabby/Renderer/VertexArray.h>

namespace Tabby {

tinygltf::TinyGLTF GLTF::m_Loader;

static ShaderDataType GLTFTypeToShaderDataType(int type)
{

    switch (type) {
    case TINYGLTF_TYPE_SCALAR:
        return ShaderDataType::Int;
    case TINYGLTF_TYPE_VEC2:
        return ShaderDataType::Float2;
    case TINYGLTF_TYPE_VEC3:
        return ShaderDataType::Float3;
    case TINYGLTF_TYPE_VEC4:
        return ShaderDataType::Float4;
    // case TINYGLTF_TYPE_MAT2:
    //     return ShaderDataType::Mat2;
    case TINYGLTF_TYPE_MAT3:
        return ShaderDataType::Mat3;
    case TINYGLTF_TYPE_MAT4:
        return ShaderDataType::Mat4;
    }

    TB_CORE_ASSERT(false, "Unknown ShaderDataType!");
    return ShaderDataType::Float;
}

GLTF::GLTF(const std::string& filePath)
{
    std::string err;
    std::string warn;

    bool ret;
    if (std::filesystem::path(filePath).extension().string() == ".gltf")
        ret = m_Loader.LoadASCIIFromFile(&m_Model, &err, &warn, filePath);
    else if (std::filesystem::path(filePath).extension().string() == ".glb")
        ret = m_Loader.LoadBinaryFromFile(&m_Model, &err, &warn, filePath);
    else
        TB_CORE_ASSERT(false, "Unknown File Format");

    if (!warn.empty()) {
        TB_CORE_WARN("{0}", warn);
    }

    if (!err.empty()) {
        TB_CORE_ERROR("{0}", err);
    }

    TB_CORE_ASSERT(ret, "Failed to parse glTF");

    Initialize();
}

void GLTF::Initialize()
{
    m_VertexArray = VertexArray::Create();
    m_VertexArray->Bind();

    const tinygltf::Scene& scene = m_Model.scenes[m_Model.defaultScene];
    for (size_t i = 0; i < scene.nodes.size(); i++) {
        TB_CORE_ASSERT((scene.nodes[i] >= 0) && (scene.nodes[i] < scene.nodes.size()), "Invalid node!");
    }

    m_VertexArray->Unbind();

    for (auto it = m_IndexBuffers.cbegin(); it != m_IndexBuffers.cend();) {
        tinygltf::BufferView bufferView = m_Model.bufferViews[it->first];
        if (bufferView.target != 0x8893) { // GL_ELEMENT_ARRAY_BUFFER
            // glDeleteBuffers(1, &ebos[it->first]);
            m_IndexBuffers.erase(it->first);
        } else {
            ++it;
        }
    }

    // m_VertexBuffer = VertexBuffer::Create(m_Model.meshes.at(0).primitives.at(0).mode);
    // m_VertexBuffer->SetLayout(m_BufferElements);
    // m_VertexArray->AddVertexBuffer(m_VertexBuffer);
}

void GLTF::BindModelNodes(tinygltf::Node& node)
{
    if ((node.mesh >= 0) && (node.mesh < m_Model.meshes.size())) {
        BindMesh(m_Model.meshes[node.mesh]);
    }

    for (size_t i = 0; i < node.children.size(); i++) {
        TB_CORE_ASSERT((node.children[i] >= 0) && (node.children[i] < m_Model.nodes.size()), "Invalid node!");
        BindModelNodes(m_Model.nodes[node.children[i]]);
    }
}

void GLTF::BindMesh(tinygltf::Mesh& mesh)
{
    for (size_t i = 0; i < m_Model.bufferViews.size(); i++) {
        const tinygltf::BufferView& bufferView = m_Model.bufferViews[i];
        if (bufferView.target == 0) {
            continue;
        }

        const tinygltf::Buffer& buffer = m_Model.buffers[bufferView.buffer];
        // m_IndexBuffers[i] = IndexBuffer::Create(reinterpret_cast<const uint32_t*>(&buffer.data.at(0) + bufferView.byteOffset), bufferView.byteLength / sizeof(uint32_t));
        m_IndexBuffers[i] = IndexBuffer::Create((uint32_t*)(&buffer.data.at(0) + bufferView.byteOffset), bufferView.byteLength);
    }

    for (size_t i = 0; i < mesh.primitives.size(); i++) {
        tinygltf::Primitive primitive = mesh.primitives[i];
        tinygltf::Accessor indexAccessor = m_Model.accessors[primitive.indices];

        for (auto& attrib : primitive.attributes) {
            tinygltf::Accessor accessor = m_Model.accessors[attrib.second];
            int byteStride = accessor.ByteStride(m_Model.bufferViews[accessor.bufferView]);
            m_IndexBuffers[accessor.bufferView]->Bind();

            int size = 1;
            if (accessor.type != TINYGLTF_TYPE_SCALAR)
                size = accessor.type;

            // static ShaderDataType OpenGLBaseTypeToShaderDataType(int type)
            // {
            //     switch (type) {
            //     case GL_FLOAT:
            //         return ShaderDataType::Float;
            //     case GL_INT:
            //         return ShaderDataType::Int;
            //     case GL_BOOL:
            //         return ShaderDataType::Bool;
            //     }
            //
            //     TB_CORE_ASSERT(false, "Unknown ShaderDataType!");
            //     return ShaderDataType::Int;
            // }

            // glEnableVertexAttribArray(attribLocation);
            // glVertexAttribPointer(attribLocation, accessor.type, accessor.componentType,
            //     accessor.normalized ? GL_TRUE : GL_FALSE,
            //     accessor.ByteStride(bufferView), BUFFER_OFFSET(accessor.byteOffset));
            int attribute = -1;
            if (attrib.first.compare("POSITION") == 0)
                m_BufferElements[0] = { GLTFTypeToShaderDataType(accessor.componentType), "o_Position", accessor.normalized };
            if (attrib.first.compare("TEXCOORD_0") == 0)
                m_BufferElements[1] = { GLTFTypeToShaderDataType(accessor.componentType), "a_TexCoord", accessor.normalized };
            if (attrib.first.compare("NORMAL") == 0)
                m_BufferElements[2] = { GLTFTypeToShaderDataType(accessor.componentType), "o_Normal", accessor.normalized };
            // if (attribute > 0) {
            //     // layout(location = 0) in vec3 a_Position;
            //     // layout(location = 1) in vec2 a_TexCoord;
            //     // layout(location = 2) in vec3 a_Normal;
            //
            //     elements[attribute] =
            // }
        }
    }
}

void GLTF::Draw()
{
    const tinygltf::Scene& scene = m_Model.scenes[m_Model.defaultScene];
    for (int i = 0; i < scene.nodes.size(); i++) {
        DrawNode(m_Model.nodes[scene.nodes[i]]);
    }
}

void GLTF::DrawNode(tinygltf::Node& node)
{
    if ((node.mesh >= 0) && (node.mesh < m_Model.meshes.size())) {
        DrawMesh(m_Model.meshes[node.mesh]);
    }
    for (size_t i = 0; i < node.children.size(); i++) {
        DrawNode(m_Model.nodes[node.children[i]]);
    }
}

void GLTF::DrawMesh(tinygltf::Mesh& mesh)
{
    for (size_t i = 0; i < mesh.primitives.size(); ++i) {
        tinygltf::Primitive primitive = mesh.primitives[i];
        tinygltf::Accessor indexAccessor = m_Model.accessors[primitive.indices];

        m_IndexBuffers.at(indexAccessor.bufferView)->Bind();
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebos.at(indexAccessor.bufferView));

        // glDrawElements(primitive.mode, indexAccessor.count,
        //     indexAccessor.componentType,
        //     ((char*)NULL + indexAccessor.byteOffset));
    }
}

}
