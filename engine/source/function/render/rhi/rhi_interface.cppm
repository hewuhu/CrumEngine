export module crumb.render:rhi_interface;

import :rhi_types;
import :rhi_struct;
import std;

// ============================================================================
// RHI — 渲染硬件抽象层 纯虚接口
// 约 60 个虚方法，覆盖渲染全生命周期
// 本模块不依赖任何 Vulkan 头文件
// ============================================================================

export namespace Crumb::Render {

class RHI {
public:
    virtual ~RHI() = default;

    // ========================================================================
    // 初始化
    // ========================================================================
    virtual bool initialize(const RHIInitInfo& initInfo) = 0;

    // ========================================================================
    // Buffer — 缓冲区
    // ========================================================================
    virtual RHIBuffer* createBuffer(const RHIBufferCreateInfo& info,
                                    RHIMemoryPropertyFlags memoryFlags) = 0;
    virtual void destroyBuffer(RHIBuffer* buffer) = 0;

    virtual void copyBuffer(RHIBuffer* srcBuffer, RHIBuffer* dstBuffer,
                            RHIDeviceSize srcOffset, RHIDeviceSize dstOffset,
                            RHIDeviceSize size) = 0;

    // ========================================================================
    // Image — 图像
    // ========================================================================
    virtual RHIImage* createImage(const RHIImageCreateInfo& info,
                                   RHIMemoryPropertyFlags memoryFlags) = 0;
    virtual void destroyImage(RHIImage* image) = 0;

    virtual RHIImageView* createImageView(const RHIImageViewCreateInfo& info) = 0;
    virtual void destroyImageView(RHIImageView* imageView) = 0;

    virtual RHISampler* createSampler(const RHISamplerCreateInfo& info) = 0;
    virtual void destroySampler(RHISampler* sampler) = 0;

    // ========================================================================
    // Shader — 着色器
    // ========================================================================
    virtual RHIShader* createShaderModule(const RHIShaderModuleCreateInfo& info) = 0;
    virtual void destroyShaderModule(RHIShader* shader) = 0;

    // ========================================================================
    // Descriptor — 描述符
    // ========================================================================
    virtual RHIDescriptorPool* createDescriptorPool(const RHIDescriptorPoolCreateInfo& info) = 0;
    virtual void destroyDescriptorPool(RHIDescriptorPool* pool) = 0;

    virtual RHIDescriptorSetLayout*
    createDescriptorSetLayout(const RHIDescriptorSetLayoutCreateInfo& info) = 0;
    virtual void destroyDescriptorSetLayout(RHIDescriptorSetLayout* layout) = 0;

    virtual bool allocateDescriptorSets(const RHIDescriptorSetAllocateInfo& info,
                                         std::vector<RHIDescriptorSet*>& outSets) = 0;

    virtual void updateDescriptorSets(const std::vector<RHIWriteDescriptorSet>& writes,
                                      const std::vector<RHICopyDescriptorSet>& copies = {}) = 0;

    // ========================================================================
    // Sync — 同步原语
    // ========================================================================
    virtual RHIFence* createFence(const RHIFenceCreateInfo& info) = 0;
    virtual void destroyFence(RHIFence* fence) = 0;
    virtual bool waitForFences(const std::vector<RHIFence*>& fences,
                                RHIBool32 waitAll, std::uint64_t timeout) = 0;
    virtual bool resetFences(const std::vector<RHIFence*>& fences) = 0;

    virtual RHISemaphore* createSemaphore(const RHISemaphoreCreateInfo& info) = 0;
    virtual void destroySemaphore(RHISemaphore* semaphore) = 0;

    // ========================================================================
    // Pipeline — 管线
    // ========================================================================
    virtual RHIPipeline*
    createGraphicsPipeline(const RHIGraphicsPipelineCreateInfo& info) = 0;
    virtual RHIPipeline*
    createComputePipeline(const RHIComputePipelineCreateInfo& info) = 0;
    virtual void destroyPipeline(RHIPipeline* pipeline) = 0;

    virtual RHIPipelineLayout*
    createPipelineLayout(const RHIPipelineLayoutCreateInfo& info) = 0;
    virtual void destroyPipelineLayout(RHIPipelineLayout* layout) = 0;

    // ========================================================================
    // Command — 命令池和命令缓冲
    // ========================================================================
    virtual RHICommandPool* createCommandPool(const RHICommandPoolCreateInfo& info) = 0;
    virtual void destroyCommandPool(RHICommandPool* pool) = 0;
    virtual bool resetCommandPool(RHICommandPool* pool,
                                   RHICommandPoolResetFlags flags = 0) = 0;

    virtual bool allocateCommandBuffers(const RHICommandBufferAllocateInfo& info,
                                         std::vector<RHICommandBuffer*>& outBuffers) = 0;
    virtual void freeCommandBuffers(RHICommandPool* pool,
                                     const std::vector<RHICommandBuffer*>& buffers) = 0;

    virtual bool beginCommandBuffer(RHICommandBuffer* commandBuffer,
                                     const RHICommandBufferBeginInfo& beginInfo) = 0;
    virtual bool endCommandBuffer(RHICommandBuffer* commandBuffer) = 0;

    // ========================================================================
    // Command Recording — 命令录制
    // ========================================================================

    // 绑定
    virtual void cmdBindPipeline(RHICommandBuffer* cmd,
                                  RHIPipelineBindPoint bindPoint,
                                  RHIPipeline* pipeline) = 0;
    virtual void cmdBindVertexBuffers(RHICommandBuffer* cmd,
                                       std::uint32_t firstBinding,
                                       const std::vector<RHIBuffer*>& buffers,
                                       const std::vector<RHIDeviceSize>& offsets) = 0;
    virtual void cmdBindIndexBuffer(RHICommandBuffer* cmd,
                                     RHIBuffer* buffer,
                                     RHIDeviceSize offset,
                                     RHIIndexType indexType) = 0;
    virtual void cmdBindDescriptorSets(RHICommandBuffer* cmd,
                                        RHIPipelineBindPoint bindPoint,
                                        RHIPipelineLayout* layout,
                                        std::uint32_t firstSet,
                                        const std::vector<RHIDescriptorSet*>& sets,
                                        const std::vector<std::uint32_t>& dynamicOffsets = {}) = 0;

    // 绘制
    virtual void cmdDraw(RHICommandBuffer* cmd,
                          std::uint32_t vertexCount,
                          std::uint32_t instanceCount = 1,
                          std::uint32_t firstVertex = 0,
                          std::uint32_t firstInstance = 0) = 0;
    virtual void cmdDrawIndexed(RHICommandBuffer* cmd,
                                 std::uint32_t indexCount,
                                 std::uint32_t instanceCount = 1,
                                 std::uint32_t firstIndex = 0,
                                 std::int32_t vertexOffset = 0,
                                 std::uint32_t firstInstance = 0) = 0;
    virtual void cmdDispatch(RHICommandBuffer* cmd,
                              std::uint32_t groupCountX,
                              std::uint32_t groupCountY = 1,
                              std::uint32_t groupCountZ = 1) = 0;

    // 视口/裁剪
    virtual void cmdSetViewport(RHICommandBuffer* cmd,
                                 std::uint32_t firstViewport,
                                 const std::vector<RHIViewport>& viewports) = 0;
    virtual void cmdSetScissor(RHICommandBuffer* cmd,
                                std::uint32_t firstScissor,
                                const std::vector<RHIRect2D>& scissors) = 0;

    // Barrier
    virtual void cmdPipelineBarrier(RHICommandBuffer* cmd,
                                     RHIPipelineStageFlags srcStageMask,
                                     RHIPipelineStageFlags dstStageMask,
                                     RHIDependencyFlags dependencyFlags,
                                     const std::vector<RHIMemoryBarrier>& memoryBarriers = {},
                                     const std::vector<RHIBufferMemoryBarrier>& bufferBarriers = {},
                                     const std::vector<RHIImageMemoryBarrier>& imageBarriers = {}) = 0;

    // 拷贝
    virtual void cmdCopyBuffer(RHICommandBuffer* cmd,
                                RHIBuffer* srcBuffer,
                                RHIBuffer* dstBuffer,
                                const std::vector<RHIBufferCopy>& regions) = 0;
    virtual void cmdCopyImageToBuffer(RHICommandBuffer* cmd,
                                       RHIImage* srcImage,
                                       RHIImageLayout srcImageLayout,
                                       RHIBuffer* dstBuffer,
                                       const std::vector<RHIBufferImageCopy>& regions) = 0;

    // Push Constants
    virtual void cmdPushConstants(RHICommandBuffer* cmd,
                                   RHIPipelineLayout* layout,
                                   RHIShaderStageFlags stageFlags,
                                   std::uint32_t offset,
                                   std::uint32_t size,
                                   const void* data) = 0;

    // 动态渲染 (Vulkan 1.4)
    virtual void cmdBeginRendering(RHICommandBuffer* cmd,
                                    const RHIRenderingInfo& renderingInfo) = 0;
    virtual void cmdEndRendering(RHICommandBuffer* cmd) = 0;

    // ========================================================================
    // Queue — 队列操作
    // ========================================================================
    virtual bool queueSubmit(RHIQueue* queue,
                              const std::vector<RHISubmitInfo>& submits,
                              RHIFence* fence = nullptr) = 0;
    virtual bool queueWaitIdle(RHIQueue* queue) = 0;

    // ========================================================================
    // Memory — 内存操作
    // ========================================================================
    virtual bool mapMemory(RHIDeviceMemory* memory,
                            RHIDeviceSize offset,
                            RHIDeviceSize size,
                            void** ppData) = 0;
    virtual void unmapMemory(RHIDeviceMemory* memory) = 0;

    // ========================================================================
    // Query — 查询
    // ========================================================================
    virtual void getPhysicalDeviceProperties(RHIPhysicalDeviceProperties& outProps) = 0;
    virtual RHICommandBuffer* getCurrentCommandBuffer() const = 0;
    virtual RHICommandPool* getCommandPool() const = 0;
    virtual RHIQueue* getGraphicsQueue() const = 0;
    virtual RHIQueue* getComputeQueue() const = 0;
    virtual QueueFamilyIndices getQueueFamilyIndices() const = 0;
    virtual RHISwapChainDesc getSwapchainInfo() = 0;
    virtual RHIDepthImageDesc getDepthImageInfo() const = 0;
    virtual std::uint32_t getMaxFramesInFlight() const = 0;
    virtual std::uint32_t getCurrentFrameIndex() const = 0;
    virtual void setCurrentFrameIndex(std::uint32_t index) = 0;

    // ========================================================================
    // Frame — 帧循环辅助
    // ========================================================================
    virtual RHICommandBuffer* beginSingleTimeCommands() = 0;
    virtual void endSingleTimeCommands(RHICommandBuffer* cmd) = 0;
    virtual bool prepareBeforePass() = 0;
    virtual void submitRendering() = 0;
};

} // namespace Crumb::Render
