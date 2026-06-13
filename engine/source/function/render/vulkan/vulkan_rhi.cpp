module;
#include <vk_mem_alloc.h>

module crumb.render.vulkan;
import :vulkan_rhi;

import crumb.core;
import vulkan;
import vk_mem_alloc;
import std;

namespace Crumb::Render {

// ============================================================================
// 内部辅助
// ============================================================================

vk::raii::Device& VulkanRHI::GetDevice() {
    return m_context->GetDevice();
}

vma::raii::Allocator& VulkanRHI::GetAllocator() {
    return m_context->GetAllocator();
}

vk::raii::PhysicalDevice& VulkanRHI::GetPhysicalDevice() {
    return m_context->GetPhysicalDevice();
}

VmaMemoryUsage VulkanRHI::ToVmaMemUsage(RHIMemoryPropertyFlags flags) {
    if (flags & RHI_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
        if (flags & RHI_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
            return VMA_MEMORY_USAGE_CPU_TO_GPU;
        return VMA_MEMORY_USAGE_GPU_ONLY;
    }
    if (flags & RHI_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
        return VMA_MEMORY_USAGE_CPU_ONLY;
    return VMA_MEMORY_USAGE_GPU_ONLY;
}

// ============================================================================
// AttachToContext — 注入现有的 VKContext 和 SwapChain
// ============================================================================

void VulkanRHI::AttachToContext(VKContext& context, SwapChain& swapChain) {
    m_context = &context;
    m_swapChain = &swapChain;
}

// ============================================================================
// initialize
// ============================================================================

bool VulkanRHI::initialize(const RHIInitInfo& initInfo) {
    if (!m_context || !m_swapChain) {
        Log::Error("[VulkanRHI] Must call AttachToContext before initialize");
        return false;
    }

    auto& device = GetDevice();
    auto& physicalDevice = GetPhysicalDevice();
    auto& queueIndices = m_context->GetQueueFamilyIndices();

    // 1. 创建 RHI 命令池
    vk::CommandPoolCreateInfo poolCI{};
    poolCI.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
          .setQueueFamilyIndex(queueIndices.graphicsFamily.value());
    m_rhiCommandPool = vk::raii::CommandPool(device, poolCI);

    // 2. 创建命令缓冲 (per frame-in-flight)
    std::uint32_t imageCount = m_swapChain->GetImageCount();
    m_commandBuffers.clear();
    vk::CommandBufferAllocateInfo cbAllocInfo{};
    cbAllocInfo.setCommandPool(*m_rhiCommandPool)
               .setLevel(vk::CommandBufferLevel::ePrimary)
               .setCommandBufferCount(kMaxFramesInFlight);
    m_commandBuffers = vk::raii::CommandBuffers(device, cbAllocInfo);

    // 3. 创建同步对象：semaphore per image, fence per frame
    m_imageAvailableSemaphores.clear();
    m_renderFinishedSemaphores.clear();
    m_inFlightFences.clear();

    vk::SemaphoreCreateInfo semCI{};
    for (std::uint32_t i = 0; i < imageCount; ++i) {
        m_imageAvailableSemaphores.emplace_back(device, semCI);
        m_renderFinishedSemaphores.emplace_back(device, semCI);
    }

    vk::FenceCreateInfo fenceCI{};
    fenceCI.setFlags(vk::FenceCreateFlagBits::eSignaled);
    for (std::uint32_t i = 0; i < kMaxFramesInFlight; ++i) {
        m_inFlightFences.emplace_back(device, fenceCI);
    }

    m_currentFrame = 0;
    m_imageIndex = 0;
    m_initialized = true;

    Log::Info("[VulkanRHI] Initialized: {} images, {} frames in flight", imageCount, kMaxFramesInFlight);
    return true;
}

// ============================================================================
// Buffer
// ============================================================================

RHIBuffer* VulkanRHI::createBuffer(const RHIBufferCreateInfo& info,
                                     RHIMemoryPropertyFlags memoryFlags) {
    auto& allocator = GetAllocator();
    auto& device = GetDevice();
    (void)device;

    vk::BufferCreateInfo vkCI{};
    vkCI.setSize(info.size)
        .setUsage(ToVkBufUsage(info.usage))
        .setSharingMode(ToVk(info.sharingMode));
    if (!info.queueFamilyIndices.empty()) {
        vkCI.setQueueFamilyIndices(info.queueFamilyIndices);
    }

    VmaAllocationCreateInfo vmaCI{};
    vmaCI.usage = ToVmaMemUsage(memoryFlags);

    auto* wrapper = new VulkanBuffer();
    try {
        wrapper->SetResource(vma::raii::Buffer(allocator, vkCI, vmaCI));
    } catch (const std::exception& e) {
        Log::Error("[VulkanRHI] createBuffer failed: {}", e.what());
        delete wrapper;
        return nullptr;
    }
    return wrapper;
}

void VulkanRHI::destroyBuffer(RHIBuffer* buffer) {
    delete static_cast<VulkanBuffer*>(buffer);
}

void VulkanRHI::copyBuffer(RHIBuffer* srcBuffer, RHIBuffer* dstBuffer,
                             RHIDeviceSize srcOffset, RHIDeviceSize dstOffset,
                             RHIDeviceSize size) {
    auto* cmd = beginSingleTimeCommands();
    if (!cmd) return;

    auto vkSrc = static_cast<VulkanBuffer*>(srcBuffer)->GetRaw();
    auto vkDst = static_cast<VulkanBuffer*>(dstBuffer)->GetRaw();

    vk::BufferCopy copyRegion{};
    copyRegion.setSrcOffset(srcOffset)
              .setDstOffset(dstOffset)
              .setSize(size);

    auto vkCmd = static_cast<VulkanCommandBuffer*>(cmd)->GetRaw();
    vkCmd.copyBuffer(vkSrc, vkDst, copyRegion);

    endSingleTimeCommands(cmd);
}

// ============================================================================
// Image
// ============================================================================

RHIImage* VulkanRHI::createImage(const RHIImageCreateInfo& info,
                                   RHIMemoryPropertyFlags memoryFlags) {
    auto& allocator = GetAllocator();

    vk::ImageCreateInfo vkCI{};
    vkCI.setImageType(ToVk(info.imageType))
        .setFormat(ToVk(info.format))
        .setExtent({info.extent.width, info.extent.height, info.extent.depth})
        .setMipLevels(info.mipLevels)
        .setArrayLayers(info.arrayLayers)
        .setSamples(ToVkSample(info.samples))
        .setTiling(ToVk(info.tiling))
        .setUsage(ToVkImgUsage(info.usage))
        .setSharingMode(ToVk(info.sharingMode))
        .setInitialLayout(ToVk(info.initialLayout));

    if (!info.queueFamilyIndices.empty()) {
        vkCI.setQueueFamilyIndices(info.queueFamilyIndices);
    }
    if (info.flags != 0) {
        vkCI.setFlags(static_cast<vk::ImageCreateFlags>(info.flags));
    }

    VmaAllocationCreateInfo vmaCI{};
    vmaCI.usage = ToVmaMemUsage(memoryFlags);

    auto* wrapper = new VulkanImage();
    try {
        wrapper->SetResource(vma::raii::Image(allocator, vkCI, vmaCI));
    } catch (const std::exception& e) {
        Log::Error("[VulkanRHI] createImage failed: {}", e.what());
        delete wrapper;
        return nullptr;
    }
    return wrapper;
}

void VulkanRHI::destroyImage(RHIImage* image) {
    delete static_cast<VulkanImage*>(image);
}

RHIImageView* VulkanRHI::createImageView(const RHIImageViewCreateInfo& info) {
    auto& device = GetDevice();

    vk::ImageViewCreateInfo vkCI{};
    vkCI.setImage(static_cast<VulkanImage*>(info.image)->GetRaw())
        .setViewType(ToVk(info.viewType))
        .setFormat(ToVk(info.format))
        .setComponents({
            ToVk(info.components.r),
            ToVk(info.components.g),
            ToVk(info.components.b),
            ToVk(info.components.a)})
        .setSubresourceRange({
            ToVkImgAspect(info.subresourceRange.aspectMask),
            info.subresourceRange.baseMipLevel,
            info.subresourceRange.levelCount,
            info.subresourceRange.baseArrayLayer,
            info.subresourceRange.layerCount});

    auto* wrapper = new VulkanImageView();
    try {
        wrapper->SetResource(vk::raii::ImageView(device, vkCI));
    } catch (const std::exception& e) {
        Log::Error("[VulkanRHI] createImageView failed: {}", e.what());
        delete wrapper;
        return nullptr;
    }
    return wrapper;
}

void VulkanRHI::destroyImageView(RHIImageView* imageView) {
    delete static_cast<VulkanImageView*>(imageView);
}

RHISampler* VulkanRHI::createSampler(const RHISamplerCreateInfo& info) {
    auto& device = GetDevice();

    vk::SamplerCreateInfo vkCI{};
    vkCI.setMagFilter(ToVk(info.magFilter))
        .setMinFilter(ToVk(info.minFilter))
        .setMipmapMode(ToVkMipmap(info.mipmapMode))
        .setAddressModeU(ToVk(info.addressModeU))
        .setAddressModeV(ToVk(info.addressModeV))
        .setAddressModeW(ToVk(info.addressModeW))
        .setMipLodBias(info.mipLodBias)
        .setAnisotropyEnable(static_cast<vk::Bool32>(info.anisotropyEnable))
        .setMaxAnisotropy(info.maxAnisotropy)
        .setCompareEnable(static_cast<vk::Bool32>(info.compareEnable))
        .setCompareOp(ToVk(info.compareOp))
        .setMinLod(info.minLod)
        .setMaxLod(info.maxLod)
        .setBorderColor(ToVk(info.borderColor))
        .setUnnormalizedCoordinates(static_cast<vk::Bool32>(info.unnormalizedCoordinates));

    auto* wrapper = new VulkanSampler();
    try {
        wrapper->SetResource(vk::raii::Sampler(device, vkCI));
    } catch (const std::exception& e) {
        Log::Error("[VulkanRHI] createSampler failed: {}", e.what());
        delete wrapper;
        return nullptr;
    }
    return wrapper;
}

void VulkanRHI::destroySampler(RHISampler* sampler) {
    delete static_cast<VulkanSampler*>(sampler);
}

// ============================================================================
// Shader
// ============================================================================

RHIShader* VulkanRHI::createShaderModule(const RHIShaderModuleCreateInfo& info) {
    auto& device = GetDevice();

    vk::ShaderModuleCreateInfo vkCI{};
    vkCI.setCodeSize(info.code.size() * sizeof(std::uint32_t))
        .setPCode(info.code.data());

    auto* wrapper = new VulkanShader();
    try {
        wrapper->SetResource(vk::raii::ShaderModule(device, vkCI));
    } catch (const std::exception& e) {
        Log::Error("[VulkanRHI] createShaderModule failed: {}", e.what());
        delete wrapper;
        return nullptr;
    }
    return wrapper;
}

void VulkanRHI::destroyShaderModule(RHIShader* shader) {
    delete static_cast<VulkanShader*>(shader);
}

// ============================================================================
// Descriptor
// ============================================================================

RHIDescriptorPool* VulkanRHI::createDescriptorPool(const RHIDescriptorPoolCreateInfo& info) {
    auto& device = GetDevice();

    std::vector<vk::DescriptorPoolSize> poolSizes;
    poolSizes.reserve(info.poolSizes.size());
    for (auto& ps : info.poolSizes) {
        vk::DescriptorPoolSize vkPS{};
        vkPS.setType(ToVk(ps.type))
            .setDescriptorCount(ps.descriptorCount);
        poolSizes.push_back(vkPS);
    }

    vk::DescriptorPoolCreateInfo vkCI{};
    vkCI.setFlags(static_cast<vk::DescriptorPoolCreateFlags>(info.flags))
        .setMaxSets(info.maxSets)
        .setPoolSizes(poolSizes);

    auto* wrapper = new VulkanDescriptorPool();
    try {
        wrapper->SetResource(vk::raii::DescriptorPool(device, vkCI));
    } catch (const std::exception& e) {
        Log::Error("[VulkanRHI] createDescriptorPool failed: {}", e.what());
        delete wrapper;
        return nullptr;
    }
    return wrapper;
}

void VulkanRHI::destroyDescriptorPool(RHIDescriptorPool* pool) {
    delete static_cast<VulkanDescriptorPool*>(pool);
}

RHIDescriptorSetLayout*
VulkanRHI::createDescriptorSetLayout(const RHIDescriptorSetLayoutCreateInfo& info) {
    auto& device = GetDevice();

    std::vector<vk::DescriptorSetLayoutBinding> bindings;
    bindings.reserve(info.bindings.size());
    for (auto& b : info.bindings) {
        vk::DescriptorSetLayoutBinding vkB{};
        vkB.setBinding(b.binding)
           .setDescriptorType(ToVk(b.descriptorType))
           .setDescriptorCount(b.descriptorCount)
           .setStageFlags(ToVkShaderStage(b.stageFlags));
        bindings.push_back(vkB);
    }

    vk::DescriptorSetLayoutCreateInfo vkCI{};
    vkCI.setFlags(static_cast<vk::DescriptorSetLayoutCreateFlags>(info.flags))
        .setBindings(bindings);

    auto* wrapper = new VulkanDescriptorSetLayout();
    try {
        wrapper->SetResource(vk::raii::DescriptorSetLayout(device, vkCI));
    } catch (const std::exception& e) {
        Log::Error("[VulkanRHI] createDescriptorSetLayout failed: {}", e.what());
        delete wrapper;
        return nullptr;
    }
    return wrapper;
}

void VulkanRHI::destroyDescriptorSetLayout(RHIDescriptorSetLayout* layout) {
    delete static_cast<VulkanDescriptorSetLayout*>(layout);
}

bool VulkanRHI::allocateDescriptorSets(const RHIDescriptorSetAllocateInfo& info,
                                         std::vector<RHIDescriptorSet*>& outSets) {
    auto& device = GetDevice();
    auto vkPool = static_cast<VulkanDescriptorPool*>(info.descriptorPool)->GetRaw();

    std::vector<vk::DescriptorSetLayout> vkLayouts;
    vkLayouts.reserve(info.setLayouts.size());
    for (auto* sl : info.setLayouts) {
        vkLayouts.push_back(static_cast<VulkanDescriptorSetLayout*>(sl)->GetRaw());
    }

    vk::DescriptorSetAllocateInfo vkAI{};
    vkAI.setDescriptorPool(vkPool)
        .setSetLayouts(vkLayouts);

    try {
        auto vkSets = vk::raii::DescriptorSets(device, vkAI);
        outSets.clear();
        for (auto& vkSet : vkSets) {
            auto* wrapper = new VulkanDescriptorSet();
            wrapper->Set(*vkSet);
            outSets.push_back(wrapper);
        }
    } catch (const std::exception& e) {
        Log::Error("[VulkanRHI] allocateDescriptorSets failed: {}", e.what());
        return false;
    }
    return true;
}

void VulkanRHI::updateDescriptorSets(const std::vector<RHIWriteDescriptorSet>& writes,
                                       const std::vector<RHICopyDescriptorSet>& copies) {
    // Convert RHI structures to Vulkan structures and call vkUpdateDescriptorSets
    // This is simplified — a full implementation would handle all descriptor types
    (void)writes;
    (void)copies;
    // TODO: Implement full descriptor write conversion
}

// ============================================================================
// Sync
// ============================================================================

RHIFence* VulkanRHI::createFence(const RHIFenceCreateInfo& info) {
    auto& device = GetDevice();
    vk::FenceCreateInfo vkCI{};
    vkCI.setFlags(ToVkFenceCreate(info.flags));

    auto* wrapper = new VulkanFence();
    try {
        wrapper->SetResource(vk::raii::Fence(device, vkCI));
    } catch (const std::exception& e) {
        Log::Error("[VulkanRHI] createFence failed: {}", e.what());
        delete wrapper;
        return nullptr;
    }
    return wrapper;
}

void VulkanRHI::destroyFence(RHIFence* fence) {
    delete static_cast<VulkanFence*>(fence);
}

bool VulkanRHI::waitForFences(const std::vector<RHIFence*>& fences,
                                RHIBool32 waitAll, std::uint64_t timeout) {
    auto& device = GetDevice();
    std::vector<vk::Fence> vkFences;
    vkFences.reserve(fences.size());
    for (auto* f : fences) {
        vkFences.push_back(static_cast<VulkanFence*>(f)->GetRaw());
    }
    vk::Result result = device.waitForFences(vkFences, static_cast<vk::Bool32>(waitAll), timeout);
    return result == vk::Result::eSuccess;
}

bool VulkanRHI::resetFences(const std::vector<RHIFence*>& fences) {
    auto& device = GetDevice();
    std::vector<vk::Fence> vkFences;
    vkFences.reserve(fences.size());
    for (auto* f : fences) {
        vkFences.push_back(static_cast<VulkanFence*>(f)->GetRaw());
    }
    device.resetFences(vkFences);
    return true;
}

RHISemaphore* VulkanRHI::createSemaphore(const RHISemaphoreCreateInfo& info) {
    auto& device = GetDevice();
    vk::SemaphoreCreateInfo vkCI{};
    vkCI.setFlags(ToVkSemCreate(info.flags));

    auto* wrapper = new VulkanSemaphore();
    try {
        wrapper->SetResource(vk::raii::Semaphore(device, vkCI));
    } catch (const std::exception& e) {
        Log::Error("[VulkanRHI] createSemaphore failed: {}", e.what());
        delete wrapper;
        return nullptr;
    }
    return wrapper;
}

void VulkanRHI::destroySemaphore(RHISemaphore* semaphore) {
    delete static_cast<VulkanSemaphore*>(semaphore);
}

// ============================================================================
// Pipeline
// ============================================================================

RHIPipeline* VulkanRHI::createGraphicsPipeline(const RHIGraphicsPipelineCreateInfo& info) {
    // TODO: Full implementation requires careful conversion of all pipeline state structs
    Log::Warn("[VulkanRHI] createGraphicsPipeline not yet fully implemented");
    return nullptr;
}

RHIPipeline* VulkanRHI::createComputePipeline(const RHIComputePipelineCreateInfo& info) {
    // TODO: Full implementation
    Log::Warn("[VulkanRHI] createComputePipeline not yet fully implemented");
    return nullptr;
}

void VulkanRHI::destroyPipeline(RHIPipeline* pipeline) {
    delete static_cast<VulkanPipeline*>(pipeline);
}

RHIPipelineLayout*
VulkanRHI::createPipelineLayout(const RHIPipelineLayoutCreateInfo& info) {
    // TODO: Full implementation
    Log::Warn("[VulkanRHI] createPipelineLayout not yet fully implemented");
    return nullptr;
}

void VulkanRHI::destroyPipelineLayout(RHIPipelineLayout* layout) {
    delete static_cast<VulkanPipelineLayout*>(layout);
}

// ============================================================================
// Command
// ============================================================================

RHICommandPool* VulkanRHI::createCommandPool(const RHICommandPoolCreateInfo& info) {
    auto& device = GetDevice();
    vk::CommandPoolCreateInfo vkCI{};
    vkCI.setFlags(ToVkCmdPoolCreate(info.flags))
        .setQueueFamilyIndex(info.queueFamilyIndex);

    auto* wrapper = new VulkanCommandPool();
    try {
        wrapper->SetResource(vk::raii::CommandPool(device, vkCI));
    } catch (const std::exception& e) {
        Log::Error("[VulkanRHI] createCommandPool failed: {}", e.what());
        delete wrapper;
        return nullptr;
    }
    return wrapper;
}

void VulkanRHI::destroyCommandPool(RHICommandPool* pool) {
    delete static_cast<VulkanCommandPool*>(pool);
}

bool VulkanRHI::resetCommandPool(RHICommandPool* pool, RHICommandPoolResetFlags flags) {
    // vk::raii::CommandPool handles reset internally; no-op for now
    (void)pool; (void)flags;
    return true;
}

bool VulkanRHI::allocateCommandBuffers(const RHICommandBufferAllocateInfo& info,
                                         std::vector<RHICommandBuffer*>& outBuffers) {
    auto& device = GetDevice();
    auto rawPool = static_cast<VulkanCommandPool*>(info.commandPool)->GetRaw();

    vk::CommandBufferAllocateInfo vkAI{};
    vkAI.setCommandPool(rawPool)
        .setLevel(ToVk(info.level))
        .setCommandBufferCount(info.commandBufferCount);

    try {
        auto vkBuffers = vk::raii::CommandBuffers(device, vkAI);
        outBuffers.clear();
        for (auto& vkBuf : vkBuffers) {
            auto* wrapper = new VulkanCommandBuffer();
            wrapper->Set(*vkBuf);
            outBuffers.push_back(wrapper);
        }
        // Release vk::raii ownership — command buffers are freed by pool reset/destroy
        for (auto& vkBuf : vkBuffers) {
            vkBuf.release();
        }
    } catch (const std::exception& e) {
        Log::Error("[VulkanRHI] allocateCommandBuffers failed: {}", e.what());
        return false;
    }
    return true;
}

void VulkanRHI::freeCommandBuffers(RHICommandPool* /*pool*/,
                                     const std::vector<RHICommandBuffer*>& buffers) {
    for (auto* b : buffers) delete static_cast<VulkanCommandBuffer*>(b);
}

bool VulkanRHI::beginCommandBuffer(RHICommandBuffer* commandBuffer,
                                     const RHICommandBufferBeginInfo& beginInfo) {
    auto vkCmd = static_cast<VulkanCommandBuffer*>(commandBuffer)->GetRaw();
    vk::CommandBufferBeginInfo vkBI{};
    vkBI.setFlags(ToVkCmdBufUsage(beginInfo.flags));
    vkCmd.begin(vkBI);
    return true;
}

bool VulkanRHI::endCommandBuffer(RHICommandBuffer* commandBuffer) {
    auto vkCmd = static_cast<VulkanCommandBuffer*>(commandBuffer)->GetRaw();
    vkCmd.end();
    return true;
}

// ============================================================================
// Command Recording — 基本绘制命令
// ============================================================================

void VulkanRHI::cmdBindPipeline(RHICommandBuffer* cmd, RHIPipelineBindPoint bindPoint,
                                  RHIPipeline* pipeline) {
    auto vkCmd = static_cast<VulkanCommandBuffer*>(cmd)->GetRaw();
    auto vkPipeline = static_cast<VulkanPipeline*>(pipeline)->GetRaw();
    vkCmd.bindPipeline(ToVk(bindPoint), vkPipeline);
}

void VulkanRHI::cmdBindVertexBuffers(RHICommandBuffer* cmd, std::uint32_t firstBinding,
                                       const std::vector<RHIBuffer*>& buffers,
                                       const std::vector<RHIDeviceSize>& offsets) {
    auto vkCmd = static_cast<VulkanCommandBuffer*>(cmd)->GetRaw();
    std::vector<vk::Buffer> vkBufs;
    vkBufs.reserve(buffers.size());
    for (auto* b : buffers) {
        vkBufs.push_back(static_cast<VulkanBuffer*>(b)->GetRaw());
    }
    vkCmd.bindVertexBuffers(firstBinding, vkBufs, offsets);
}

void VulkanRHI::cmdBindIndexBuffer(RHICommandBuffer* cmd, RHIBuffer* buffer,
                                     RHIDeviceSize offset, RHIIndexType indexType) {
    auto vkCmd = static_cast<VulkanCommandBuffer*>(cmd)->GetRaw();
    auto vkBuf = static_cast<VulkanBuffer*>(buffer)->GetRaw();
    vkCmd.bindIndexBuffer(vkBuf, offset, ToVk(indexType));
}

void VulkanRHI::cmdBindDescriptorSets(RHICommandBuffer* cmd, RHIPipelineBindPoint bindPoint,
                                        RHIPipelineLayout* layout, std::uint32_t firstSet,
                                        const std::vector<RHIDescriptorSet*>& sets,
                                        const std::vector<std::uint32_t>& dynamicOffsets) {
    auto vkCmd = static_cast<VulkanCommandBuffer*>(cmd)->GetRaw();
    auto vkLayout = static_cast<VulkanPipelineLayout*>(layout)->GetRaw();
    std::vector<vk::DescriptorSet> vkSets;
    vkSets.reserve(sets.size());
    for (auto* s : sets) {
        vkSets.push_back(static_cast<VulkanDescriptorSet*>(s)->GetRaw());
    }
    vkCmd.bindDescriptorSets(ToVk(bindPoint), vkLayout, firstSet, vkSets, dynamicOffsets);
}

void VulkanRHI::cmdDraw(RHICommandBuffer* cmd, std::uint32_t vertexCount,
                          std::uint32_t instanceCount, std::uint32_t firstVertex,
                          std::uint32_t firstInstance) {
    auto vkCmd = static_cast<VulkanCommandBuffer*>(cmd)->GetRaw();
    vkCmd.draw(vertexCount, instanceCount, firstVertex, firstInstance);
}

void VulkanRHI::cmdDrawIndexed(RHICommandBuffer* cmd, std::uint32_t indexCount,
                                 std::uint32_t instanceCount, std::uint32_t firstIndex,
                                 std::int32_t vertexOffset, std::uint32_t firstInstance) {
    auto vkCmd = static_cast<VulkanCommandBuffer*>(cmd)->GetRaw();
    vkCmd.drawIndexed(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void VulkanRHI::cmdDispatch(RHICommandBuffer* cmd, std::uint32_t groupCountX,
                              std::uint32_t groupCountY, std::uint32_t groupCountZ) {
    auto vkCmd = static_cast<VulkanCommandBuffer*>(cmd)->GetRaw();
    vkCmd.dispatch(groupCountX, groupCountY, groupCountZ);
}

void VulkanRHI::cmdSetViewport(RHICommandBuffer* cmd, std::uint32_t firstViewport,
                                 const std::vector<RHIViewport>& viewports) {
    auto vkCmd = static_cast<VulkanCommandBuffer*>(cmd)->GetRaw();
    std::vector<vk::Viewport> vkVPs;
    vkVPs.reserve(viewports.size());
    for (auto& vp : viewports) {
        vk::Viewport vkVP{};
        vkVP.setX(vp.x).setY(vp.y).setWidth(vp.width).setHeight(vp.height)
            .setMinDepth(vp.minDepth).setMaxDepth(vp.maxDepth);
        vkVPs.push_back(vkVP);
    }
    vkCmd.setViewport(firstViewport, vkVPs);
}

void VulkanRHI::cmdSetScissor(RHICommandBuffer* cmd, std::uint32_t firstScissor,
                                const std::vector<RHIRect2D>& scissors) {
    auto vkCmd = static_cast<VulkanCommandBuffer*>(cmd)->GetRaw();
    std::vector<vk::Rect2D> vkScs;
    vkScs.reserve(scissors.size());
    for (auto& s : scissors) {
        vk::Rect2D vkSc{};
        vkSc.setOffset({s.offset.x, s.offset.y})
            .setExtent({s.extent.width, s.extent.height});
        vkScs.push_back(vkSc);
    }
    vkCmd.setScissor(firstScissor, vkScs);
}

void VulkanRHI::cmdPipelineBarrier(RHICommandBuffer* cmd, RHIPipelineStageFlags srcStageMask,
                                     RHIPipelineStageFlags dstStageMask,
                                     RHIDependencyFlags dependencyFlags,
                                     const std::vector<RHIMemoryBarrier>& memoryBarriers,
                                     const std::vector<RHIBufferMemoryBarrier>& bufferBarriers,
                                     const std::vector<RHIImageMemoryBarrier>& imageBarriers) {
    auto vkCmd = static_cast<VulkanCommandBuffer*>(cmd)->GetRaw();

    std::vector<vk::MemoryBarrier> vkMem;
    for (auto& b : memoryBarriers) {
        vk::MemoryBarrier mb{};
        mb.setSrcAccessMask(ToVkAccess(b.srcAccessMask))
          .setDstAccessMask(ToVkAccess(b.dstAccessMask));
        vkMem.push_back(mb);
    }

    std::vector<vk::BufferMemoryBarrier> vkBuf;
    for (auto& b : bufferBarriers) {
        vk::BufferMemoryBarrier bb{};
        bb.setSrcAccessMask(ToVkAccess(b.srcAccessMask))
          .setDstAccessMask(ToVkAccess(b.dstAccessMask))
          .setSrcQueueFamilyIndex(b.srcQueueFamilyIndex)
          .setDstQueueFamilyIndex(b.dstQueueFamilyIndex)
          .setBuffer(static_cast<VulkanBuffer*>(b.buffer)->GetRaw())
          .setOffset(b.offset)
          .setSize(b.size);
        vkBuf.push_back(bb);
    }

    std::vector<vk::ImageMemoryBarrier> vkImg;
    for (auto& b : imageBarriers) {
        vk::ImageMemoryBarrier ib{};
        ib.setSrcAccessMask(ToVkAccess(b.srcAccessMask))
          .setDstAccessMask(ToVkAccess(b.dstAccessMask))
          .setOldLayout(ToVk(b.oldLayout))
          .setNewLayout(ToVk(b.newLayout))
          .setSrcQueueFamilyIndex(b.srcQueueFamilyIndex)
          .setDstQueueFamilyIndex(b.dstQueueFamilyIndex)
          .setImage(static_cast<VulkanImage*>(b.image)->GetRaw())
          .setSubresourceRange({
              ToVkImgAspect(b.subresourceRange.aspectMask),
              b.subresourceRange.baseMipLevel,
              b.subresourceRange.levelCount,
              b.subresourceRange.baseArrayLayer,
              b.subresourceRange.layerCount});
        vkImg.push_back(ib);
    }

    vkCmd.pipelineBarrier(ToVkPipeStage(srcStageMask), ToVkPipeStage(dstStageMask),
                          ToVkDep(dependencyFlags), vkMem, vkBuf, vkImg);
}

void VulkanRHI::cmdCopyBuffer(RHICommandBuffer* cmd, RHIBuffer* srcBuffer,
                                RHIBuffer* dstBuffer, const std::vector<RHIBufferCopy>& regions) {
    auto vkCmd = static_cast<VulkanCommandBuffer*>(cmd)->GetRaw();
    auto vkSrc = static_cast<VulkanBuffer*>(srcBuffer)->GetRaw();
    auto vkDst = static_cast<VulkanBuffer*>(dstBuffer)->GetRaw();

    std::vector<vk::BufferCopy> vkRegions;
    vkRegions.reserve(regions.size());
    for (auto& r : regions) {
        vk::BufferCopy bc{};
        bc.setSrcOffset(r.srcOffset).setDstOffset(r.dstOffset).setSize(r.size);
        vkRegions.push_back(bc);
    }
    vkCmd.copyBuffer(vkSrc, vkDst, vkRegions);
}

void VulkanRHI::cmdCopyImageToBuffer(RHICommandBuffer* cmd, RHIImage* srcImage,
                                       RHIImageLayout srcImageLayout, RHIBuffer* dstBuffer,
                                       const std::vector<RHIBufferImageCopy>& regions) {
    auto vkCmd = static_cast<VulkanCommandBuffer*>(cmd)->GetRaw();
    auto vkImg = static_cast<VulkanImage*>(srcImage)->GetRaw();
    auto vkBuf = static_cast<VulkanBuffer*>(dstBuffer)->GetRaw();

    std::vector<vk::BufferImageCopy> vkRegions;
    vkRegions.reserve(regions.size());
    for (auto& r : regions) {
        vk::BufferImageCopy bic{};
        bic.setBufferOffset(r.bufferOffset)
           .setBufferRowLength(r.bufferRowLength)
           .setBufferImageHeight(r.bufferImageHeight)
           .setImageSubresource({
               ToVkImgAspect(r.imageSubresource.aspectMask),
               r.imageSubresource.mipLevel,
               r.imageSubresource.baseArrayLayer,
               r.imageSubresource.layerCount})
           .setImageOffset({r.imageOffset.x, r.imageOffset.y, r.imageOffset.z})
           .setImageExtent({r.imageExtent.width, r.imageExtent.height, r.imageExtent.depth});
        vkRegions.push_back(bic);
    }
    vkCmd.copyImageToBuffer(vkImg, ToVk(srcImageLayout), vkBuf, vkRegions);
}

void VulkanRHI::cmdPushConstants(RHICommandBuffer* cmd, RHIPipelineLayout* layout,
                                   RHIShaderStageFlags stageFlags, std::uint32_t offset,
                                   std::uint32_t size, const void* data) {
    auto vkCmd = static_cast<VulkanCommandBuffer*>(cmd)->GetRaw();
    auto vkLayout = static_cast<VulkanPipelineLayout*>(layout)->GetRaw();
    vkCmd.pushConstants(vkLayout, ToVkShaderStage(stageFlags), offset, size, data);
}

void VulkanRHI::cmdBeginRendering(RHICommandBuffer* cmd,
                                    const RHIRenderingInfo& renderingInfo) {
    auto vkCmd = static_cast<VulkanCommandBuffer*>(cmd)->GetRaw();

    vk::RenderingInfo vkRI{};
    vkRI.setRenderArea({
            {renderingInfo.renderArea.offset.x, renderingInfo.renderArea.offset.y},
            {renderingInfo.renderArea.extent.width, renderingInfo.renderArea.extent.height}})
        .setLayerCount(renderingInfo.layerCount);

    std::vector<vk::RenderingAttachmentInfo> vkColorAtts;
    vkColorAtts.reserve(renderingInfo.colorAttachments.size());
    for (auto& ca : renderingInfo.colorAttachments) {
        vk::RenderingAttachmentInfo vkAtt{};
        {
            vk::ClearValue cv;
            cv.color.float32[0] = ca.clearValue.color.float32[0];
            cv.color.float32[1] = ca.clearValue.color.float32[1];
            cv.color.float32[2] = ca.clearValue.color.float32[2];
            cv.color.float32[3] = ca.clearValue.color.float32[3];
            vkAtt.setImageView(static_cast<VulkanImageView*>(ca.imageView)->GetRaw())
                 .setImageLayout(ToVk(ca.imageLayout))
                 .setLoadOp(ToVk(ca.loadOp))
                 .setStoreOp(ToVk(ca.storeOp))
                 .setClearValue(cv);
        }
        vkColorAtts.push_back(vkAtt);
    }
    vkRI.setColorAttachments(vkColorAtts);

    vk::RenderingAttachmentInfo vkDepthAtt, vkStencilAtt;
    if (renderingInfo.pDepthAttachment) {
        auto& da = *renderingInfo.pDepthAttachment;
        vk::ClearValue depthCV;
        depthCV.depthStencil.depth = da.clearValue.depthStencil.depth;
        depthCV.depthStencil.stencil = da.clearValue.depthStencil.stencil;
        vkDepthAtt.setImageView(static_cast<VulkanImageView*>(da.imageView)->GetRaw())
                   .setImageLayout(ToVk(da.imageLayout))
                   .setLoadOp(ToVk(da.loadOp))
                   .setStoreOp(ToVk(da.storeOp))
                   .setClearValue(depthCV);
        vkRI.setPDepthAttachment(&vkDepthAtt);
    }

    if (renderingInfo.pStencilAttachment) {
        auto& sa = *renderingInfo.pStencilAttachment;
        vkStencilAtt.setImageView(static_cast<VulkanImageView*>(sa.imageView)->GetRaw())
                     .setImageLayout(ToVk(sa.imageLayout))
                     .setLoadOp(ToVk(sa.loadOp))
                     .setStoreOp(ToVk(sa.storeOp));
        vkRI.setPStencilAttachment(&vkStencilAtt);
    }

    vkCmd.beginRendering(vkRI);
}

void VulkanRHI::cmdEndRendering(RHICommandBuffer* cmd) {
    auto vkCmd = static_cast<VulkanCommandBuffer*>(cmd)->GetRaw();
    vkCmd.endRendering();
}

// ============================================================================
// Queue
// ============================================================================

bool VulkanRHI::queueSubmit(RHIQueue* queue, const std::vector<RHISubmitInfo>& submits,
                              RHIFence* fence) {
    auto vkQueue = static_cast<VulkanQueue*>(queue)->GetRaw();
    std::vector<vk::SubmitInfo> vkSubmits;
    vkSubmits.reserve(submits.size());

    // We need temporary storage for the converted vectors
    for (auto& s : submits) {
        std::vector<vk::Semaphore> vkWaitSems;
        for (auto* ws : s.waitSemaphores)
            vkWaitSems.push_back(static_cast<VulkanSemaphore*>(ws)->GetRaw());

        std::vector<vk::PipelineStageFlags> vkWaitMasks;
        for (auto m : s.waitDstStageMask)
            vkWaitMasks.push_back(ToVkPipeStage(m));

        std::vector<vk::CommandBuffer> vkCmdBufs;
        for (auto* cb : s.commandBuffers)
            vkCmdBufs.push_back(static_cast<VulkanCommandBuffer*>(cb)->GetRaw());

        std::vector<vk::Semaphore> vkSignalSems;
        for (auto* ss : s.signalSemaphores)
            vkSignalSems.push_back(static_cast<VulkanSemaphore*>(ss)->GetRaw());

        vk::SubmitInfo vkSI{};
        vkSI.setWaitSemaphores(vkWaitSems)
            .setWaitDstStageMask(vkWaitMasks)
            .setCommandBuffers(vkCmdBufs)
            .setSignalSemaphores(vkSignalSems);
        vkSubmits.push_back(vkSI);
    }

    vk::Fence vkFence = fence ? static_cast<VulkanFence*>(fence)->GetRaw() : nullptr;
    vkQueue.submit(vkSubmits, vkFence);
    return true;
}

bool VulkanRHI::queueWaitIdle(RHIQueue* queue) {
    auto vkQueue = static_cast<VulkanQueue*>(queue)->GetRaw();
    vkQueue.waitIdle();
    return true;
}

// ============================================================================
// Memory
// ============================================================================

bool VulkanRHI::mapMemory(RHIDeviceMemory* /*memory*/, RHIDeviceSize /*offset*/,
                            RHIDeviceSize /*size*/, void** /*ppData*/) {
    return false; // TODO
}

void VulkanRHI::unmapMemory(RHIDeviceMemory* /*memory*/) {
    // TODO
}

// ============================================================================
// Query
// ============================================================================

void VulkanRHI::getPhysicalDeviceProperties(RHIPhysicalDeviceProperties& outProps) {
    auto& pd = GetPhysicalDevice();
    auto props = pd.getProperties();
    outProps.apiVersion = props.apiVersion;
    outProps.driverVersion = props.driverVersion;
    outProps.vendorID = props.vendorID;
    outProps.deviceID = props.deviceID;
    outProps.deviceType = static_cast<RHIPhysicalDeviceType>(props.deviceType);
    std::strncpy(outProps.deviceName, props.deviceName, RHI_MAX_PHYSICAL_DEVICE_NAME_SIZE - 1);
    outProps.limits.maxImageDimension2D = props.limits.maxImageDimension2D;
    outProps.limits.maxSamplerAnisotropy = props.limits.maxSamplerAnisotropy;
    outProps.limits.maxColorAttachments = props.limits.maxColorAttachments;
}

RHICommandBuffer* VulkanRHI::getCurrentCommandBuffer() const {
    if (m_currentFrame >= m_commandBuffers.size()) return nullptr;
    m_currentCmdWrapper.Set(*m_commandBuffers[m_currentFrame]);
    return &m_currentCmdWrapper;
}

RHICommandPool* VulkanRHI::getCommandPool() const {
    return nullptr; // TODO: expose internal pool safely
}

RHIQueue* VulkanRHI::getGraphicsQueue() const {
    m_queueWrapper.Set(*m_context->GetGraphicsQueue());
    return &m_queueWrapper;
}

RHIQueue* VulkanRHI::getComputeQueue() const {
    m_queueWrapper.Set(*m_context->GetGraphicsQueue());
    return &m_queueWrapper;
}

QueueFamilyIndices VulkanRHI::getQueueFamilyIndices() const {
    return m_context->GetQueueFamilyIndices();
}

RHISwapChainDesc VulkanRHI::getSwapchainInfo() {
    RHISwapChainDesc desc;
    desc.extent = {m_swapChain->GetExtent().width, m_swapChain->GetExtent().height};
    desc.imageFormat = static_cast<RHIFormat>(m_swapChain->GetImageFormat());
    desc.viewport = {0.0f, 0.0f,
                     static_cast<float>(m_swapChain->GetExtent().width),
                     static_cast<float>(m_swapChain->GetExtent().height),
                     0.0f, 1.0f};
    desc.scissor.offset = {0, 0};
    desc.scissor.extent = {m_swapChain->GetExtent().width, m_swapChain->GetExtent().height};
    return desc;
}

RHIDepthImageDesc VulkanRHI::getDepthImageInfo() const {
    RHIDepthImageDesc desc;
    return desc;
}

std::uint32_t VulkanRHI::getMaxFramesInFlight() const {
    return kMaxFramesInFlight;
}

std::uint32_t VulkanRHI::getCurrentFrameIndex() const {
    return m_currentFrame;
}

void VulkanRHI::setCurrentFrameIndex(std::uint32_t index) {
    m_currentFrame = index;
}

// ============================================================================
// Frame 辅助
// ============================================================================

RHICommandBuffer* VulkanRHI::beginSingleTimeCommands() {
    auto& device = GetDevice();

    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.setCommandPool(*m_rhiCommandPool)
             .setLevel(vk::CommandBufferLevel::ePrimary)
             .setCommandBufferCount(1);

    auto vkBuffers = vk::raii::CommandBuffers(device, allocInfo);
    auto& vkBuf = vkBuffers[0];

    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    vkBuf.begin(beginInfo);

    auto* wrapper = new VulkanCommandBuffer();
    wrapper->Set(*vkBuf);
    vkBuf.release();

    return wrapper;
}

void VulkanRHI::endSingleTimeCommands(RHICommandBuffer* cmd) {
    auto& device = GetDevice();
    auto vkCmd = static_cast<VulkanCommandBuffer*>(cmd)->GetRaw();
    vkCmd.end();

    vk::SubmitInfo submitInfo{};
    submitInfo.setCommandBuffers(vkCmd);

    auto gfxQueue = *m_context->GetGraphicsQueue();
    gfxQueue.submit(submitInfo, nullptr);
    gfxQueue.waitIdle();

    // Command buffer freed by pool RAII on destroy/reset
    delete static_cast<VulkanCommandBuffer*>(cmd);
}

void VulkanRHI::RecreateSyncObjects() {
    auto& device = GetDevice();
    std::uint32_t imageCount = m_swapChain->GetImageCount();

    m_imageAvailableSemaphores.clear();
    m_renderFinishedSemaphores.clear();
    m_inFlightFences.clear();

    vk::SemaphoreCreateInfo semCI{};
    for (std::uint32_t i = 0; i < imageCount; ++i) {
        m_imageAvailableSemaphores.emplace_back(device, semCI);
        m_renderFinishedSemaphores.emplace_back(device, semCI);
    }

    vk::FenceCreateInfo fenceCI{};
    fenceCI.setFlags(vk::FenceCreateFlagBits::eSignaled);
    for (std::uint32_t i = 0; i < kMaxFramesInFlight; ++i) {
        m_inFlightFences.emplace_back(device, fenceCI);
    }
}

bool VulkanRHI::prepareBeforePass() {
    if (!m_initialized) return false;

    auto& device = GetDevice();

    // 窗口最小化 / swapchain 不可用时跳过，等恢复后重建
    if (m_swapChain->GetImageCount() == 0) {
        if (!m_swapChain->Recreate()) return false;
        RecreateSyncObjects();
        return false;
    }

    // 1. 等待上一帧的 fence
    vk::Result result = device.waitForFences(
        *m_inFlightFences[m_currentFrame], vk::True,
        std::numeric_limits<std::uint64_t>::max());
    if (result != vk::Result::eSuccess) {
        Log::Error("[VulkanRHI] waitForFences failed: {}", vk::to_string(result));
        return false;
    }
    device.resetFences(*m_inFlightFences[m_currentFrame]);

    // 2. 从交换链获取下一个图像
    // 使用有限超时（1 秒）避免 UINT64_MAX 的 forward-progress 违规
    std::uint32_t semaphoreIndex = m_currentFrame % m_swapChain->GetImageCount();
    try {
        auto [acquireResult, index] = m_swapChain->GetSwapChain().acquireNextImage(
            1'000'000'000,  // 1 秒超时
            *m_imageAvailableSemaphores[semaphoreIndex]);

        if (acquireResult == vk::Result::eSuboptimalKHR) {
            // 图像已获取且有效，继续渲染，present 后再重建
            m_imageIndex = index;
            m_framebufferResized = true;  // 标记需要重建
        } else if (acquireResult == vk::Result::eErrorOutOfDateKHR) {
            // 图像无效，必须重建
            m_swapChain->Recreate();
            RecreateSyncObjects();
            return false;
        } else {
            m_imageIndex = index;
        }
    } catch (const vk::OutOfDateKHRError&) {
        m_swapChain->Recreate();
        RecreateSyncObjects();
        return false;
    }

    // 3. 重置并开始当前帧的命令缓冲
    m_commandBuffers[m_currentFrame].reset();
    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    m_commandBuffers[m_currentFrame].begin(beginInfo);

    // 4. 图像布局转换: Undefined → ColorAttachmentOptimal
    vk::ImageMemoryBarrier2 toColor{};
    toColor.setSrcStageMask(vk::PipelineStageFlagBits2::eTopOfPipe)
           .setSrcAccessMask({})
           .setDstStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput)
           .setDstAccessMask(vk::AccessFlagBits2::eColorAttachmentWrite)
           .setOldLayout(vk::ImageLayout::eUndefined)
           .setNewLayout(vk::ImageLayout::eColorAttachmentOptimal)
           .setImage(m_swapChain->GetImages()[m_imageIndex])
           .setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});

    vk::DependencyInfo dep{};
    dep.setImageMemoryBarrierCount(1).setPImageMemoryBarriers(&toColor);
    m_commandBuffers[m_currentFrame].pipelineBarrier2(dep);

    return true;
}

void VulkanRHI::submitRendering() {
    auto& cmd = m_commandBuffers[m_currentFrame];

    // 1. 图像布局转换: ColorAttachmentOptimal → PresentSrc
    vk::ImageMemoryBarrier2 toPresent{};
    toPresent.setSrcStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput)
             .setSrcAccessMask(vk::AccessFlagBits2::eColorAttachmentWrite)
             .setDstStageMask(vk::PipelineStageFlagBits2::eBottomOfPipe)
             .setDstAccessMask({})
             .setOldLayout(vk::ImageLayout::eColorAttachmentOptimal)
             .setNewLayout(vk::ImageLayout::ePresentSrcKHR)
             .setImage(m_swapChain->GetImages()[m_imageIndex])
             .setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});

    vk::DependencyInfo dep{};
    dep.setImageMemoryBarrierCount(1).setPImageMemoryBarriers(&toPresent);
    cmd.pipelineBarrier2(dep);

    // 2. 结束命令缓冲
    cmd.end();

    // 3. Submit (Sync2)
    std::uint32_t semaphoreIndex = m_currentFrame % m_swapChain->GetImageCount();

    vk::SemaphoreSubmitInfo waitInfo{};
    waitInfo.setSemaphore(*m_imageAvailableSemaphores[semaphoreIndex])
            .setStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput);

    vk::CommandBufferSubmitInfo cmdInfo{};
    cmdInfo.setCommandBuffer(*cmd);

    vk::SemaphoreSubmitInfo signalInfo{};
    signalInfo.setSemaphore(*m_renderFinishedSemaphores[m_imageIndex])
              .setStageMask(vk::PipelineStageFlagBits2::eAllGraphics);

    vk::SubmitInfo2 submit{};
    submit.setWaitSemaphoreInfoCount(1)
          .setPWaitSemaphoreInfos(&waitInfo)
          .setCommandBufferInfoCount(1)
          .setPCommandBufferInfos(&cmdInfo)
          .setSignalSemaphoreInfoCount(1)
          .setPSignalSemaphoreInfos(&signalInfo);

    m_context->GetGraphicsQueue().submit2(submit, *m_inFlightFences[m_currentFrame]);

    // 4. Present
    vk::PresentInfoKHR present{};
    present.setWaitSemaphoreCount(1)
           .setPWaitSemaphores(&*m_renderFinishedSemaphores[m_imageIndex])
           .setSwapchainCount(1)
           .setPSwapchains(&*m_swapChain->GetSwapChain())
           .setPImageIndices(&m_imageIndex);

    try {
        auto presentResult = m_context->GetPresentQueue().presentKHR(present);
        if (presentResult == vk::Result::eSuboptimalKHR || m_framebufferResized) {
            m_swapChain->Recreate();
            RecreateSyncObjects();
            m_framebufferResized = false;
        }
    } catch (const vk::OutOfDateKHRError&) {
        m_swapChain->Recreate();
        RecreateSyncObjects();
        m_framebufferResized = false;
    }

    // 5. 前进帧索引
    m_currentFrame = (m_currentFrame + 1) % kMaxFramesInFlight;
}

} // namespace Crumb::Render
