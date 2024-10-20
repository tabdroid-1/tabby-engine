#include <tbpch.h>
#include <Drivers/gles3/OpenGLES3Shader.h>
#include <Drivers/gles3/GLES3.h>

#include <glm/gtc/type_ptr.hpp>
#include <SDL_rwops.h>
#include <gles3.h>

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

OpenGLES3Shader::OpenGLES3Shader(const std::string& filepath)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Shader::Constructor");

    auto source = ReadFile(filepath);
    auto shaderSources = PreProcess(source);

    Compile(shaderSources, "Path: " + filepath);

    // Assets / Shaders / TextureCombined.glsl
    auto lastSlash = filepath.find_last_of("/\\");
    lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
    auto lastDot = filepath.rfind('.');

    auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;

    m_Name = filepath.substr(lastSlash, count);
}

OpenGLES3Shader::OpenGLES3Shader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource)
    : m_Name(name)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Shader::Constructor");

    std::unordered_map<GLenum, std::string> sources;
    sources[GL_VERTEX_SHADER] = vertexSource;
    sources[GL_FRAGMENT_SHADER] = fragmentSource;
    Compile(sources, "Name: " + name);
}

OpenGLES3Shader::~OpenGLES3Shader()
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Shader::Destructor");

    GLES::gl->DeleteProgram(m_RendererID);
}

std::unordered_map<GLenum, std::string> OpenGLES3Shader::PreProcess(const std::string& source)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Shader::PreProcess");

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

std::string OpenGLES3Shader::ReadFile(const std::string& filepath)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Shader::ReadFile");

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
void OpenGLES3Shader::Compile(const std::unordered_map<GLenum, std::string>& shaderSource, const std::string& shaderInfo)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Shader::Compile");

    GLuint program = GLES::gl->CreateProgram();
    TB_CORE_ASSERT_TAGGED(shaderSource.size() <= 2, "Only 3 shaders are supported");
    std::array<GLenum, 3> glShaderIDs;
    int glShaderIDIndex = 0;
    for (auto& kv_pair : shaderSource) {
        GLenum type = kv_pair.first;
        const std::string& sourceBase = kv_pair.second;

        GLuint shader = GLES::gl->CreateShader(type);

        const GLchar* source = sourceBase.c_str();
        GLES::gl->ShaderSource(shader, 1, &source, 0);

        // Compile the vertex shader
        GLES::gl->CompileShader(shader);

        GLint isCompiled = 0;
        GLES::gl->GetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
        if (isCompiled == GL_FALSE) {
            GLint maxLength = 0;
            GLES::gl->GetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<GLchar> infoLog(maxLength);
            GLES::gl->GetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

            GLES::gl->DeleteShader(shader);

            TB_CORE_ERROR("Shader compilation error for {0}!", shaderInfo);
            TB_CORE_ERROR("{0}", infoLog.data());
            TB_CORE_ASSERT_TAGGED(false, "Shader compilation issue");

            break;
        }
        GLES::gl->AttachShader(program, shader);
        glShaderIDs[glShaderIDIndex++] = shader;
    }

    GLES::gl->LinkProgram(program);
    GLint isLinked = 0;
    GLES::gl->GetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
    if (isLinked != 1) {
        GLint maxLength = 0;
        GLES::gl->GetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        GLES::gl->GetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

        // We don't need the program anymore.
        GLES::gl->DeleteProgram(program);

        for (auto id : glShaderIDs)
            GLES::gl->DeleteShader(id);

        TB_CORE_ERROR("Linking shader error for {0}!", shaderInfo);
        TB_CORE_ERROR("{0}", infoLog.data());
        TB_CORE_ASSERT_TAGGED(false, "Shader linking issue");
        return;
    }
    for (auto id : glShaderIDs)
        GLES::gl->DetachShader(program, id);

    m_RendererID = program;
}

void OpenGLES3Shader::Bind() const
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Shader::Bind");

    GLES::gl->UseProgram(m_RendererID);
}

void OpenGLES3Shader::Unbind() const
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Shader::Unbind");

    GLES::gl->UseProgram(0);
}

void OpenGLES3Shader::SetBool(const std::string& name, bool value)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Shader::SetBool");

    GLint location = GLES::gl->GetUniformLocation(m_RendererID, name.c_str());
    GLES::gl->Uniform1i(location, (int)value);
}

void OpenGLES3Shader::SetInt(const std::string& name, int value)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Shader::SetInt");

    UploadUniformInt(name, value);
}

void OpenGLES3Shader::SetIntArray(const std::string& name, int* values, uint32_t count)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Shader::SetIntArray");

    UploadUniformIntArray(name, values, count);
}

void OpenGLES3Shader::SetFloatArray(const std::string& name, float* values, uint32_t count)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Shader::SetFloatArray");

    GLint location = GLES::gl->GetUniformLocation(m_RendererID, name.c_str());
    GLES::gl->Uniform1fv(location, count, values);
}

void OpenGLES3Shader::SetFloat(const std::string& name, float value)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Shader::SetFloat");

    UploadUniformFloat(name, value);
}

void OpenGLES3Shader::SetFloat2(const std::string& name, const glm::vec2& value)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Shader::SetFloat2");

    UploadUniformFloat2(name, value);
}

void OpenGLES3Shader::SetFloat3(const std::string& name, const glm::vec3& value)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Shader::SetFloat3");

    UploadUniformFloat3(name, value);
}

void OpenGLES3Shader::SetFloat4(const std::string& name, const glm::vec4& value)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Shader::SetFloat4");

    UploadUniformFloat4(name, value);
}

void OpenGLES3Shader::SetMat3(const std::string& name, const glm::mat3& value)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Shader::SetMat3");

    UploadUniformMat3(name, value);
}

void OpenGLES3Shader::SetMat4(const std::string& name, const glm::mat4& value)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Shader::SetMat4");

    UploadUniformMat4(name, value);
}

void OpenGLES3Shader::UploadUniformInt(const std::string& name, int value)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Shader::UploadUniformInt");

    GLint location = GLES::gl->GetUniformLocation(m_RendererID, name.c_str());
    GLES::gl->Uniform1i(location, value);
}

void OpenGLES3Shader::UploadUniformIntArray(const std::string& name, int* values, uint32_t count)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Shader::UploadUniformIntArray");

    GLint location = GLES::gl->GetUniformLocation(m_RendererID, name.c_str());
    GLES::gl->Uniform1iv(location, count, values);
}

void OpenGLES3Shader::UploadUniformFloat(const std::string& name, float value)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Shader::UploadUniformFloat");

    GLint location = GLES::gl->GetUniformLocation(m_RendererID, name.c_str());
    GLES::gl->Uniform1f(location, value);
}

void OpenGLES3Shader::UploadUniformFloat2(const std::string& name, const glm::vec2& value)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Shader::UploadUniformFloat2");

    GLint location = GLES::gl->GetUniformLocation(m_RendererID, name.c_str());
    GLES::gl->Uniform2f(location, value.x, value.y);
}

void OpenGLES3Shader::UploadUniformFloat3(const std::string& name, const glm::vec3& value)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Shader::UploadUniformFloat3");

    GLint location = GLES::gl->GetUniformLocation(m_RendererID, name.c_str());
    GLES::gl->Uniform3f(location, value.x, value.y, value.z);
}

void OpenGLES3Shader::UploadUniformFloat4(const std::string& name, const glm::vec4& value)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Shader::UploadUniformFloat4");

    GLint location = GLES::gl->GetUniformLocation(m_RendererID, name.c_str());
    GLES::gl->Uniform4f(location, value.x, value.y, value.z, value.w);
}

void OpenGLES3Shader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Shader::UploadUniformMat3");

    GLint location = GLES::gl->GetUniformLocation(m_RendererID, name.c_str());
    GLES::gl->UniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void OpenGLES3Shader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3Shader::UploadUniformMat4");

    GLint location = GLES::gl->GetUniformLocation(m_RendererID, name.c_str());
    GLES::gl->UniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

}
