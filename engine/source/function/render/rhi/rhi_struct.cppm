export module crumb.render:rhi_struct;

import :rhi_types;
import std;

// ============================================================================
// RHI Struct — 句柄基类 + CreateInfo 结构体 + 运行时结构体
// 不依赖任何 Vulkan 头文件
// 注意：省略 sType/pNext（Vulkan 扩展链概念，不应在抽象层暴露）
// ============================================================================

export namespace Crumb::Render {

// ============================================================================
// RHI 句柄基类（空壳标记类型）
// ============================================================================
class RHIBuffer {};
class RHIBufferView {};
class RHICommandBuffer {};
class RHICommandPool {};
class RHIDescriptorPool {};
class RHIDescriptorSet {};
class RHIDescriptorSetLayout {};
class RHIDeviceMemory {};
class RHIEvent {};
class RHIFence {};
class RHIFramebuffer {};
class RHIImage {};
class RHIImageView {};
class RHIInstance {};
class RHIPhysicalDevice {};
class RHIPipeline {};
class RHIPipelineCache {};
class RHIPipelineLayout {};
class RHIQueue {};
class RHIRenderPass {};
class RHISampler {};
class RHISemaphore {};
class RHIShader {};

// ============================================================================
// 基础几何结构体
// ============================================================================
struct RHIOffset2D {
    std::int32_t x = 0;
    std::int32_t y = 0;
};

struct RHIOffset3D {
    std::int32_t x = 0;
    std::int32_t y = 0;
    std::int32_t z = 0;
};

struct RHIExtent2D {
    std::uint32_t width = 0;
    std::uint32_t height = 0;
};

struct RHIExtent3D {
    std::uint32_t width = 0;
    std::uint32_t height = 0;
    std::uint32_t depth = 0;
};

struct RHIRect2D {
    RHIOffset2D offset;
    RHIExtent2D extent;
};

struct RHIViewport {
    float x = 0.0f;
    float y = 0.0f;
    float width = 0.0f;
    float height = 0.0f;
    float minDepth = 0.0f;
    float maxDepth = 1.0f;
};

// ============================================================================
// 内存/同步 基础结构体
// ============================================================================
struct RHIMemoryBarrier {
    RHIAccessFlags srcAccessMask = 0;
    RHIAccessFlags dstAccessMask = 0;
};

struct RHIBufferMemoryBarrier {
    RHIAccessFlags srcAccessMask = 0;
    RHIAccessFlags dstAccessMask = 0;
    std::uint32_t srcQueueFamilyIndex = RHI_QUEUE_FAMILY_IGNORED;
    std::uint32_t dstQueueFamilyIndex = RHI_QUEUE_FAMILY_IGNORED;
    RHIBuffer* buffer = nullptr;
    RHIDeviceSize offset = 0;
    RHIDeviceSize size = RHI_WHOLE_SIZE;
};

struct RHIImageSubresourceRange {
    RHIImageAspectFlags aspectMask = 0;
    std::uint32_t baseMipLevel = 0;
    std::uint32_t levelCount = 0;
    std::uint32_t baseArrayLayer = 0;
    std::uint32_t layerCount = 0;
};

struct RHIImageMemoryBarrier {
    RHIAccessFlags srcAccessMask = 0;
    RHIAccessFlags dstAccessMask = 0;
    RHIImageLayout oldLayout = RHI_IMAGE_LAYOUT_UNDEFINED;
    RHIImageLayout newLayout = RHI_IMAGE_LAYOUT_UNDEFINED;
    std::uint32_t srcQueueFamilyIndex = RHI_QUEUE_FAMILY_IGNORED;
    std::uint32_t dstQueueFamilyIndex = RHI_QUEUE_FAMILY_IGNORED;
    RHIImage* image = nullptr;
    RHIImageSubresourceRange subresourceRange;
};

struct RHIBufferCopy {
    RHIDeviceSize srcOffset = 0;
    RHIDeviceSize dstOffset = 0;
    RHIDeviceSize size = 0;
};

struct RHIImageSubresourceLayers {
    RHIImageAspectFlags aspectMask = 0;
    std::uint32_t mipLevel = 0;
    std::uint32_t baseArrayLayer = 0;
    std::uint32_t layerCount = 1;
};

struct RHIBufferImageCopy {
    RHIDeviceSize bufferOffset = 0;
    std::uint32_t bufferRowLength = 0;
    std::uint32_t bufferImageHeight = 0;
    RHIImageSubresourceLayers imageSubresource;
    RHIOffset3D imageOffset;
    RHIExtent3D imageExtent;
};

struct RHIClearColorValue {
    union {
        float float32[4];
        std::int32_t int32[4];
        std::uint32_t uint32[4];
    };
};

struct RHIClearDepthStencilValue {
    float depth = 1.0f;
    std::uint32_t stencil = 0;
};

union RHIClearValue {
    RHIClearColorValue color;
    RHIClearDepthStencilValue depthStencil;
};

struct RHIClearAttachment {
    RHIImageAspectFlags aspectMask = 0;
    std::uint32_t colorAttachment = 0;
    RHIClearValue clearValue;
};

struct RHIClearRect {
    RHIRect2D rect;
    std::uint32_t baseArrayLayer = 0;
    std::uint32_t layerCount = 1;
};

// ============================================================================
// 组件映射
// ============================================================================
struct RHIComponentMapping {
    RHIComponentSwizzle r = RHI_COMPONENT_SWIZZLE_IDENTITY;
    RHIComponentSwizzle g = RHI_COMPONENT_SWIZZLE_IDENTITY;
    RHIComponentSwizzle b = RHI_COMPONENT_SWIZZLE_IDENTITY;
    RHIComponentSwizzle a = RHI_COMPONENT_SWIZZLE_IDENTITY;
};

// ============================================================================
// CreateInfo 结构体
// ============================================================================

// --- Buffer ---
struct RHIBufferCreateInfo {
    RHIBufferCreateFlags flags = 0;
    RHIDeviceSize size = 0;
    RHIBufferUsageFlags usage = 0;
    RHISharingMode sharingMode = RHI_SHARING_MODE_EXCLUSIVE;
    std::vector<std::uint32_t> queueFamilyIndices;
};

// --- Image ---
struct RHIImageCreateInfo {
    RHIImageCreateFlags flags = 0;
    RHIImageType imageType = RHI_IMAGE_TYPE_2D;
    RHIFormat format = RHI_FORMAT_UNDEFINED;
    RHIExtent3D extent = {0, 0, 1};
    std::uint32_t mipLevels = 1;
    std::uint32_t arrayLayers = 1;
    RHISampleCountFlags samples = RHI_SAMPLE_COUNT_1_BIT;
    RHIImageTiling tiling = RHI_IMAGE_TILING_OPTIMAL;
    RHIImageUsageFlags usage = 0;
    RHISharingMode sharingMode = RHI_SHARING_MODE_EXCLUSIVE;
    std::vector<std::uint32_t> queueFamilyIndices;
    RHIImageLayout initialLayout = RHI_IMAGE_LAYOUT_UNDEFINED;
};

// --- ImageView ---
struct RHIImageViewCreateInfo {
    RHIImageViewCreateFlags flags = 0;
    RHIImage* image = nullptr;
    RHIImageViewType viewType = RHI_IMAGE_VIEW_TYPE_2D;
    RHIFormat format = RHI_FORMAT_UNDEFINED;
    RHIComponentMapping components;
    RHIImageSubresourceRange subresourceRange;
};

// --- Sampler ---
struct RHISamplerCreateInfo {
    RHISamplerCreateFlags flags = 0;
    RHIFilter magFilter = RHI_FILTER_LINEAR;
    RHIFilter minFilter = RHI_FILTER_LINEAR;
    RHISamplerMipmapMode mipmapMode = RHI_SAMPLER_MIPMAP_MODE_LINEAR;
    RHISamplerAddressMode addressModeU = RHI_SAMPLER_ADDRESS_MODE_REPEAT;
    RHISamplerAddressMode addressModeV = RHI_SAMPLER_ADDRESS_MODE_REPEAT;
    RHISamplerAddressMode addressModeW = RHI_SAMPLER_ADDRESS_MODE_REPEAT;
    float mipLodBias = 0.0f;
    RHIBool32 anisotropyEnable = 0;
    float maxAnisotropy = 0.0f;
    RHIBool32 compareEnable = 0;
    RHICompareOp compareOp = RHI_COMPARE_OP_ALWAYS;
    float minLod = 0.0f;
    float maxLod = 0.0f;
    RHIBorderColor borderColor = RHI_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
    RHIBool32 unnormalizedCoordinates = 0;
};

// --- Shader ---
struct RHIShaderModuleCreateInfo {
    RHIShaderModuleCreateFlags flags = 0;
    std::vector<std::uint32_t> code;
};

// --- Descriptor ---
struct RHIDescriptorSetLayoutBinding {
    std::uint32_t binding = 0;
    RHIDescriptorType descriptorType = RHI_DESCRIPTOR_TYPE_SAMPLER;
    std::uint32_t descriptorCount = 1;
    RHIShaderStageFlags stageFlags = 0;
};

struct RHIDescriptorSetLayoutCreateInfo {
    RHIDescriptorSetLayoutCreateFlags flags = 0;
    std::vector<RHIDescriptorSetLayoutBinding> bindings;
};

struct RHIDescriptorPoolSize {
    RHIDescriptorType type = RHI_DESCRIPTOR_TYPE_SAMPLER;
    std::uint32_t descriptorCount = 0;
};

struct RHIDescriptorPoolCreateInfo {
    RHIDescriptorPoolCreateFlags flags = 0;
    std::uint32_t maxSets = 0;
    std::vector<RHIDescriptorPoolSize> poolSizes;
};

struct RHIDescriptorSetAllocateInfo {
    RHIDescriptorPool* descriptorPool = nullptr;
    std::vector<RHIDescriptorSetLayout*> setLayouts;
};

struct RHIDescriptorImageInfo {
    RHISampler* sampler = nullptr;
    RHIImageView* imageView = nullptr;
    RHIImageLayout imageLayout = RHI_IMAGE_LAYOUT_UNDEFINED;
};

struct RHIDescriptorBufferInfo {
    RHIBuffer* buffer = nullptr;
    RHIDeviceSize offset = 0;
    RHIDeviceSize range = RHI_WHOLE_SIZE;
};

struct RHIWriteDescriptorSet {
    RHIDescriptorSet* dstSet = nullptr;
    std::uint32_t dstBinding = 0;
    std::uint32_t dstArrayElement = 0;
    std::uint32_t descriptorCount = 1;
    RHIDescriptorType descriptorType = RHI_DESCRIPTOR_TYPE_SAMPLER;
    RHIDescriptorImageInfo* pImageInfo = nullptr;
    RHIDescriptorBufferInfo* pBufferInfo = nullptr;
    RHIBufferView* pTexelBufferView = nullptr;
};

struct RHICopyDescriptorSet {
    RHIDescriptorSet* srcSet = nullptr;
    std::uint32_t srcBinding = 0;
    std::uint32_t srcArrayElement = 0;
    RHIDescriptorSet* dstSet = nullptr;
    std::uint32_t dstBinding = 0;
    std::uint32_t dstArrayElement = 0;
    std::uint32_t descriptorCount = 1;
};

// --- Command ---
struct RHICommandPoolCreateInfo {
    RHICommandPoolCreateFlags flags = 0;
    std::uint32_t queueFamilyIndex = 0;
};

struct RHICommandBufferAllocateInfo {
    RHICommandPool* commandPool = nullptr;
    RHICommandBufferLevel level = RHI_COMMAND_BUFFER_LEVEL_PRIMARY;
    std::uint32_t commandBufferCount = 1;
};

struct RHICommandBufferInheritanceInfo {
    RHIRenderPass* renderPass = nullptr;
    std::uint32_t subpass = 0;
    RHIFramebuffer* framebuffer = nullptr;
};

struct RHICommandBufferBeginInfo {
    RHICommandBufferUsageFlags flags = 0;
    RHICommandBufferInheritanceInfo* pInheritanceInfo = nullptr;
};

// --- Sync ---
struct RHIFenceCreateInfo {
    RHIFenceCreateFlags flags = 0;
};

struct RHISemaphoreCreateInfo {
    RHISemaphoreCreateFlags flags = 0;
};

struct RHISubmitInfo {
    std::vector<RHISemaphore*> waitSemaphores;
    std::vector<RHIPipelineStageFlags> waitDstStageMask;
    std::vector<RHICommandBuffer*> commandBuffers;
    std::vector<RHISemaphore*> signalSemaphores;
};

// --- RenderPass (保留用于兼容) ---
struct RHIAttachmentDescription {
    RHIAttachmentDescriptionFlags flags = 0;
    RHIFormat format = RHI_FORMAT_UNDEFINED;
    RHISampleCountFlags samples = RHI_SAMPLE_COUNT_1_BIT;
    RHIAttachmentLoadOp loadOp = RHI_ATTACHMENT_LOAD_OP_DONT_CARE;
    RHIAttachmentStoreOp storeOp = RHI_ATTACHMENT_STORE_OP_STORE;
    RHIAttachmentLoadOp stencilLoadOp = RHI_ATTACHMENT_LOAD_OP_DONT_CARE;
    RHIAttachmentStoreOp stencilStoreOp = RHI_ATTACHMENT_STORE_OP_DONT_CARE;
    RHIImageLayout initialLayout = RHI_IMAGE_LAYOUT_UNDEFINED;
    RHIImageLayout finalLayout = RHI_IMAGE_LAYOUT_UNDEFINED;
};

struct RHIAttachmentReference {
    std::uint32_t attachment = 0;
    RHIImageLayout layout = RHI_IMAGE_LAYOUT_UNDEFINED;
};

struct RHISubpassDescription {
    RHISubpassDescriptionFlags flags = 0;
    RHIPipelineBindPoint pipelineBindPoint = RHI_PIPELINE_BIND_POINT_GRAPHICS;
    std::vector<RHIAttachmentReference> inputAttachments;
    std::vector<RHIAttachmentReference> colorAttachments;
    std::vector<RHIAttachmentReference> resolveAttachments;
    RHIAttachmentReference* pDepthStencilAttachment = nullptr;
    std::vector<std::uint32_t> preserveAttachments;
};

struct RHISubpassDependency {
    std::uint32_t srcSubpass = 0;
    std::uint32_t dstSubpass = 0;
    RHIPipelineStageFlags srcStageMask = 0;
    RHIPipelineStageFlags dstStageMask = 0;
    RHIAccessFlags srcAccessMask = 0;
    RHIAccessFlags dstAccessMask = 0;
    RHIDependencyFlags dependencyFlags = 0;
};

struct RHIRenderPassCreateInfo {
    RHIRenderPassCreateFlags flags = 0;
    std::vector<RHIAttachmentDescription> attachments;
    std::vector<RHISubpassDescription> subpasses;
    std::vector<RHISubpassDependency> dependencies;
};

struct RHIRenderPassBeginInfo {
    RHIRenderPass* renderPass = nullptr;
    RHIFramebuffer* framebuffer = nullptr;
    RHIRect2D renderArea;
    std::vector<RHIClearValue> clearValues;
};

// --- Framebuffer ---
struct RHIFramebufferCreateInfo {
    RHIFramebufferCreateFlags flags = 0;
    RHIRenderPass* renderPass = nullptr;
    std::vector<RHIImageView*> attachments;
    std::uint32_t width = 0;
    std::uint32_t height = 0;
    std::uint32_t layers = 1;
};

// --- Pipeline ---
struct RHISpecializationMapEntry {
    std::uint32_t constantID = 0;
    std::uint32_t offset = 0;
    std::size_t size = 0;
};

struct RHISpecializationInfo {
    std::vector<RHISpecializationMapEntry> mapEntries;
    std::vector<std::uint8_t> data;
};

struct RHIPipelineShaderStageCreateInfo {
    RHIPipelineShaderStageCreateFlags flags = 0;
    RHIShaderStageFlagBits stage = RHI_SHADER_STAGE_VERTEX_BIT;
    RHIShader* module = nullptr;
    std::string pName = "main";
    RHISpecializationInfo* pSpecializationInfo = nullptr;
};

struct RHIVertexInputBindingDescription {
    std::uint32_t binding = 0;
    std::uint32_t stride = 0;
    RHIVertexInputRate inputRate = RHI_VERTEX_INPUT_RATE_VERTEX;
};

struct RHIVertexInputAttributeDescription {
    std::uint32_t location = 0;
    std::uint32_t binding = 0;
    RHIFormat format = RHI_FORMAT_UNDEFINED;
    std::uint32_t offset = 0;
};

struct RHIPipelineVertexInputStateCreateInfo {
    RHIPipelineVertexInputStateCreateFlags flags = 0;
    std::vector<RHIVertexInputBindingDescription> vertexBindingDescriptions;
    std::vector<RHIVertexInputAttributeDescription> vertexAttributeDescriptions;
};

struct RHIPipelineInputAssemblyStateCreateInfo {
    RHIPipelineInputAssemblyStateCreateFlags flags = 0;
    RHIPrimitiveTopology topology = RHI_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    RHIBool32 primitiveRestartEnable = 0;
};

struct RHIPipelineTessellationStateCreateInfo {
    RHIPipelineTessellationStateCreateFlags flags = 0;
    std::uint32_t patchControlPoints = 0;
};

struct RHIPipelineViewportStateCreateInfo {
    RHIPipelineViewportStateCreateFlags flags = 0;
    std::vector<RHIViewport> viewports;
    std::vector<RHIRect2D> scissors;
};

struct RHIPipelineRasterizationStateCreateInfo {
    RHIPipelineRasterizationStateCreateFlags flags = 0;
    RHIBool32 depthClampEnable = 0;
    RHIBool32 rasterizerDiscardEnable = 0;
    RHIPolygonMode polygonMode = RHI_POLYGON_MODE_FILL;
    RHICullModeFlags cullMode = RHI_CULL_MODE_BACK_BIT;
    RHIFrontFace frontFace = RHI_FRONT_FACE_CLOCKWISE;
    RHIBool32 depthBiasEnable = 0;
    float depthBiasConstantFactor = 0.0f;
    float depthBiasClamp = 0.0f;
    float depthBiasSlopeFactor = 0.0f;
    float lineWidth = 1.0f;
};

struct RHIPipelineMultisampleStateCreateInfo {
    RHIPipelineMultisampleStateCreateFlags flags = 0;
    RHISampleCountFlags rasterizationSamples = RHI_SAMPLE_COUNT_1_BIT;
    RHIBool32 sampleShadingEnable = 0;
    float minSampleShading = 1.0f;
    RHIBool32 alphaToCoverageEnable = 0;
    RHIBool32 alphaToOneEnable = 0;
};

struct RHIStencilOpState {
    RHIStencilOp failOp = RHI_STENCIL_OP_KEEP;
    RHIStencilOp passOp = RHI_STENCIL_OP_KEEP;
    RHIStencilOp depthFailOp = RHI_STENCIL_OP_KEEP;
    RHICompareOp compareOp = RHI_COMPARE_OP_ALWAYS;
    std::uint32_t compareMask = 0xFF;
    std::uint32_t writeMask = 0xFF;
    std::uint32_t reference = 0;
};

struct RHIPipelineDepthStencilStateCreateInfo {
    RHIPipelineDepthStencilStateCreateFlags flags = 0;
    RHIBool32 depthTestEnable = 0;
    RHIBool32 depthWriteEnable = 0;
    RHICompareOp depthCompareOp = RHI_COMPARE_OP_LESS;
    RHIBool32 depthBoundsTestEnable = 0;
    RHIBool32 stencilTestEnable = 0;
    RHIStencilOpState front;
    RHIStencilOpState back;
    float minDepthBounds = 0.0f;
    float maxDepthBounds = 1.0f;
};

struct RHIPipelineColorBlendAttachmentState {
    RHIBool32 blendEnable = 0;
    RHIBlendFactor srcColorBlendFactor = RHI_BLEND_FACTOR_ONE;
    RHIBlendFactor dstColorBlendFactor = RHI_BLEND_FACTOR_ZERO;
    RHIBlendOp colorBlendOp = RHI_BLEND_OP_ADD;
    RHIBlendFactor srcAlphaBlendFactor = RHI_BLEND_FACTOR_ONE;
    RHIBlendFactor dstAlphaBlendFactor = RHI_BLEND_FACTOR_ZERO;
    RHIBlendOp alphaBlendOp = RHI_BLEND_OP_ADD;
    RHIColorComponentFlags colorWriteMask = RHI_COLOR_COMPONENT_R_BIT | RHI_COLOR_COMPONENT_G_BIT |
                                             RHI_COLOR_COMPONENT_B_BIT | RHI_COLOR_COMPONENT_A_BIT;
};

struct RHIPipelineColorBlendStateCreateInfo {
    RHIPipelineColorBlendStateCreateFlags flags = 0;
    RHIBool32 logicOpEnable = 0;
    RHILogicOp logicOp = RHI_LOGIC_OP_COPY;
    std::vector<RHIPipelineColorBlendAttachmentState> attachments;
    float blendConstants[4] = {0.0f, 0.0f, 0.0f, 0.0f};
};

struct RHIPipelineDynamicStateCreateInfo {
    RHIPipelineDynamicStateCreateFlags flags = 0;
    std::vector<RHIDynamicState> dynamicStates;
};

// --- Push Constants ---
struct RHIPushConstantRange {
    RHIShaderStageFlags stageFlags = 0;
    std::uint32_t offset = 0;
    std::uint32_t size = 0;
};

struct RHIPipelineLayoutCreateInfo {
    RHIPipelineLayoutCreateFlags flags = 0;
    std::vector<RHIDescriptorSetLayout*> setLayouts;
    std::vector<RHIPushConstantRange> pushConstantRanges;
};

struct RHIGraphicsPipelineCreateInfo {
    RHIPipelineCreateFlags flags = 0;
    std::vector<RHIPipelineShaderStageCreateInfo> stages;
    RHIPipelineVertexInputStateCreateInfo* pVertexInputState = nullptr;
    RHIPipelineInputAssemblyStateCreateInfo* pInputAssemblyState = nullptr;
    RHIPipelineTessellationStateCreateInfo* pTessellationState = nullptr;
    RHIPipelineViewportStateCreateInfo* pViewportState = nullptr;
    RHIPipelineRasterizationStateCreateInfo* pRasterizationState = nullptr;
    RHIPipelineMultisampleStateCreateInfo* pMultisampleState = nullptr;
    RHIPipelineDepthStencilStateCreateInfo* pDepthStencilState = nullptr;
    RHIPipelineColorBlendStateCreateInfo* pColorBlendState = nullptr;
    RHIPipelineDynamicStateCreateInfo* pDynamicState = nullptr;
    RHIPipelineLayout* layout = nullptr;
    RHIRenderPass* renderPass = nullptr;
    std::uint32_t subpass = 0;
    RHIPipeline* basePipelineHandle = nullptr;
    std::int32_t basePipelineIndex = -1;
};

struct RHIComputePipelineCreateInfo {
    RHIPipelineCreateFlags flags = 0;
    RHIPipelineShaderStageCreateInfo stage;
    RHIPipelineLayout* layout = nullptr;
    RHIPipeline* basePipelineHandle = nullptr;
    std::int32_t basePipelineIndex = -1;
};

// --- 动态渲染 (Vulkan 1.4) ---
struct RHIRenderingAttachmentInfo {
    RHIImageView* imageView = nullptr;
    RHIImageLayout imageLayout = RHI_IMAGE_LAYOUT_UNDEFINED;
    RHIAttachmentLoadOp loadOp = RHI_ATTACHMENT_LOAD_OP_DONT_CARE;
    RHIAttachmentStoreOp storeOp = RHI_ATTACHMENT_STORE_OP_STORE;
    RHIClearValue clearValue;
};

struct RHIRenderingInfo {
    RHIRenderingInfoFlags flags = 0;
    RHIRect2D renderArea;
    std::uint32_t layerCount = 1;
    std::uint32_t viewMask = 0;
    std::vector<RHIRenderingAttachmentInfo> colorAttachments;
    RHIRenderingAttachmentInfo* pDepthAttachment = nullptr;
    RHIRenderingAttachmentInfo* pStencilAttachment = nullptr;
};

// --- 物理设备属性 ---
struct RHIPhysicalDeviceLimits {
    std::uint32_t maxImageDimension2D = 4096;
    std::uint32_t maxImageArrayLayers = 256;
    std::uint32_t maxBoundDescriptorSets = 4;
    std::uint32_t maxDescriptorSetSamplers = 256;
    std::uint32_t maxDescriptorSetUniformBuffers = 12;
    std::uint32_t maxDescriptorSetStorageBuffers = 8;
    std::uint32_t maxDescriptorSetSampledImages = 256;
    std::uint32_t maxPerStageDescriptorSamplers = 256;
    std::uint32_t maxPerStageDescriptorUniformBuffers = 12;
    std::uint32_t maxPerStageDescriptorStorageBuffers = 8;
    std::uint32_t maxPerStageDescriptorSampledImages = 256;
    std::uint32_t maxVertexInputAttributes = 16;
    std::uint32_t maxVertexInputBindings = 16;
    float maxSamplerAnisotropy = 16.0f;
    std::uint32_t maxColorAttachments = 8;
    std::uint32_t maxFramebufferWidth = 16384;
    std::uint32_t maxFramebufferHeight = 16384;
    float timestampPeriod = 1.0f;
};

struct RHIPhysicalDeviceProperties {
    std::uint32_t apiVersion = 0;
    std::uint32_t driverVersion = 0;
    std::uint32_t vendorID = 0;
    std::uint32_t deviceID = 0;
    RHIPhysicalDeviceType deviceType = RHI_PHYSICAL_DEVICE_TYPE_OTHER;
    char deviceName[RHI_MAX_PHYSICAL_DEVICE_NAME_SIZE] = {};
    RHIPhysicalDeviceLimits limits;
};

// ============================================================================
// 运行时结构体
// ============================================================================

struct QueueFamilyIndices {
    std::optional<std::uint32_t> graphicsFamily;
    std::optional<std::uint32_t> presentFamily;
    std::optional<std::uint32_t> computeFamily;
    std::optional<std::uint32_t> transferFamily;

    bool IsComplete() const {
        return graphicsFamily.has_value() && presentFamily.has_value() && computeFamily.has_value();
    }
};

struct RHISwapChainDesc {
    RHIExtent2D extent;
    RHIFormat imageFormat = RHI_FORMAT_UNDEFINED;
    RHIViewport viewport;
    RHIRect2D scissor;
};

struct RHIDepthImageDesc {
    RHIImage* depthImage = RHI_NULL_HANDLE;
    RHIImageView* depthImageView = RHI_NULL_HANDLE;
    RHIFormat depthImageFormat = RHI_FORMAT_UNDEFINED;
};

// ============================================================================
// RHI 初始化信息
// ============================================================================
struct RHIInitInfo {
    void* windowHandle = nullptr;
    bool enableValidationLayers = true;
};

} // namespace Crumb::Render
