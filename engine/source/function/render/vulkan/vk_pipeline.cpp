module crumb.render.vulkan;
import :pipeline;

import crumb.core;
import vulkan;
import std;

namespace Crumb::Render {

VKPipeline::VKPipeline(VKContext &context, SwapChain &swapChain)
    : m_context(context)
    , m_swapChain(swapChain)
{
    Log::Info("[VKPipeline] constructor");
}

vk::raii::ShaderModule VKPipeline::CreateShaderModule(const std::vector<char> &code)
{
	Log::Info("[VKPipeline] CreateShaderModule ({} bytes)", code.size());
	vk::ShaderModuleCreateInfo createInfo{};
	createInfo.setCodeSize(code.size())
	          .setPCode(reinterpret_cast<const std::uint32_t *>(code.data()));
	return vk::raii::ShaderModule(m_context.GetDevice(), createInfo);
}

static vk::DescriptorSetLayoutBinding MakeBinding(std::uint32_t binding, vk::DescriptorType type, vk::ShaderStageFlags stages, std::uint32_t count = 1)
{
	vk::DescriptorSetLayoutBinding b{};
	b.setBinding(binding)
	 .setDescriptorType(type)
	 .setDescriptorCount(count)
	 .setStageFlags(stages);
	return b;
}

bool VKPipeline::CreateDescriptorSetLayout()
{
	Log::Info("[VKPipeline] CreateDescriptorSetLayout (SSBO + texture array)");
	try
	{
		std::array bindings = {
			// binding 0: SSBO — 所有精灵的数据
			MakeBinding(0, vk::DescriptorType::eStorageBuffer,
			            vk::ShaderStageFlagBits::eVertex),
			// binding 1: 纹理数组 — descriptor indexing
			MakeBinding(1, vk::DescriptorType::eCombinedImageSampler,
			            vk::ShaderStageFlagBits::eFragment, MaxTextures)
		};

		vk::DescriptorSetLayoutBindingFlagsCreateInfo bindingFlagsInfo{};
		std::array<vk::DescriptorBindingFlags, 2> bindingFlags{};
		if (m_context.IsDescriptorIndexingEnabled())
		{
			bindingFlags[0] = vk::DescriptorBindingFlagBits::eUpdateAfterBind
			                | vk::DescriptorBindingFlagBits::eUpdateUnusedWhilePending;
			bindingFlags[1] = vk::DescriptorBindingFlagBits::eUpdateAfterBind
			                | vk::DescriptorBindingFlagBits::eUpdateUnusedWhilePending
			                | vk::DescriptorBindingFlagBits::ePartiallyBound;
			bindingFlagsInfo.setBindingCount(static_cast<std::uint32_t>(bindingFlags.size()))
			                .setPBindingFlags(bindingFlags.data());
		}

		vk::DescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.setBindingCount(static_cast<std::uint32_t>(bindings.size()))
		          .setPBindings(bindings.data());
		if (m_context.IsDescriptorIndexingEnabled())
		{
			layoutInfo.setFlags(vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool)
			          .setPNext(&bindingFlagsInfo);
		}

		m_descriptorSetLayout = vk::raii::DescriptorSetLayout(m_context.GetDevice(), layoutInfo);
		return true;
	}
	catch (const std::exception &e)
	{
		Log::Error("[VK] Failed to create descriptor set layout: {}", e.what());
		return false;
	}
}

// 创建图形管线
bool VKPipeline::CreateGraphicsPipeline(vk::raii::ShaderModule &shaderModule)
{
	Log::Info("[VKPipeline] CreateGraphicsPipeline");
	try
	{

		// 着色器阶段
		vk::PipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.setStage(vk::ShaderStageFlagBits::eVertex)
		                   .setModule(*shaderModule)
		                   .setPName("VSMain");

		vk::PipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.setStage(vk::ShaderStageFlagBits::eFragment)
		                   .setModule(*shaderModule)
		                   .setPName("PSMain");

		vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

		// 顶点输入：position(vec2) + uv(vec2)，匹配 SpriteVertex
		vk::VertexInputBindingDescription vertexBinding{};
		vertexBinding.setBinding(0).setStride(sizeof(float) * 4)
		             .setInputRate(vk::VertexInputRate::eVertex);

		std::array<vk::VertexInputAttributeDescription, 2> vertexAttrs{};
		vertexAttrs[0].setBinding(0).setLocation(0)
		              .setFormat(vk::Format::eR32G32Sfloat).setOffset(0);
		vertexAttrs[1].setBinding(0).setLocation(1)
		              .setFormat(vk::Format::eR32G32Sfloat).setOffset(sizeof(float) * 2);

		vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.setVertexBindingDescriptionCount(1)
		               .setPVertexBindingDescriptions(&vertexBinding)
		               .setVertexAttributeDescriptionCount(static_cast<std::uint32_t>(vertexAttrs.size()))
		               .setPVertexAttributeDescriptions(vertexAttrs.data());

		// 输入装配
		vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.setTopology(vk::PrimitiveTopology::eTriangleList)
		             .setPrimitiveRestartEnable(vk::False);

		// 视口状态
		vk::PipelineViewportStateCreateInfo viewportState{};
		viewportState.setViewportCount(1)
		             .setScissorCount(1);

		// 光栅化
		vk::PipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.setDepthClampEnable(vk::False)
		          .setRasterizerDiscardEnable(vk::False)
		          .setPolygonMode(vk::PolygonMode::eFill)
		          .setCullMode(vk::CullModeFlagBits::eNone)
		          .setFrontFace(vk::FrontFace::eCounterClockwise)
		          .setDepthBiasEnable(vk::False)
		          .setLineWidth(1.0f);

		// 多重采样
		vk::PipelineMultisampleStateCreateInfo multisampling{};
		multisampling.setRasterizationSamples(vk::SampleCountFlagBits::e1)
		             .setSampleShadingEnable(vk::False);

		// 深度模板
		vk::PipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.setDepthTestEnable(vk::True)
		            .setDepthWriteEnable(vk::True)
		            .setDepthCompareOp(vk::CompareOp::eLessOrEqual)
		            .setDepthBoundsTestEnable(vk::False)
		            .setStencilTestEnable(vk::False);

		// 颜色混合
		vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.setBlendEnable(vk::True)
		                    .setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha)
		                    .setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha)
		                    .setColorBlendOp(vk::BlendOp::eAdd)
		                    .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
		                    .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
		                    .setAlphaBlendOp(vk::BlendOp::eAdd)
		                    .setColorWriteMask(
		                        vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
		                        vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);

		vk::PipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.setLogicOpEnable(vk::False)
		             .setLogicOp(vk::LogicOp::eCopy)
		             .setAttachmentCount(1)
		             .setPAttachments(&colorBlendAttachment);

		// 动态状态
		std::vector dynamicStates = {
			vk::DynamicState::eViewport,
			vk::DynamicState::eScissor
		};

		vk::PipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.setDynamicStateCount(static_cast<std::uint32_t>(dynamicStates.size()))
		            .setPDynamicStates(dynamicStates.data());

		// 管线布局
		vk::DescriptorSetLayout setLayout = *m_descriptorSetLayout;
		vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.setSetLayoutCount(1)
		                  .setPSetLayouts(&setLayout);

		m_pipelineLayout = vk::raii::PipelineLayout(m_context.GetDevice(), pipelineLayoutInfo);

		// 管线渲染创建信息（2D 无需深度）
		vk::PipelineRenderingCreateInfo renderingCreateInfo{};
		renderingCreateInfo.setColorAttachmentCount(1)
		                   .setPColorAttachmentFormats(m_swapChain.GetImageFormatPtr())
		                   .setDepthAttachmentFormat(vk::Format::eUndefined)
		                   .setStencilAttachmentFormat(vk::Format::eUndefined);

		// 关闭背面剔除
		vk::PipelineRasterizationStateCreateInfo rasterizerBack = rasterizer;
		rasterizerBack.setCullMode(vk::CullModeFlagBits::eNone);

		vk::GraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.setPNext(&renderingCreateInfo)
		            .setStageCount(2)
		            .setPStages(shaderStages)
		            .setPVertexInputState(&vertexInputInfo)
		            .setPInputAssemblyState(&inputAssembly)
		            .setPViewportState(&viewportState)
		            .setPRasterizationState(&rasterizerBack)
		            .setPMultisampleState(&multisampling)
		            .setPDepthStencilState(&depthStencil)
		            .setPColorBlendState(&colorBlending)
		            .setPDynamicState(&dynamicState)
		            .setLayout(*m_pipelineLayout);

		m_graphicsPipeline = vk::raii::Pipeline(m_context.GetDevice(), nullptr, pipelineInfo);
		return true;
	}
	catch (const std::exception &e)
	{
		Log::Error("[VK] Failed to create graphics pipeline: {}", e.what());
		return false;
	}
}

} // namespace Crumb::Render
