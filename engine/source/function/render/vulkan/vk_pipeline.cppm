export module crumb.render.vulkan:pipeline;

import :context;
import :swapchain;
import vulkan;

export namespace Crumb::Render {

class VKPipeline
{
  public:
	static constexpr std::uint32_t MaxTextures = 256;

	explicit VKPipeline(VKContext &context, SwapChain &swapChain);
    ~VKPipeline() = default;

	// 创建描述符集布局（UBO + 纹理采样器）
	bool CreateDescriptorSetLayout();

	// 创建图形管线（shader 模块由调用者加载）
	bool CreateGraphicsPipeline(vk::raii::ShaderModule &shaderModule);

	// 从 SPIR-V 字节码创建着色器模块
	vk::raii::ShaderModule CreateShaderModule(const std::vector<char> &code);

	// 获取描述符集布局
	vk::raii::DescriptorSetLayout &GetDescriptorSetLayout() {
		return m_descriptorSetLayout;
	}

	// 获取管线布局
	vk::raii::PipelineLayout &GetPipelineLayout() {
		return m_pipelineLayout;
	}

	// 获取图形管线
	vk::raii::Pipeline &GetGraphicsPipeline() {
		return m_graphicsPipeline;
	}

  private:
	// Vulkan 设备
	VKContext &m_context;

	// 交换链
	SwapChain &m_swapChain;

	// 图形管线
	vk::raii::PipelineLayout m_pipelineLayout = nullptr;
	vk::raii::Pipeline m_graphicsPipeline = nullptr;

	// 描述符集布局
	vk::raii::DescriptorSetLayout m_descriptorSetLayout = nullptr;
};

} // namespace Crumb::Render
