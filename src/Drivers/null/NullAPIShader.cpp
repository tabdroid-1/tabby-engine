#include <tbpch.h>
#include <Drivers/null/NullAPIShader.h>

namespace Tabby {

NullAPIShader::NullAPIShader(const std::string& filepath)
{
}

NullAPIShader::NullAPIShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource)
{
}

NullAPIShader::~NullAPIShader()
{
}

void NullAPIShader::Bind() const
{
}

void NullAPIShader::Unbind() const
{
}

void NullAPIShader::SetBool(const std::string& name, bool value)
{
}

void NullAPIShader::SetInt(const std::string& name, int value)
{
}

void NullAPIShader::SetIntArray(const std::string& name, int* values, uint32_t count)
{
}

void NullAPIShader::SetFloatArray(const std::string& name, float* values, uint32_t count)
{
}

void NullAPIShader::SetFloat(const std::string& name, float value)
{
}

void NullAPIShader::SetFloat2(const std::string& name, const glm::vec2& value)
{
}

void NullAPIShader::SetFloat3(const std::string& name, const glm::vec3& value)
{
}

void NullAPIShader::SetFloat4(const std::string& name, const glm::vec4& value)
{
}

void NullAPIShader::SetMat3(const std::string& name, const glm::mat3& value)
{
}

void NullAPIShader::SetMat4(const std::string& name, const glm::mat4& value)
{
}
}
