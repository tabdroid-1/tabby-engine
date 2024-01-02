#include "Drivers/gl33/OpenGL33Shader.h"
#include "Tabby/Core/Timer.h"
#include "tbpch.h"

#include <fstream>
#include <glad/gl33.h>

#include <glm/gtc/type_ptr.hpp>
#include <string>

// #include <shaderc/shaderc.hpp>
// #include <spirv_cross/spirv_cross.hpp>
// #include <spirv_cross/spirv_glsl.hpp>

namespace Tabby {

static GLenum ShaderTypeFromString(const std::string& type)
{
    if (type == "vertex")
        return GL_VERTEX_SHADER;
    if (type == "fragment" || type == "pixel")
        return GL_FRAGMENT_SHADER;
    TB_CORE_ASSERT(false, "unknown shader type");
    return 0;
}

OpenGL33Shader::OpenGL33Shader(const std::string& filepath)
{
    // TB_PROFILE_FUNCTION();

    auto source = ReadFile(filepath);
    auto shaderSources = PreProcess(source);

    Compile(shaderSources, "Path: " + filepath);

    // Assets/Shaders/TextureCombined.glsl
    auto lastSlash = filepath.find_last_of("/\\");
    lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
    auto lastDot = filepath.rfind('.');

    auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;

    m_Name = filepath.substr(lastSlash, count);
}

OpenGL33Shader::OpenGL33Shader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource)
    : m_Name(name)
{
    // TB_PROFILE_FUNCTION();

    std::unordered_map<GLenum, std::string> sources;
    sources[GL_VERTEX_SHADER] = vertexSource;
    sources[GL_FRAGMENT_SHADER] = fragmentSource;
    Compile(sources, "Name: " + name);
}

OpenGL33Shader::~OpenGL33Shader()
{
    // TB_PROFILE_FUNCTION();

    glDeleteProgram(m_RendererID);
}

std::unordered_map<GLenum, std::string> OpenGL33Shader::PreProcess(const std::string& source)
{
    std::unordered_map<GLenum, std::string> shaderSources;

    const char* typeToken = "#type";
    size_t typeTokenLength = strlen(typeToken);
    size_t pos = source.find(typeToken, 0);
    while (pos != std::string::npos) {
        size_t eol = source.find_first_of("\r\n", pos);
        TB_CORE_ASSERT(eol != std::string::npos, "Syntax error");
        size_t begin = pos + typeTokenLength + 1;
        std::string type = source.substr(begin, eol - begin);

        size_t nextLinePos = source.find_first_not_of("\r\n", eol);
        pos = source.find(typeToken, nextLinePos);
        shaderSources[ShaderTypeFromString(type)] = source.substr(nextLinePos,
            pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
    }
    return shaderSources;
}

std::string OpenGL33Shader::ReadFile(const std::string& filepath)
{
    std::string result;
    std::ifstream in(filepath, std::ios::in | std::ios::binary);

    if (in) {
        in.seekg(0, std::ios::end);
        result.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&result[0], result.size());
    } else {

        TB_CORE_ERROR("Could not open file {0}", filepath);
        TB_CORE_INFO("Current working dir: {0}", std::filesystem::current_path());
    }
    return result;
}

// NOTE: ShaderInfo holds name or path of shader. this is to show path or name for the shader with error
void OpenGL33Shader::Compile(const std::unordered_map<GLenum, std::string>& shaderSource, const std::string& shaderInfo)
{
    // TB_PROFILE_FUNCTION();

    GLuint program = glCreateProgram();
    TB_CORE_ASSERT(shaderSource.size() <= 2, "Only 3 shaders are supported");
    std::array<GLenum, 3> glShaderIDs;
    int glShaderIDIndex = 0;
    for (auto& kv_pair : shaderSource) {
        GLenum type = kv_pair.first;
        const std::string& sourceBase = kv_pair.second;

        GLuint shader = glCreateShader(type);

        const GLchar* source = sourceBase.c_str();
        glShaderSource(shader, 1, &source, 0);

        // Compile the vertex shader
        glCompileShader(shader);

        GLint isCompiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
        if (isCompiled == GL_FALSE) {
            GLint maxLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<GLchar> infoLog(maxLength);
            glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

            glDeleteShader(shader);

            TB_CORE_ERROR("Shader compilation error for {0}!", shaderInfo);
            TB_CORE_ERROR("{0}", infoLog.data());
            TB_CORE_ASSERT(false, "Shader compilation issue");

            break;
        }
        glAttachShader(program, shader);
        glShaderIDs[glShaderIDIndex++] = shader;
    }

    glLinkProgram(program);
    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
    if (isLinked != 1) {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

        // We don't need the program anymore.
        glDeleteProgram(program);

        for (auto id : glShaderIDs)
            glDeleteShader(id);

        TB_CORE_ERROR("Linking shader error for {0}!", shaderInfo);
        TB_CORE_ERROR("{0}", infoLog.data());
        TB_CORE_ASSERT(false, "Shader linking issue");
        return;
    }
    for (auto id : glShaderIDs)
        glDetachShader(program, id);

    m_RendererID = program;
}

void OpenGL33Shader::Bind() const
{
    // TB_PROFILE_FUNCTION();

    glUseProgram(m_RendererID);
}

void OpenGL33Shader::Unbind() const
{
    // TB_PROFILE_FUNCTION();

    glUseProgram(0);
}

void OpenGL33Shader::SetInt(const std::string& name, int value)
{
    // TB_PROFILE_FUNCTION();

    UploadUniformInt(name, value);
}

void OpenGL33Shader::SetIntArray(const std::string& name, int* values, uint32_t count)
{
    UploadUniformIntArray(name, values, count);
}

void OpenGL33Shader::SetFloat(const std::string& name, float value)
{
    // TB_PROFILE_FUNCTION();

    UploadUniformFloat(name, value);
}

void OpenGL33Shader::SetFloat2(const std::string& name, const glm::vec2& value)
{
    // TB_PROFILE_FUNCTION();

    UploadUniformFloat2(name, value);
}

void OpenGL33Shader::SetFloat3(const std::string& name, const glm::vec3& value)
{
    // TB_PROFILE_FUNCTION();

    UploadUniformFloat3(name, value);
}

void OpenGL33Shader::SetFloat4(const std::string& name, const glm::vec4& value)
{
    // TB_PROFILE_FUNCTION();

    UploadUniformFloat4(name, value);
}

void OpenGL33Shader::SetMat4(const std::string& name, const glm::mat4& value)
{
    // TB_PROFILE_FUNCTION();

    UploadUniformMat4(name, value);
}

void OpenGL33Shader::UploadUniformInt(const std::string& name, int value)
{
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform1i(location, value);
}

void OpenGL33Shader::UploadUniformIntArray(const std::string& name, int* values, uint32_t count)
{
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform1iv(location, count, values);
}

void OpenGL33Shader::UploadUniformFloat(const std::string& name, float value)
{
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform1f(location, value);
}

void OpenGL33Shader::UploadUniformFloat2(const std::string& name, const glm::vec2& value)
{
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform2f(location, value.x, value.y);
}

void OpenGL33Shader::UploadUniformFloat3(const std::string& name, const glm::vec3& value)
{
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform3f(location, value.x, value.y, value.z);
}

void OpenGL33Shader::UploadUniformFloat4(const std::string& name, const glm::vec4& value)
{
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform4f(location, value.x, value.y, value.z, value.w);
}

void OpenGL33Shader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
{
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void OpenGL33Shader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
{
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

}
