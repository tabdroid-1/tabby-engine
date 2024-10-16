#include <tbpch.h>
#include <Drivers/gl33/OpenGL33Shader.h>
#include <Drivers/GPUProfiler.h>

#include <glm/gtc/type_ptr.hpp>
#include <SDL_rwops.h>
#include <gl.h>

namespace Tabby {

static GLenum ShaderTypeFromString(const std::string& type)
{
    if (type == "vertex")
        return GL_VERTEX_SHADER;
    if (type == "fragment" || type == "pixel")
        return GL_FRAGMENT_SHADER;
    TB_CORE_ASSERT_TAGGED(false, "unknown shader type");
    return 0;
}

OpenGL33Shader::OpenGL33Shader(const std::string& filepath)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33Shader::Constructor");
    TB_PROFILE_GPU("Tabby::OpenGL33Shader::Constructor");

    auto source = ReadFile(filepath);
    auto shaderSources = PreProcess(source);

    Compile(shaderSources, "Path: " + filepath);

    auto lastSlash = filepath.find_last_of("/\\");
    lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
    auto lastDot = filepath.rfind('.');

    auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;

    m_Name = filepath.substr(lastSlash, count);
}

OpenGL33Shader::OpenGL33Shader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource)
    : m_Name(name)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33Shader::Constructor");
    TB_PROFILE_GPU("Tabby::OpenGL33Shader::Constructor");

    std::unordered_map<GLenum, std::string> sources;
    sources[GL_VERTEX_SHADER] = vertexSource;
    sources[GL_FRAGMENT_SHADER] = fragmentSource;
    Compile(sources, "Name: " + name);
}

OpenGL33Shader::~OpenGL33Shader()
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33Shader::Destructor");
    TB_PROFILE_GPU("Tabby::OpenGL33Shader::Destructor");

    glDeleteProgram(m_RendererID);
}

std::unordered_map<GLenum, std::string> OpenGL33Shader::PreProcess(const std::string& source)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33Shader::PreProcess");
    std::unordered_map<GLenum, std::string> shaderSources;

    const char* typeToken = "#type";
    size_t typeTokenLength = strlen(typeToken);
    size_t pos = source.find(typeToken, 0);
    while (pos != std::string::npos) {
        size_t eol = source.find_first_of("\r\n", pos);
        TB_CORE_ASSERT_TAGGED(eol != std::string::npos, "Syntax error");
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
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33Shader::ReadFile");

    std::string result;

    SDL_RWops* rw = SDL_RWFromFile(filepath.c_str(), "rb");
    if (rw != nullptr) {
        Sint64 size = SDL_RWsize(rw);

        if (size > 0) {
            result.resize(size);
            Sint64 bytesRead = SDL_RWread(rw, &result[0], 1, size);
            if (bytesRead != size) {
                TB_CORE_ERROR("Error reading file {0}", filepath);
                result.clear();
            }
        }

        SDL_RWclose(rw);
    } else {
        TB_CORE_ERROR("Could not open file {0}", filepath);
    }

    return result;
}

// NOTE: ShaderInfo holds name or path of shader. this is to show path or name for the shader with error
void OpenGL33Shader::Compile(const std::unordered_map<GLenum, std::string>& shaderSource, const std::string& shaderInfo)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33Shader::Compile");
    TB_PROFILE_GPU("Tabby::OpenGL33Shader::Compile");

    GLuint program = glCreateProgram();
    TB_CORE_ASSERT_TAGGED(shaderSource.size() <= 2, "Only 3 shaders are supported");
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
            TB_CORE_ASSERT_TAGGED(false, "Shader compilation issue");

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
        TB_CORE_ASSERT_TAGGED(false, "Shader linking issue");
        return;
    }
    for (auto id : glShaderIDs)
        glDetachShader(program, id);

    m_RendererID = program;
}

void OpenGL33Shader::Bind() const
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33Shader::Bind");
    TB_PROFILE_GPU("Tabby::OpenGL33Shader::Bind");

    glUseProgram(m_RendererID);
}

void OpenGL33Shader::Unbind() const
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33Shader::Unbind");
    TB_PROFILE_GPU("Tabby::OpenGL33Shader::Unbind");

    glUseProgram(0);
}

void OpenGL33Shader::SetBool(const std::string& name, bool value)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33Shader::SetBool");
    TB_PROFILE_GPU("Tabby::OpenGL33Shader::SetBool");

    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform1i(location, (int)value);
}

void OpenGL33Shader::SetInt(const std::string& name, int value)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33Shader::SetInt");
    TB_PROFILE_GPU("Tabby::OpenGL33Shader::SetInt");

    UploadUniformInt(name, value);
}

void OpenGL33Shader::SetIntArray(const std::string& name, int* values, uint32_t count)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33Shader::SetIntArray");
    TB_PROFILE_GPU("Tabby::OpenGL33Shader::SetIntArray");

    UploadUniformIntArray(name, values, count);
}

void OpenGL33Shader::SetFloatArray(const std::string& name, float* values, uint32_t count)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33Shader::SetFloatArray");
    TB_PROFILE_GPU("Tabby::OpenGL33Shader::SetFloatArray");

    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform1fv(location, count, values);
}

void OpenGL33Shader::SetFloat(const std::string& name, float value)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33Shader::SetFloat");
    TB_PROFILE_GPU("Tabby::OpenGL33Shader::SetFloat");

    UploadUniformFloat(name, value);
}

void OpenGL33Shader::SetFloat2(const std::string& name, const glm::vec2& value)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33Shader::SetFloat2");
    TB_PROFILE_GPU("Tabby::OpenGL33Shader::SetFloat2");

    UploadUniformFloat2(name, value);
}

void OpenGL33Shader::SetFloat3(const std::string& name, const glm::vec3& value)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33Shader::SetFloat3");
    TB_PROFILE_GPU("Tabby::OpenGL33Shader::SetFloat3");

    UploadUniformFloat3(name, value);
}

void OpenGL33Shader::SetFloat4(const std::string& name, const glm::vec4& value)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33Shader::SetFloat4");
    TB_PROFILE_GPU("Tabby::OpenGL33Shader::SetFloat4");

    UploadUniformFloat4(name, value);
}

void OpenGL33Shader::SetMat3(const std::string& name, const glm::mat3& value)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33Shader::SetMat3");
    TB_PROFILE_GPU("Tabby::OpenGL33Shader::SetMat3");

    UploadUniformMat3(name, value);
}

void OpenGL33Shader::SetMat4(const std::string& name, const glm::mat4& value)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33Shader::SetMat4");
    TB_PROFILE_GPU("Tabby::OpenGL33Shader::SetMat4");

    UploadUniformMat4(name, value);
}

void OpenGL33Shader::UploadUniformInt(const std::string& name, int value)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33Shader::UploadUniformInt");
    TB_PROFILE_GPU("Tabby::OpenGL33Shader::UploadUniformInt");

    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform1i(location, value);
}

void OpenGL33Shader::UploadUniformIntArray(const std::string& name, int* values, uint32_t count)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33Shader::UploadUniformIntArray");
    TB_PROFILE_GPU("Tabby::OpenGL33Shader::UploadUniformIntArray");

    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform1iv(location, count, values);
}

void OpenGL33Shader::UploadUniformFloat(const std::string& name, float value)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33Shader::UploadUniformFloat");
    TB_PROFILE_GPU("Tabby::OpenGL33Shader::UploadUniformFloat");

    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform1f(location, value);
}

void OpenGL33Shader::UploadUniformFloat2(const std::string& name, const glm::vec2& value)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33Shader::UploadUniformFloat2");
    TB_PROFILE_GPU("Tabby::OpenGL33Shader::UploadUniformFloat2");

    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform2f(location, value.x, value.y);
}

void OpenGL33Shader::UploadUniformFloat3(const std::string& name, const glm::vec3& value)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33Shader::UploadUniformFloat3");
    TB_PROFILE_GPU("Tabby::OpenGL33Shader::UploadUniformFloat3");

    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform3f(location, value.x, value.y, value.z);
}

void OpenGL33Shader::UploadUniformFloat4(const std::string& name, const glm::vec4& value)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33Shader::UploadUniformFloat4");
    TB_PROFILE_GPU("Tabby::OpenGL33Shader::UploadUniformFloat4");

    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniform4f(location, value.x, value.y, value.z, value.w);
}

void OpenGL33Shader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33Shader::UploadUniformMat3");
    TB_PROFILE_GPU("Tabby::OpenGL33Shader::UploadUniformMat3");

    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void OpenGL33Shader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33Shader::UploadUniformMat4");
    TB_PROFILE_GPU("Tabby::OpenGL33Shader::UploadUniformMat4");

    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

}
