#include <tbpch.h>
#include <Drivers/gl33/OpenGL33VertexArray.h>
#include <Drivers/GPUProfiler.h>

#include <gl.h>

namespace Tabby {

static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
{
    switch (type) {
    case ShaderDataType::Float:
        return GL_FLOAT;
    case ShaderDataType::Float2:
        return GL_FLOAT;
    case ShaderDataType::Float3:
        return GL_FLOAT;
    case ShaderDataType::Float4:
        return GL_FLOAT;
    case ShaderDataType::Mat3:
        return GL_FLOAT;
    case ShaderDataType::Mat4:
        return GL_FLOAT;
    case ShaderDataType::Int:
        return GL_INT;
    case ShaderDataType::Int2:
        return GL_INT;
    case ShaderDataType::Int3:
        return GL_INT;
    case ShaderDataType::Int4:
        return GL_INT;
    case ShaderDataType::Bool:
        return GL_BOOL;
    default:
        TB_CORE_ASSERT_TAGGED(false, "Unknown ShaderDataType!");
        return 0;
    }
}

OpenGL33VertexArray::OpenGL33VertexArray()
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33VertexArray::Constructor");
    TB_PROFILE_GPU("Tabby::OpenGL33VertexArray::Constructor");

    glGenVertexArrays(1, &m_RendererID);
}

OpenGL33VertexArray::~OpenGL33VertexArray()
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33VertexArray::Destructor");
    TB_PROFILE_GPU("Tabby::OpenGL33VertexArray::Destructor");

    glDeleteVertexArrays(1, &m_RendererID);
}

void OpenGL33VertexArray::Bind() const
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33VertexArray::Bind");
    TB_PROFILE_GPU("Tabby::OpenGL33VertexArray::Bind");

    glBindVertexArray(m_RendererID);
}

void OpenGL33VertexArray::Unbind() const
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33VertexArray::Unbind");
    TB_PROFILE_GPU("Tabby::OpenGL33VertexArray::Unbind");

    glBindVertexArray(0);
}

void OpenGL33VertexArray::AddVertexBuffer(const Shared<VertexBuffer>& vertexBuffer)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33VertexArray::AddVertexBuffer");
    TB_PROFILE_GPU("Tabby::OpenGL33VertexArray::AddVertexBuffer");

    TB_CORE_ASSERT_TAGGED(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

    glBindVertexArray(m_RendererID);
    vertexBuffer->Bind();

    const auto& layout = vertexBuffer->GetLayout();
    for (const auto& element : layout) {
        switch (element.Type) {
        case ShaderDataType::Float:
        case ShaderDataType::Float2:
        case ShaderDataType::Float3:
        case ShaderDataType::Float4: {
            glEnableVertexAttribArray(m_VertexBufferIndex);
            glVertexAttribPointer(m_VertexBufferIndex,
                element.GetComponentCount(),
                ShaderDataTypeToOpenGLBaseType(element.Type),
                element.Normalized ? GL_TRUE : GL_FALSE,
                layout.GetStride(),
                (const void*)element.Offset);
            m_VertexBufferIndex++;
            break;
        }
        case ShaderDataType::Int:
        case ShaderDataType::Int2:
        case ShaderDataType::Int3:
        case ShaderDataType::Int4:
        case ShaderDataType::Bool: {
            glEnableVertexAttribArray(m_VertexBufferIndex);
            glVertexAttribIPointer(m_VertexBufferIndex,
                element.GetComponentCount(),
                ShaderDataTypeToOpenGLBaseType(element.Type),
                layout.GetStride(),
                (const void*)element.Offset);
            m_VertexBufferIndex++;
            break;
        }
        case ShaderDataType::Mat3:
        case ShaderDataType::Mat4: {
            uint8_t count = element.GetComponentCount();
            for (uint8_t i = 0; i < count; i++) {
                glEnableVertexAttribArray(m_VertexBufferIndex);
                glVertexAttribPointer(m_VertexBufferIndex,
                    count,
                    ShaderDataTypeToOpenGLBaseType(element.Type),
                    element.Normalized ? GL_TRUE : GL_FALSE,
                    layout.GetStride(),
                    (const void*)(element.Offset + sizeof(float) * count * i));
                glVertexAttribDivisor(m_VertexBufferIndex, 1);
                m_VertexBufferIndex++;
            }
            break;
        }
        default:
            TB_CORE_ASSERT_TAGGED(false, "Unknown ShaderDataType!");
        }
    }

    m_VertexBuffers.push_back(vertexBuffer);
}

void OpenGL33VertexArray::SetIndexBuffer(const Shared<IndexBuffer>& indexBuffer)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33VertexArray::SetIndexBuffer");
    TB_PROFILE_GPU("Tabby::OpenGL33VertexArray::SetIndexBuffer");

    glBindVertexArray(m_RendererID);
    indexBuffer->Bind();

    m_IndexBuffer = indexBuffer;
}

}
