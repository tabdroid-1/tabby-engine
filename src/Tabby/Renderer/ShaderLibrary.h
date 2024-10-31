#pragma once

#include <Tabby/Foundation/Types.h>
#include "Shader.h"

namespace Tabby {
/*
 *	@brief provides tools to store all shaders in centralized manner. Also gives toolset to load / unload shader if necessary.
 */
class ShaderLibrary {
public:
    ShaderLibrary();
    ~ShaderLibrary();

    static void Init();
    static void Destroy();
    using InternalStorage = std::map<std::string, std::vector<std::pair<Shared<Shader>, ShaderMacroTable>>>;

    // @return true if loaded successfully, false if not. It can happen due to incorrect shader path or invalid shader code
    static bool LoadShader(const std::filesystem::path& path, const ShaderMacroTable& macros = {});

    // @return true if shader was successfully unloaded from library and destroyed, false if no shader with such name was found
    static bool UnloadShader(std::string name, const ShaderMacroTable& macros = {});

    // @return true if shader was successfully found and recompiled, false if shader not found / contains errors in its code
    static bool ReloadShader(std::filesystem::path name);

    /*
     *  @brief attempts to find shader with matching name. Tries to find both with or without shader extension (e.g. .vs, .fs and so on)
     *  @return true if shader is present in library, otherwise returns false
     */
    static bool HasShader(std::string key, const ShaderMacroTable& macros = {});

    /*
     *  @brief Acquire shader from library. Read-only.
     *  @return a shared pointer to shader. If shader is not present, returns nullptr.
     */
    static Shared<Shader> GetShader(std::string key, const ShaderMacroTable& macros = {});

    /*
     *  @return a whole shader library. Can be used to iterate through, e.g. for using with ImGui to list all shaders
     */
    static const InternalStorage* const GetShaders() { return &s_Instance->m_Library; }

    static ShaderStage EvaluateStage(std::filesystem::path file);

private:
    inline static ShaderLibrary* s_Instance;
    InternalStorage m_Library;
    std::shared_mutex m_Mutex;
    std::vector<std::pair<std::string, std::string>> m_GlobalMacros;
};

}
