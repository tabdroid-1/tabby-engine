#include <Tabby/Renderer/ShaderLibrary.h>

#include <SDL_rwops.h>

namespace Tabby {
ShaderLibrary::ShaderLibrary()
{
}

ShaderLibrary::~ShaderLibrary()
{
}

void ShaderLibrary::Init()
{
    if (!s_Instance) {
        s_Instance = new ShaderLibrary();
    }
}

void ShaderLibrary::Shutdown()
{
    if (!s_Instance)
        return;

    for (auto programs : s_Instance->m_Library) {
        bgfx::destroy(programs.second);
    };

    delete s_Instance;
}

bool ShaderLibrary::LoadShader(const std::string& name, const std::string& vertex_path, const std::string& fragment_path)
{
    if (s_Instance->m_Library.find(name) != s_Instance->m_Library.end()) {

        TB_ERROR("Shader '{}' already exists.", name);
        return false;
    }

    bgfx::ShaderHandle vs;
    bgfx::ShaderHandle fs;
    {
        void* vertex_data;
        SDL_RWops* rw = SDL_RWFromFile(vertex_path.c_str(), "rb");
        Sint64 vertex_size;
        if (rw != nullptr) {
            vertex_size = SDL_RWsize(rw);

            if (vertex_size > 0) {
                vertex_data = malloc(vertex_size);
                Sint64 bytesRead = SDL_RWread(rw, vertex_data, 1, vertex_size);
                if (bytesRead != vertex_size) {
                    TB_CORE_ERROR("Error reading file {0}", vertex_path);
                    free(vertex_data);
                    return false;
                }
            }

            SDL_RWclose(rw);
        } else {
            TB_CORE_ERROR("Could not open file {0}", vertex_path);
            return false;
        }

        void* fragment_data;
        rw = SDL_RWFromFile(fragment_path.c_str(), "rb");
        Sint64 fragment_size;
        if (rw != nullptr) {
            fragment_size = SDL_RWsize(rw);

            if (fragment_size > 0) {
                fragment_data = malloc(fragment_size);
                Sint64 bytesRead = SDL_RWread(rw, fragment_data, 1, fragment_size);
                if (bytesRead != fragment_size) {
                    TB_CORE_ERROR("Error reading file {0}", fragment_path);
                    free(fragment_data);
                    return false;
                }
            }

            SDL_RWclose(rw);
        } else {
            TB_CORE_ERROR("Could not open file {0}", fragment_path);
            return false;
        }

        fs = bgfx::createShader(bgfx::makeRef(fragment_data, fragment_size));
        vs = bgfx::createShader(bgfx::makeRef(vertex_data, vertex_size));

        // vs = bgfx::createShader(bgfx::makeRef(vertex_source.data(), sizeof(vertex_source.data())));
        // fs = bgfx::createShader(bgfx::makeRef(fragment_source.data(), sizeof(fragment_source.data())));
    }

    auto& program = s_Instance->m_Library[name];
    program = bgfx::createProgram(vs, fs, true);

    return true;
}

bgfx::ProgramHandle ShaderLibrary::GetShader(const std::string& key)
{
    if (s_Instance->m_Library.find(key) != s_Instance->m_Library.end()) {
        return s_Instance->m_Library[key];
    }

    return BGFX_INVALID_HANDLE;
}

bool ShaderLibrary::UnloadShader(const std::string& name)
{
    if (s_Instance->m_Library.find(name) != s_Instance->m_Library.end()) {
        s_Instance->m_Library.erase(name);
        return true;
    }

    return false;
}

bool ShaderLibrary::HasShader(const std::string& key)
{
    if (s_Instance->m_Library.find(key) != s_Instance->m_Library.end()) {
        return true;
    }

    return false;
}

}
