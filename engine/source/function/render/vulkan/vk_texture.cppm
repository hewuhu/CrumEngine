export module crumb.render.vulkan:texture;

import :context;
import vk_mem_alloc;
import vulkan;
import std;

export namespace Crumb::Render {

struct VKTexture
{
	vma::raii::Image image = nullptr;
	vk::raii::ImageView view = nullptr;
	vk::raii::Sampler sampler = nullptr;
	std::uint32_t width = 0;
	std::uint32_t height = 0;
};

// 从文件加载 2D 纹理
bool LoadTexture2D(VKContext &ctx, const std::string &path, VKTexture &out);

// 从内存 RGBA8 像素创建 2D 纹理
bool CreateTextureRGBA8(VKContext &ctx, std::uint32_t width, std::uint32_t height,
                        const void *pixels, VKTexture &out);
} // namespace Crumb::Render
