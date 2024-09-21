#include "ShaderLibrary.h"

#include <Tabby/Core/Filesystem/Filesystem.h>
#include <Tabby/Threading/Task.h>
#include "ShaderCompiler.h"
#include "Renderer.h"

namespace Tabby {

ShaderLibrary::ShaderLibrary()
{
    m_GlobalMacros.shrink_to_fit();
}

ShaderLibrary::~ShaderLibrary()
{
    for (auto& [key, permutation_map] : m_Library)
        for (auto& [shader, macro_table] : permutation_map)
            shader->Destroy();

    m_Library.clear();
}

void ShaderLibrary::Init()
{
    if (!s_Instance) {
        s_Instance = new ShaderLibrary;
    }
}

void ShaderLibrary::Destroy()
{
    if (s_Instance) {
        delete s_Instance;
    }
}

bool ShaderLibrary::LoadShader(const ShaderSpecification& spec, const std::filesystem::path& path, const ShaderMacroTable& macros)
{
    ShaderCompiler shader_compiler;

    for (auto& global_macro : s_Instance->m_GlobalMacros)
        shader_compiler.AddGlobalMacro(global_macro.first, global_macro.second);

    bool result = HasShader(path.filename().string(), macros);

    if (result) {
        TB_CORE_WARN("Shader \"{0}\" is already loaded.", path.filename().string());
        return true;
    }

    result = FileSystem::CheckDirectory(path);

    if (!result) {
        TB_CORE_ERROR("Shader directory not found: {0}", path.string());
        return false;
    }

    std::string shader_source;
    std::string line;
    std::ifstream input_stream(path);
    while (std::getline(input_stream, line))
        shader_source.append(line + '\n');

    ShaderCompilationResult compilation_result = shader_compiler.Compile(shader_source, path.filename().string(), macros);

    if (!compilation_result.valid)
        return false;

    Shared<Shader> shader = Shader::Create(spec, compilation_result.bytecode);

    s_Instance->m_Mutex.lock();
    if (auto shader_filename = path.filename().string(); s_Instance->m_Library.find(shader_filename) != s_Instance->m_Library.end()) {
        s_Instance->m_Library[shader_filename].push_back({ shader, macros });
    } else {
        std::vector<std::pair<Shared<Shader>, ShaderMacroTable>> permutations;
        permutations.push_back({ shader, macros });
        s_Instance->m_Library.emplace(shader_filename, permutations);
    }
    s_Instance->m_Mutex.unlock();

    return true;
}

bool ShaderLibrary::UnloadShader(std::string name, const ShaderMacroTable& macros)
{
    if (s_Instance->m_Library.find(name) == s_Instance->m_Library.end())
        return false;

    auto& permutation_table = s_Instance->m_Library.at(name);

    // Delete specific permutation
    uint32_t idx = 0;
    bool permutation_found = false;
    for (auto& [shader, permutation] : permutation_table) {
        if (permutation == macros) {
            shader->Destroy();
            permutation_table.erase(permutation_table.begin() + idx);
            return true;
        }
        idx++;
    }

    // Shader found, but didn't find specific permutation
    if (idx == permutation_table.size())
        return false;

    // If there's no permutations of a specific shader left, delete entry from library.
    if (!s_Instance->m_Library.at(name).size()) {
        for (auto& shader : s_Instance->m_Library.at(name)) {
            shader.first->Destroy();
        }
        s_Instance->m_Library.erase(name);
    }

    return true;
}

bool ShaderLibrary::ReloadShader(std::filesystem::path name)
{
    return false;
}

bool ShaderLibrary::HasShader(std::string key, const ShaderMacroTable& macros /*= {}*/)
{
    if (s_Instance->m_Library.find(key) == s_Instance->m_Library.end())
        return false;

    auto& permutation_list = s_Instance->m_Library.at(key);

    for (auto& permutation : permutation_list) {
        if (permutation.second == macros)
            return true;
    }

    return false;
}

Shared<Shader> ShaderLibrary::GetShader(std::string key, const ShaderMacroTable& macros)
{
    if (s_Instance->m_Library.find(key) == s_Instance->m_Library.end())
        return nullptr;

    auto& permutation_list = s_Instance->m_Library.at(key);

    for (auto& permutation : permutation_list) {
        if (permutation.second == macros)
            return permutation.first;
    }

    return nullptr;
}

ShaderStage ShaderLibrary::EvaluateStage(std::filesystem::path file)
{
    if (file.extension().string() == ".vert")
        return ShaderStage::VERTEX;
    if (file.extension().string() == ".frag")
        return ShaderStage::FRAGMENT;
    if (file.extension().string() == ".comp")
        return ShaderStage::COMPUTE;

    return ShaderStage::UNKNOWN;
}

}
