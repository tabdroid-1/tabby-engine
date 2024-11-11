#pragma once

#include <Tabby/Foundation/Types.h>

#include <bgfx/bgfx.h>

namespace Tabby {
/*
 *	@brief provides tools to store all shaders in centralized manner. Also gives toolset to load / unload shader if necessary.
 */
class ShaderLibrary {
public:
    ShaderLibrary();
    ~ShaderLibrary();

    static void Init();
    static void Shutdown();

    static bool LoadShader(const std::string& name, const std::string& vertex_path, const std::string& fragment_path);
    static bgfx::ProgramHandle GetShader(const std::string& key);

    // @return true if shader was successfully unloaded from library and destroyed, false if no shader with such name was found
    static bool UnloadShader(const std::string& name);

    /*
     *  @brief attempts to find shader with matching name. Tries to find both with or without shader extension (e.g. .vs, .fs and so on)
     *  @return true if shader is present in library, otherwise returns false
     */
    static bool HasShader(const std::string& key);

    /*
     *  @brief Acquire shader from library. Read-only.
     *  @return a shared pointer to shader. If shader is not present, returns nullptr.
     */

private:
    inline static ShaderLibrary* s_Instance;
    std::unordered_map<std::string, bgfx::ProgramHandle> m_Library;
    //     InternalStorage m_Library;
    //     std::shared_mutex m_Mutex;
    //     std::vector<std::pair<std::string, std::string>> m_GlobalMacros;
};

}
