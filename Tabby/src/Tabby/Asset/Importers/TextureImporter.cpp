// #include <Tabby/Asset/Importers/TextureImporter.h>
//
// #include "SDL2/include/SDL_surface.h"
// #include <stb_image.h>
//
// namespace Tabby {
//
// // Shared<Texture> TextureImporter::ImportTexture2D(AssetHandle handle, const AssetMetadata& metadata)
// // {
// //
// // }
//
// Shared<Texture> TextureImporter::LoadTexture(const std::filesystem::path& path)
// {
//     TB_PROFILE_SCOPE();
//
//     int width, height, channels;
//     stbi_set_flip_vertically_on_load(1);
//     Buffer data;
//
//     {
//         TB_PROFILE_SCOPE_NAME("stbi_load - TextureImporter::ImportTexture");
//
// #if defined(TB_PLATFORM_ANDROID)
//         std::vector<unsigned char> imageData;
//
//         SDL_RWops* rw = SDL_RWFromFile(path.c_str(), "rb");
//         if (rw != nullptr) {
//             Sint64 size = SDL_RWsize(rw);
//
//             if (size > 0) {
//                 imageData.resize(size);
//                 Sint64 bytesRead = SDL_RWread(rw, imageData.data(), 1, size);
//                 if (bytesRead != size) {
//                     // Handle read error
//                     TB_CORE_ERROR("Error reading file {0}", path);
//                     imageData.clear(); // Clear the imageData to indicate an error
//                 }
//             }
//
//             SDL_RWclose(rw);
//         } else {
//             // Handle file open error
//             TB_CORE_ERROR("Could not open file {0}", path);
//             TB_CORE_INFO("Current working dir: {0}", std::filesystem::current_path());
//         }
//
//         data.Data = stbi_load_from_memory(imageData.data(), static_cast<int>(imageData.size()), &width, &height, &channels, 4);
//
// #else
//         std::string pathStr = path.string();
//         data.Data = stbi_load(pathStr.c_str(), &width, &height, &channels, 4);
// #endif
//         channels = 4;
//     }
//
//     if (data.Data == nullptr) {
//         TB_CORE_ERROR("TextureImporter::ImportTexture - Could not load texture from filepath: {}", path.string());
//         return nullptr;
//     }
//
//     // TODO: think about this
//     data.Size = width * height * channels;
//
//     TextureSpecification spec;
//     spec.Width = width;
//     spec.Height = height;
//     switch (channels) {
//     case 3:
//         spec.Format = ImageFormat::RGB8;
//         break;
//     case 4:
//         spec.Format = ImageFormat::RGBA8;
//         break;
//     }
//
//     Shared<Texture> texture = Texture::Create(spec, data);
//     data.Release();
//     return texture;
// }
//
// }
