export module crumb.render.vulkan:swapchain;

import vulkan;
import std;

export namespace Crumb::Render {

class SwapChain
{
  public:
	SwapChain(vk::raii::Device &device,
	          vk::raii::PhysicalDevice &physicalDevice,
	          vk::raii::SurfaceKHR &surface);

	// 生命周期
	bool Create();
	bool CreateImageViews();
	void Cleanup();
	bool Recreate();

	// 访问器
	vk::raii::SwapchainKHR &GetSwapChain() { return m_swapChain; }
	const std::vector<vk::Image> &GetImages() const { return m_images; }
	const std::vector<vk::raii::ImageView> &GetImageViews() const { return m_imageViews; }
	const std::vector<vk::ImageLayout> &GetImageLayouts() const { return m_imageLayouts; }
	vk::Format GetImageFormat() const { return m_format; }
	const vk::Format *GetImageFormatPtr() const { return &m_format; }
	vk::Extent2D GetExtent() const { return m_extent; }
	std::uint32_t GetImageCount() const { return static_cast<std::uint32_t>(m_images.size()); }

  private:
	// 引用
	vk::raii::Device &m_device;
	vk::raii::PhysicalDevice &m_physicalDevice;
	vk::raii::SurfaceKHR &m_surface;

	// 交换链资源
	vk::raii::SwapchainKHR m_swapChain = nullptr;
	std::vector<vk::Image> m_images;
	vk::Format m_format = vk::Format::eUndefined;
	vk::Extent2D m_extent = {0, 0};
	std::vector<vk::raii::ImageView> m_imageViews;
	std::vector<vk::ImageLayout> m_imageLayouts;

	// 辅助函数
	vk::SurfaceFormatKHR ChooseSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats);
	vk::PresentModeKHR ChoosePresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes);
	vk::Extent2D ChooseExtent(const vk::SurfaceCapabilitiesKHR &capabilities);
};

} // namespace Crumb::Render
