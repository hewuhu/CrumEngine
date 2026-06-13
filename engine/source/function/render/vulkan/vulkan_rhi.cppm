module;
#include <vk_mem_alloc.h>

export module crumb.render.vulkan:vulkan_rhi;

import crumb.core;
import crumb.render;
import :vulkan_rhi_resource;
import :context;
import :swapchain;
import vulkan;
import vk_mem_alloc;
import std;

// ============================================================================
// VulkanRHI — Vulkan 后端的 RHI 具体实现 (final 类)
// 委托 VKContext 进行设备管理，自行管理命令池、同步等
// ============================================================================

export namespace Crumb::Render {

class VulkanRHI final : public RHI {
public:
    VulkanRHI() = default;
    ~VulkanRHI() override = default;

    // ---- 注入现有 VKContext 和 SwapChain（非 RHI 接口方法） ----
    void AttachToContext(VKContext& context, SwapChain& swapChain);

    // ========================================================================
    // RHI 接口实现
    // ========================================================================

    // 初始化
    bool initialize(const RHIInitInfo& initInfo) override;

    // Buffer
    RHIBuffer* createBuffer(const RHIBufferCreateInfo& info,
                            RHIMemoryPropertyFlags memoryFlags) override;
    void destroyBuffer(RHIBuffer* buffer) override;
    void copyBuffer(RHIBuffer* srcBuffer, RHIBuffer* dstBuffer,
                    RHIDeviceSize srcOffset, RHIDeviceSize dstOffset,
                    RHIDeviceSize size) override;

    // Image
    RHIImage* createImage(const RHIImageCreateInfo& info,
                           RHIMemoryPropertyFlags memoryFlags) override;
    void destroyImage(RHIImage* image) override;
    RHIImageView* createImageView(const RHIImageViewCreateInfo& info) override;
    void destroyImageView(RHIImageView* imageView) override;
    RHISampler* createSampler(const RHISamplerCreateInfo& info) override;
    void destroySampler(RHISampler* sampler) override;

    // Shader
    RHIShader* createShaderModule(const RHIShaderModuleCreateInfo& info) override;
    void destroyShaderModule(RHIShader* shader) override;

    // Descriptor
    RHIDescriptorPool* createDescriptorPool(const RHIDescriptorPoolCreateInfo& info) override;
    void destroyDescriptorPool(RHIDescriptorPool* pool) override;
    RHIDescriptorSetLayout*
    createDescriptorSetLayout(const RHIDescriptorSetLayoutCreateInfo& info) override;
    void destroyDescriptorSetLayout(RHIDescriptorSetLayout* layout) override;
    bool allocateDescriptorSets(const RHIDescriptorSetAllocateInfo& info,
                                 std::vector<RHIDescriptorSet*>& outSets) override;
    void updateDescriptorSets(const std::vector<RHIWriteDescriptorSet>& writes,
                               const std::vector<RHICopyDescriptorSet>& copies = {}) override;

    // Sync
    RHIFence* createFence(const RHIFenceCreateInfo& info) override;
    void destroyFence(RHIFence* fence) override;
    bool waitForFences(const std::vector<RHIFence*>& fences,
                        RHIBool32 waitAll, std::uint64_t timeout) override;
    bool resetFences(const std::vector<RHIFence*>& fences) override;
    RHISemaphore* createSemaphore(const RHISemaphoreCreateInfo& info) override;
    void destroySemaphore(RHISemaphore* semaphore) override;

    // Pipeline
    RHIPipeline* createGraphicsPipeline(const RHIGraphicsPipelineCreateInfo& info) override;
    RHIPipeline* createComputePipeline(const RHIComputePipelineCreateInfo& info) override;
    void destroyPipeline(RHIPipeline* pipeline) override;
    RHIPipelineLayout* createPipelineLayout(const RHIPipelineLayoutCreateInfo& info) override;
    void destroyPipelineLayout(RHIPipelineLayout* layout) override;

    // Command
    RHICommandPool* createCommandPool(const RHICommandPoolCreateInfo& info) override;
    void destroyCommandPool(RHICommandPool* pool) override;
    bool resetCommandPool(RHICommandPool* pool, RHICommandPoolResetFlags flags = 0) override;
    bool allocateCommandBuffers(const RHICommandBufferAllocateInfo& info,
                                 std::vector<RHICommandBuffer*>& outBuffers) override;
    void freeCommandBuffers(RHICommandPool* pool,
                             const std::vector<RHICommandBuffer*>& buffers) override;
    bool beginCommandBuffer(RHICommandBuffer* commandBuffer,
                             const RHICommandBufferBeginInfo& beginInfo) override;
    bool endCommandBuffer(RHICommandBuffer* commandBuffer) override;

    // Command Recording
    void cmdBindPipeline(RHICommandBuffer* cmd, RHIPipelineBindPoint bindPoint,
                          RHIPipeline* pipeline) override;
    void cmdBindVertexBuffers(RHICommandBuffer* cmd, std::uint32_t firstBinding,
                               const std::vector<RHIBuffer*>& buffers,
                               const std::vector<RHIDeviceSize>& offsets) override;
    void cmdBindIndexBuffer(RHICommandBuffer* cmd, RHIBuffer* buffer,
                             RHIDeviceSize offset, RHIIndexType indexType) override;
    void cmdBindDescriptorSets(RHICommandBuffer* cmd, RHIPipelineBindPoint bindPoint,
                                RHIPipelineLayout* layout, std::uint32_t firstSet,
                                const std::vector<RHIDescriptorSet*>& sets,
                                const std::vector<std::uint32_t>& dynamicOffsets = {}) override;
    void cmdDraw(RHICommandBuffer* cmd, std::uint32_t vertexCount,
                  std::uint32_t instanceCount = 1, std::uint32_t firstVertex = 0,
                  std::uint32_t firstInstance = 0) override;
    void cmdDrawIndexed(RHICommandBuffer* cmd, std::uint32_t indexCount,
                         std::uint32_t instanceCount = 1, std::uint32_t firstIndex = 0,
                         std::int32_t vertexOffset = 0,
                         std::uint32_t firstInstance = 0) override;
    void cmdDispatch(RHICommandBuffer* cmd, std::uint32_t groupCountX,
                      std::uint32_t groupCountY = 1, std::uint32_t groupCountZ = 1) override;
    void cmdSetViewport(RHICommandBuffer* cmd, std::uint32_t firstViewport,
                         const std::vector<RHIViewport>& viewports) override;
    void cmdSetScissor(RHICommandBuffer* cmd, std::uint32_t firstScissor,
                        const std::vector<RHIRect2D>& scissors) override;
    void cmdPipelineBarrier(RHICommandBuffer* cmd, RHIPipelineStageFlags srcStageMask,
                             RHIPipelineStageFlags dstStageMask,
                             RHIDependencyFlags dependencyFlags,
                             const std::vector<RHIMemoryBarrier>& memoryBarriers = {},
                             const std::vector<RHIBufferMemoryBarrier>& bufferBarriers = {},
                             const std::vector<RHIImageMemoryBarrier>& imageBarriers = {}) override;
    void cmdCopyBuffer(RHICommandBuffer* cmd, RHIBuffer* srcBuffer, RHIBuffer* dstBuffer,
                        const std::vector<RHIBufferCopy>& regions) override;
    void cmdCopyImageToBuffer(RHICommandBuffer* cmd, RHIImage* srcImage,
                               RHIImageLayout srcImageLayout, RHIBuffer* dstBuffer,
                               const std::vector<RHIBufferImageCopy>& regions) override;
    void cmdPushConstants(RHICommandBuffer* cmd, RHIPipelineLayout* layout,
                           RHIShaderStageFlags stageFlags, std::uint32_t offset,
                           std::uint32_t size, const void* data) override;
    void cmdBeginRendering(RHICommandBuffer* cmd,
                            const RHIRenderingInfo& renderingInfo) override;
    void cmdEndRendering(RHICommandBuffer* cmd) override;

    // Queue
    bool queueSubmit(RHIQueue* queue, const std::vector<RHISubmitInfo>& submits,
                      RHIFence* fence = nullptr) override;
    bool queueWaitIdle(RHIQueue* queue) override;

    // Memory
    bool mapMemory(RHIDeviceMemory* memory, RHIDeviceSize offset,
                    RHIDeviceSize size, void** ppData) override;
    void unmapMemory(RHIDeviceMemory* memory) override;

    // Query
    void getPhysicalDeviceProperties(RHIPhysicalDeviceProperties& outProps) override;
    RHICommandBuffer* getCurrentCommandBuffer() const override;
    RHICommandPool* getCommandPool() const override;
    RHIQueue* getGraphicsQueue() const override;
    RHIQueue* getComputeQueue() const override;
    QueueFamilyIndices getQueueFamilyIndices() const override;
    RHISwapChainDesc getSwapchainInfo() override;
    RHIDepthImageDesc getDepthImageInfo() const override;
    std::uint32_t getMaxFramesInFlight() const override;
    std::uint32_t getCurrentFrameIndex() const override;
    void setCurrentFrameIndex(std::uint32_t index) override;
    std::uint32_t getCurrentImageIndex() const { return m_imageIndex; }

    // Frame
    RHICommandBuffer* beginSingleTimeCommands() override;
    void endSingleTimeCommands(RHICommandBuffer* cmd) override;
    bool prepareBeforePass() override;
    void submitRendering() override;

    // 帧缓冲大小变更
    void setFramebufferResized() { m_framebufferResized = true; }
    void setClearColor(float r, float g, float b, float a) { m_clearColor = Color(r, g, b, a); }

private:
    // ---- 内部辅助 ----

    // 获取 VKContext 和设备
    vk::raii::Device& GetDevice();
    vma::raii::Allocator& GetAllocator();
    vk::raii::PhysicalDevice& GetPhysicalDevice();

    // 转换函数（RHI → Vulkan）
    static vk::Format ToVk(RHIFormat fmt) { return static_cast<vk::Format>(fmt); }
    static vk::ImageLayout ToVk(RHIImageLayout layout) { return static_cast<vk::ImageLayout>(layout); }
    static vk::ImageType ToVk(RHIImageType type) { return static_cast<vk::ImageType>(type); }
    static vk::ImageViewType ToVk(RHIImageViewType type) { return static_cast<vk::ImageViewType>(type); }
    static vk::ImageTiling ToVk(RHIImageTiling tiling) { return static_cast<vk::ImageTiling>(tiling); }
    static vk::ImageAspectFlags ToVkImgAspect(RHIImageAspectFlags f) { return static_cast<vk::ImageAspectFlags>(f); }
    static vk::ImageUsageFlags ToVkImgUsage(RHIImageUsageFlags f) { return static_cast<vk::ImageUsageFlags>(f); }
    static vk::BufferUsageFlags ToVkBufUsage(RHIBufferUsageFlags f) { return static_cast<vk::BufferUsageFlags>(f); }
    static vk::MemoryPropertyFlags ToVkMemProp(RHIMemoryPropertyFlags f) { return static_cast<vk::MemoryPropertyFlags>(f); }
    static vk::ShaderStageFlags ToVkShaderStage(RHIShaderStageFlags f) { return static_cast<vk::ShaderStageFlags>(f); }
    static vk::PipelineStageFlags ToVkPipeStage(RHIPipelineStageFlags f) { return static_cast<vk::PipelineStageFlags>(f); }
    static vk::AccessFlags ToVkAccess(RHIAccessFlags f) { return static_cast<vk::AccessFlags>(f); }
    static vk::DependencyFlags ToVkDep(RHIDependencyFlags f) { return static_cast<vk::DependencyFlags>(f); }
    static vk::CullModeFlags ToVkCull(RHICullModeFlags f) { return static_cast<vk::CullModeFlags>(f); }
    static vk::ColorComponentFlags ToVkColorComp(RHIColorComponentFlags f) { return static_cast<vk::ColorComponentFlags>(f); }
    static vk::SampleCountFlagBits ToVkSample(RHISampleCountFlags f) { return static_cast<vk::SampleCountFlagBits>(f); }
    static vk::CommandPoolCreateFlags ToVkCmdPoolCreate(RHICommandPoolCreateFlags f) { return static_cast<vk::CommandPoolCreateFlags>(f); }
    static vk::CommandBufferUsageFlags ToVkCmdBufUsage(RHICommandBufferUsageFlags f) { return static_cast<vk::CommandBufferUsageFlags>(f); }
    static vk::CommandBufferLevel ToVk(RHICommandBufferLevel l) { return static_cast<vk::CommandBufferLevel>(l); }
    static vk::FenceCreateFlags ToVkFenceCreate(RHIFenceCreateFlags f) { return static_cast<vk::FenceCreateFlags>(f); }
    static vk::SemaphoreCreateFlags ToVkSemCreate(RHISemaphoreCreateFlags f) { return static_cast<vk::SemaphoreCreateFlags>(f); }
    static vk::PipelineBindPoint ToVk(RHIPipelineBindPoint bp) { return static_cast<vk::PipelineBindPoint>(bp); }
    static vk::IndexType ToVk(RHIIndexType it) { return static_cast<vk::IndexType>(it); }
    static vk::DescriptorType ToVk(RHIDescriptorType dt) { return static_cast<vk::DescriptorType>(dt); }
    static vk::CompareOp ToVk(RHICompareOp op) { return static_cast<vk::CompareOp>(op); }
    static vk::Filter ToVk(RHIFilter f) { return static_cast<vk::Filter>(f); }
    static vk::SamplerMipmapMode ToVkMipmap(RHISamplerMipmapMode m) { return static_cast<vk::SamplerMipmapMode>(m); }
    static vk::SamplerAddressMode ToVk(RHISamplerAddressMode m) { return static_cast<vk::SamplerAddressMode>(m); }
    static vk::PrimitiveTopology ToVk(RHIPrimitiveTopology t) { return static_cast<vk::PrimitiveTopology>(t); }
    static vk::PolygonMode ToVk(RHIPolygonMode m) { return static_cast<vk::PolygonMode>(m); }
    static vk::FrontFace ToVk(RHIFrontFace ff) { return static_cast<vk::FrontFace>(ff); }
    static vk::BlendFactor ToVk(RHIBlendFactor bf) { return static_cast<vk::BlendFactor>(bf); }
    static vk::BlendOp ToVk(RHIBlendOp bo) { return static_cast<vk::BlendOp>(bo); }
    static vk::StencilOp ToVk(RHIStencilOp so) { return static_cast<vk::StencilOp>(so); }
    static vk::VertexInputRate ToVk(RHIVertexInputRate r) { return static_cast<vk::VertexInputRate>(r); }
    static vk::BorderColor ToVk(RHIBorderColor bc) { return static_cast<vk::BorderColor>(bc); }
    static vk::DynamicState ToVk(RHIDynamicState ds) { return static_cast<vk::DynamicState>(ds); }
    static vk::SharingMode ToVk(RHISharingMode sm) { return static_cast<vk::SharingMode>(sm); }
    static vk::AttachmentLoadOp ToVk(RHIAttachmentLoadOp op) { return static_cast<vk::AttachmentLoadOp>(op); }
    static vk::AttachmentStoreOp ToVk(RHIAttachmentStoreOp op) { return static_cast<vk::AttachmentStoreOp>(op); }
    static vk::ComponentSwizzle ToVk(RHIComponentSwizzle s) { return static_cast<vk::ComponentSwizzle>(s); }
    static vk::SubpassContents ToVk(RHISubpassContents sc) { return static_cast<vk::SubpassContents>(sc); }

    // VMA 内存属性转换
    static VmaMemoryUsage ToVmaMemUsage(RHIMemoryPropertyFlags flags);

    // 重建同步对象（swapchain resize 后调用）
    void RecreateSyncObjects();

    // ---- 成员 ----
    VKContext* m_context = nullptr;
    SwapChain* m_swapChain = nullptr;

    // 内部命令池和命令缓冲 (per frame-in-flight)
    static constexpr std::uint32_t kMaxFramesInFlight = 2;
    vk::raii::CommandPool m_rhiCommandPool = nullptr;
    std::vector<vk::raii::CommandBuffer> m_commandBuffers;

    // 同步：semaphore per swapchain image, fence per frame-in-flight
    std::vector<vk::raii::Semaphore> m_imageAvailableSemaphores;
    std::vector<vk::raii::Semaphore> m_renderFinishedSemaphores;
    std::vector<vk::raii::Fence> m_inFlightFences;

    // 帧状态
    std::uint32_t m_currentFrame = 0;
    std::uint32_t m_imageIndex = 0;
    Color m_clearColor{0.0f, 0.0f, 0.0f, 1.0f};
    bool m_initialized = false;
    bool m_framebufferResized = false;

    // RHI 包装器（复用，避免每帧 new）
    mutable VulkanCommandBuffer m_currentCmdWrapper;
    mutable VulkanQueue m_queueWrapper;
};

} // namespace Crumb::Render
