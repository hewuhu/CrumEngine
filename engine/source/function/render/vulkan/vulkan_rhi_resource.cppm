export module crumb.render.vulkan:vulkan_rhi_resource;

import crumb.render;
import vulkan;
import vk_mem_alloc;
import std;

// ============================================================================
// Vulkan RHI Resource — Vulkan 句柄包装类
// 将 vk::raii / vma::raii 对象包装在 RHI 句柄基类中
// ============================================================================

export namespace Crumb::Render {

// ============================================================================
// 拥有型包装器 — 内嵌 vk::raii / vma::raii 成员，自动生命周期管理
// ============================================================================

// --- VulkanBuffer : RHIBuffer (VMA 管理) ---
class VulkanBuffer final : public RHIBuffer {
public:
    VulkanBuffer() = default;
    void SetResource(vma::raii::Buffer&& buf) { m_resource = std::move(buf); }
    vk::Buffer GetRaw() const noexcept { return *m_resource; }
private:
    vma::raii::Buffer m_resource = nullptr;
};

// --- VulkanImage : RHIImage (VMA 管理) ---
class VulkanImage final : public RHIImage {
public:
    VulkanImage() = default;
    void SetResource(vma::raii::Image&& img) { m_resource = std::move(img); }
    vk::Image GetRaw() const noexcept { return *m_resource; }
private:
    vma::raii::Image m_resource = nullptr;
};

// --- VulkanImageView : RHIImageView ---
class VulkanImageView final : public RHIImageView {
public:
    VulkanImageView() = default;
    VulkanImageView(vk::raii::ImageView&& view) : m_resource(std::move(view)) {}
    void SetResource(vk::raii::ImageView&& view) { m_resource = std::move(view); }
    vk::ImageView GetRaw() const noexcept { return *m_resource; }
private:
    vk::raii::ImageView m_resource = nullptr;
};

// --- VulkanSampler : RHISampler ---
class VulkanSampler final : public RHISampler {
public:
    VulkanSampler() = default;
    VulkanSampler(vk::raii::Sampler&& sampler) : m_resource(std::move(sampler)) {}
    void SetResource(vk::raii::Sampler&& sampler) { m_resource = std::move(sampler); }
    vk::Sampler GetRaw() const noexcept { return *m_resource; }
private:
    vk::raii::Sampler m_resource = nullptr;
};

// --- VulkanShader : RHIShader ---
class VulkanShader final : public RHIShader {
public:
    VulkanShader() = default;
    VulkanShader(vk::raii::ShaderModule&& shader) : m_resource(std::move(shader)) {}
    void SetResource(vk::raii::ShaderModule&& shader) { m_resource = std::move(shader); }
    vk::ShaderModule GetRaw() const noexcept { return *m_resource; }
private:
    vk::raii::ShaderModule m_resource = nullptr;
};

// --- VulkanFence : RHIFence ---
class VulkanFence final : public RHIFence {
public:
    VulkanFence() = default;
    VulkanFence(vk::raii::Fence&& fence) : m_resource(std::move(fence)) {}
    void SetResource(vk::raii::Fence&& fence) { m_resource = std::move(fence); }
    vk::Fence GetRaw() const noexcept { return *m_resource; }
private:
    vk::raii::Fence m_resource = nullptr;
};

// --- VulkanSemaphore : RHISemaphore ---
class VulkanSemaphore final : public RHISemaphore {
public:
    VulkanSemaphore() = default;
    VulkanSemaphore(vk::raii::Semaphore&& sem) : m_resource(std::move(sem)) {}
    void SetResource(vk::raii::Semaphore&& sem) { m_resource = std::move(sem); }
    vk::Semaphore GetRaw() const noexcept { return *m_resource; }
private:
    vk::raii::Semaphore m_resource = nullptr;
};

// --- VulkanPipeline : RHIPipeline ---
class VulkanPipeline final : public RHIPipeline {
public:
    VulkanPipeline() = default;
    VulkanPipeline(vk::raii::Pipeline&& pipeline) : m_resource(std::move(pipeline)) {}
    void SetResource(vk::raii::Pipeline&& pipeline) { m_resource = std::move(pipeline); }
    vk::Pipeline GetRaw() const noexcept { return *m_resource; }
private:
    vk::raii::Pipeline m_resource = nullptr;
};

// --- VulkanPipelineLayout : RHIPipelineLayout ---
class VulkanPipelineLayout final : public RHIPipelineLayout {
public:
    VulkanPipelineLayout() = default;
    VulkanPipelineLayout(vk::raii::PipelineLayout&& layout) : m_resource(std::move(layout)) {}
    void SetResource(vk::raii::PipelineLayout&& layout) { m_resource = std::move(layout); }
    vk::PipelineLayout GetRaw() const noexcept { return *m_resource; }
private:
    vk::raii::PipelineLayout m_resource = nullptr;
};

// --- VulkanDescriptorSetLayout : RHIDescriptorSetLayout ---
class VulkanDescriptorSetLayout final : public RHIDescriptorSetLayout {
public:
    VulkanDescriptorSetLayout() = default;
    VulkanDescriptorSetLayout(vk::raii::DescriptorSetLayout&& layout)
        : m_resource(std::move(layout)) {}
    void SetResource(vk::raii::DescriptorSetLayout&& layout) { m_resource = std::move(layout); }
    vk::DescriptorSetLayout GetRaw() const noexcept { return *m_resource; }
private:
    vk::raii::DescriptorSetLayout m_resource = nullptr;
};

// --- VulkanDescriptorPool : RHIDescriptorPool ---
class VulkanDescriptorPool final : public RHIDescriptorPool {
public:
    VulkanDescriptorPool() = default;
    VulkanDescriptorPool(vk::raii::DescriptorPool&& pool) : m_resource(std::move(pool)) {}
    void SetResource(vk::raii::DescriptorPool&& pool) { m_resource = std::move(pool); }
    vk::DescriptorPool GetRaw() const noexcept { return *m_resource; }
private:
    vk::raii::DescriptorPool m_resource = nullptr;
};

// --- VulkanRenderPass : RHIRenderPass ---
class VulkanRenderPass final : public RHIRenderPass {
public:
    VulkanRenderPass() = default;
    VulkanRenderPass(vk::raii::RenderPass&& rp) : m_resource(std::move(rp)) {}
    void SetResource(vk::raii::RenderPass&& rp) { m_resource = std::move(rp); }
    vk::RenderPass GetRaw() const noexcept { return *m_resource; }
private:
    vk::raii::RenderPass m_resource = nullptr;
};

// --- VulkanFramebuffer : RHIFramebuffer ---
class VulkanFramebuffer final : public RHIFramebuffer {
public:
    VulkanFramebuffer() = default;
    VulkanFramebuffer(vk::raii::Framebuffer&& fb) : m_resource(std::move(fb)) {}
    void SetResource(vk::raii::Framebuffer&& fb) { m_resource = std::move(fb); }
    vk::Framebuffer GetRaw() const noexcept { return *m_resource; }
private:
    vk::raii::Framebuffer m_resource = nullptr;
};

// --- VulkanDeviceMemory : RHIDeviceMemory ---
class VulkanDeviceMemory final : public RHIDeviceMemory {
public:
    VulkanDeviceMemory() = default;
    VulkanDeviceMemory(vk::raii::DeviceMemory&& mem) : m_resource(std::move(mem)) {}
    void SetResource(vk::raii::DeviceMemory&& mem) { m_resource = std::move(mem); }
    vk::DeviceMemory GetRaw() const noexcept { return *m_resource; }
private:
    vk::raii::DeviceMemory m_resource = nullptr;
};

// ============================================================================
// 非拥有型包装器 — 存储原始 Vulkan 句柄（池管理生命周期）
// ============================================================================

// --- VulkanCommandBuffer : RHICommandBuffer ---
class VulkanCommandBuffer final : public RHICommandBuffer {
public:
    void Set(vk::CommandBuffer cb) noexcept { m_cmdBuffer = cb; }
    vk::CommandBuffer GetRaw() const noexcept { return m_cmdBuffer; }
private:
    vk::CommandBuffer m_cmdBuffer = nullptr;
};

// --- VulkanDescriptorSet : RHIDescriptorSet ---
class VulkanDescriptorSet final : public RHIDescriptorSet {
public:
    void Set(vk::DescriptorSet ds) noexcept { m_descSet = ds; }
    vk::DescriptorSet GetRaw() const noexcept { return m_descSet; }
private:
    vk::DescriptorSet m_descSet = nullptr;
};

// --- VulkanCommandPool : RHICommandPool ---
class VulkanCommandPool final : public RHICommandPool {
public:
    VulkanCommandPool() = default;
    VulkanCommandPool(vk::raii::CommandPool&& pool) : m_resource(std::move(pool)) {}
    void SetResource(vk::raii::CommandPool&& pool) { m_resource = std::move(pool); }
    vk::CommandPool GetRaw() const noexcept { return *m_resource; }
private:
    vk::raii::CommandPool m_resource = nullptr;
};

// --- VulkanQueue : RHIQueue ---
class VulkanQueue final : public RHIQueue {
public:
    void Set(vk::Queue q) noexcept { m_queue = q; }
    vk::Queue GetRaw() const noexcept { return m_queue; }
private:
    vk::Queue m_queue = nullptr;
};

} // namespace Crumb::Render
