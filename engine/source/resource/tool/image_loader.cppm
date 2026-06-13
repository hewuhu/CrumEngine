export module crumb.resource:image_loader;

import std;

// ============================================================================
// ImageData — CPU 端图像数据
// 纯数据容器，不依赖任何图形 API
// ============================================================================

export namespace Crumb {

struct ImageData {
    std::vector<unsigned char> pixels;
    int width    = 0;
    int height   = 0;
    int channels = 0;  // 总是 4（STBI_rgb_alpha）

    explicit operator bool() const { return !pixels.empty(); }
};

/// @brief 从文件加载图像，自动解码为 RGBA8
ImageData LoadImageFromFile(const std::string& path);

} // namespace Crumb
