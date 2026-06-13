// ============================================================================
// 其他模块通过 import crumb.resource:image_loader 获取 ImageData
// ============================================================================

module;
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

module crumb.resource;
import :image_loader;

import std;

namespace Crumb {

ImageData LoadImageFromFile(const std::string& path) {
    ImageData result;
    int w = 0, h = 0, c = 0;
    stbi_uc* pixels = stbi_load(path.c_str(), &w, &h, &c, STBI_rgb_alpha);
    if (!pixels) return result;

    result.width    = w;
    result.height   = h;
    result.channels = 4;  // STBI_rgb_alpha
    result.pixels.assign(pixels, pixels + static_cast<std::size_t>(w) * h * 4);
    stbi_image_free(pixels);
    return result;
}

} // namespace Crumb
