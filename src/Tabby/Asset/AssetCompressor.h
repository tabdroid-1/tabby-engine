#pragma once

#include <Tabby/Foundation/Types.h>

namespace Tabby {
// Generate mip levels. Discards last two levels with 1- or 2-pixel wide dimensions (because mips are further used for BC7 encoding)
class AssetCompressor {
public:
    static std::vector<RGBA32> GenerateMipMaps(const std::vector<RGBA32>& mip0_data, uint32_t image_width, uint32_t image_height);
};

}
