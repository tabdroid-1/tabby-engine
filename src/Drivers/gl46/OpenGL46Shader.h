#pragma once
#include <Tabby/Foundation/Types.h>
#include <Tabby/Renderer/Shader.h>

namespace Tabby {

class OpenGL46Shader : public Shader {
public:
    OpenGL46Shader(const std::string& filepath);
    OpenGL46Shader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
    virtual ~OpenGL46Shader();

    virtual void Bind() const override;
    virtual void Unbind() const override;

    virtual void SetBool(const std::string& name, bool value) override;
    virtual void SetInt(const std::string& name, int value) override;
    virtual void SetIntArray(const std::string& name, int* values, uint32_t count) override;
    virtual void SetFloatArray(const std::string& name, float* values, uint32_t count) override;
    virtual void SetFloat(const std::string& name, float value) override;
    virtual void SetFloat2(const std::string& name, const glm::vec2& value) override;
    virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;
    virtual void SetFloat4(const std::string& name, const glm::vec4& value) override;
    virtual void SetMat3(const std::string& name, const glm::mat3& value) override;
    virtual void SetMat4(const std::string& name, const Matrix4& value) override;

    virtual const std::string& GetName() const override { return m_Name; }

    void UploadUniformInt(const std::string& name, int value);
    void UploadUniformIntArray(const std::string& name, int* values, uint32_t count);

    void UploadUniformFloat(const std::string& name, float value);
    void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
    void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
    void UploadUniformFloat4(const std::string& name, const glm::vec4& value);

    void UploadUniformMat3(const std::string& name, const Matrix3& matrix);
    void UploadUniformMat4(const std::string& name, const Matrix4& matrix);

private:
    std::string ReadFile(const std::string& filepath);
    std::unordered_map<uint32_t, std::string> PreProcess(const std::string& source);
    void Compile(const std::unordered_map<uint32_t, std::string>& shaderSource, const std::string& path);

private:
    uint32_t m_RendererID;
    std::string m_FilePath;

    std::string m_Name;
};

}
