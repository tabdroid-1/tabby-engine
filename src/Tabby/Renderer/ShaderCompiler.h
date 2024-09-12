#pragma once

#include <Tabby/Foundation/Types.h>

#include <shaderc/shaderc.hpp>

namespace Tabby {

enum class ShaderStage : uint8_t;

struct ShaderCompilationResult {
    std::map<ShaderStage, std::vector<uint32_t>> bytecode;
    bool valid;
};

class ShaderCompiler {
public:
    ShaderCompiler();

    /*
     *  @brief reads shader file.
     *  @return true if successful, otherwise false (e.g. file not present)
     *  @param [out] std::stringstream* out - a pointer to stringstream to write source code to.
     */
    bool ReadShaderFile(std::filesystem::path path, std::stringstream* out);
    void AddGlobalMacro(const std::string& key, const std::string& value);

    ShaderCompilationResult Compile(std::string& source, const std::string& filename, const ShaderMacroTable& macros = {});

private:
    shaderc::Compiler m_Compiler;
    shaderc::CompileOptions m_GlobalOptions;
    std::map<std::string, std::string> m_GlobalMacros;
};

class ShaderSourceIncluder : public shaderc::CompileOptions::IncluderInterface {
public:
    shaderc_include_result* GetInclude(const char* requested_source, shaderc_include_type type, const char* requesting_source, size_t include_depth) override;
    void ReleaseInclude(shaderc_include_result* data) override;
};

}
