module crumb.render.vulkan;
import :swapchain;

import crumb.core;
import vulkan;
import std;

namespace Crumb::Render {

SwapChain::SwapChain(vk::raii::Device &device,
                     vk::raii::PhysicalDevice &physicalDevice,
                     vk::raii::SurfaceKHR &surface)
	: m_device(device)
	, m_physicalDevice(physicalDevice)
	, m_surface(surface)
{
}

// 选择表面格式：优先 sRGB
vk::SurfaceFormatKHR SwapChain::ChooseSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats)
{
	for (const auto &fmt : availableFormats)
	{
		if (fmt.format == vk::Format::eB8G8R8A8Srgb && fmt.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
		{
			return fmt;
		}
	}
	return availableFormats[0];
}

// 选择呈现模式：优先 Mailbox（三重缓冲），回退到 FIFO
vk::PresentModeKHR SwapChain::ChoosePresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes)
{
	for (const auto &mode : availablePresentModes)
	{
		if (mode == vk::PresentModeKHR::eMailbox)
		{
			return mode;
		}
	}
	return vk::PresentModeKHR::eFifo;
}

// 选择交换范围
vk::Extent2D SwapChain::ChooseExtent(const vk::SurfaceCapabilitiesKHR &capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<std::uint32_t>::max())
	{
		return capabilities.currentExtent;
	}

	vk::Extent2D actual = {
		capabilities.minImageExtent.width,
		capabilities.minImageExtent.height
	};
	return actual;
}

// 创建交换链
bool SwapChain::Create()
{
	try
	{
		auto caps = m_physicalDevice.getSurfaceCapabilitiesKHR(*m_surface);

		// 窗口最小化时 surface 尺寸为 0，跳过创建，等恢复后再 Recreate
		if (caps.currentExtent.width == 0 || caps.currentExtent.height == 0)
			return false;

		auto formats = m_physicalDevice.getSurfaceFormatsKHR(*m_surface);
		auto presentModes = m_physicalDevice.getSurfacePresentModesKHR(*m_surface);

		vk::SurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(formats);
		vk::PresentModeKHR presentMode = ChoosePresentMode(presentModes);
		vk::Extent2D extent = ChooseExtent(caps);

		std::uint32_t imageCount = caps.minImageCount + 1;
		if (caps.maxImageCount > 0 && imageCount > caps.maxImageCount)
		{
			imageCount = caps.maxImageCount;
		}

		auto queueFamilies = m_physicalDevice.getQueueFamilyProperties2();
		// 获取图形和呈现队列族索引
		// 注：这里简化处理，用独占模式
		vk::SwapchainCreateInfoKHR createInfo{};
		createInfo.setSurface(*m_surface)
			.setMinImageCount(imageCount)
			.setImageFormat(surfaceFormat.format)
			.setImageColorSpace(surfaceFormat.colorSpace)
			.setImageExtent(extent)
			.setImageArrayLayers(1)
			.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst)
			.setImageSharingMode(vk::SharingMode::eExclusive)
			.setPreTransform(caps.currentTransform)
			.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
			.setPresentMode(presentMode)
			.setClipped(vk::True)
			.setOldSwapchain(nullptr);

		m_swapChain = vk::raii::SwapchainKHR(m_device, createInfo);
		m_images = m_swapChain.getImages();
		m_imageLayouts.assign(m_images.size(), vk::ImageLayout::eUndefined);
		m_format = surfaceFormat.format;
		m_extent = extent;

		return true;
	}
	catch (const std::exception &e)
	{
		Log::Error("[VK] Failed to create swap chain: {}", e.what());
		return false;
	}
}

// 创建图像视图
bool SwapChain::CreateImageViews()
{
	try
	{
		m_imageViews.clear();
		m_imageViews.reserve(m_images.size());

		vk::ImageViewCreateInfo createInfo{};
		createInfo.setViewType(vk::ImageViewType::e2D)
			.setFormat(m_format)
			.setComponents({
				vk::ComponentSwizzle::eIdentity,
				vk::ComponentSwizzle::eIdentity,
				vk::ComponentSwizzle::eIdentity,
				vk::ComponentSwizzle::eIdentity
			})
			.setSubresourceRange({
				vk::ImageAspectFlagBits::eColor,
				0, 1, 0, 1
			});

		for (const auto &image : m_images)
		{
			createInfo.setImage(image);
			m_imageViews.emplace_back(m_device, createInfo);
		}

		return true;
	}
	catch (const std::exception &e)
	{
		Log::Error("[VK] Failed to create image views: {}", e.what());
		return false;
	}
}

// 清理交换链资源
void SwapChain::Cleanup()
{
	m_imageViews.clear();
	m_swapChain = vk::raii::SwapchainKHR(nullptr);
	m_images.clear();
	m_imageLayouts.clear();
}

// 重建交换链（窗口大小变化时调用）
bool SwapChain::Recreate()
{
	m_device.waitIdle();

	// 先创建新的，成功后再替换旧的（旧 swapchain 作为 oldSwapchain 传入）
	// 失败时旧 swapchain 不受影响（如窗口最小化 extent=0）
	auto caps = m_physicalDevice.getSurfaceCapabilitiesKHR(*m_surface);
	if (caps.currentExtent.width == 0 || caps.currentExtent.height == 0)
		return false;  // 窗口最小化，保留旧 swapchain

	auto formats = m_physicalDevice.getSurfaceFormatsKHR(*m_surface);
	auto presentModes = m_physicalDevice.getSurfacePresentModesKHR(*m_surface);

	vk::SurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(formats);
	vk::PresentModeKHR presentMode = ChoosePresentMode(presentModes);
	vk::Extent2D extent = ChooseExtent(caps);

	std::uint32_t imageCount = caps.minImageCount + 1;
	if (caps.maxImageCount > 0 && imageCount > caps.maxImageCount)
		imageCount = caps.maxImageCount;

	vk::SwapchainCreateInfoKHR createInfo{};
	createInfo.setSurface(*m_surface)
		.setMinImageCount(imageCount)
		.setImageFormat(surfaceFormat.format)
		.setImageColorSpace(surfaceFormat.colorSpace)
		.setImageExtent(extent)
		.setImageArrayLayers(1)
		.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst)
		.setImageSharingMode(vk::SharingMode::eExclusive)
		.setPreTransform(caps.currentTransform)
		.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
		.setPresentMode(presentMode)
		.setClipped(vk::True)
		.setOldSwapchain(*m_swapChain);

	try {
		auto newSwapChain = vk::raii::SwapchainKHR(m_device, createInfo);
		auto newImages = newSwapChain.getImages();
		std::vector<vk::ImageLayout> newLayouts(newImages.size(), vk::ImageLayout::eUndefined);

		// 新 swapchain 创建成功 → 替换旧的
		m_imageViews.clear();
		m_swapChain = std::move(newSwapChain);
		m_images = std::move(newImages);
		m_imageLayouts = std::move(newLayouts);
		m_format = surfaceFormat.format;
		m_extent = extent;

		return CreateImageViews();
	} catch (const std::exception& e) {
		Log::Error("[VK] Swapchain Recreate failed: {}", e.what());
		return false;  // 旧 swapchain 不受影响
	}
}

} // namespace Crumb::Render
