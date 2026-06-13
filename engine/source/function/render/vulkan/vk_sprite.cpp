module crumb.render.vulkan;
import :sprite;
import :texture;

import vk_mem_alloc;
import crumb.core;
import vulkan;
import std;

namespace Crumb::Render {

// 顶点
static const std::vector<SpriteVertex> kQuadVertices = {
    {.position={ 0.0F,  0.0F}, .uv={0.0F, 0.0F}},
    {.position={ 1.0F,  0.0F}, .uv={1.0F, 0.0F}},
    {.position={ 1.0F,  1.0F}, .uv={1.0F, 1.0F}},
    {.position={ 0.0F,  1.0F}, .uv={0.0F, 1.0F}},
};

static const std::vector<std::uint16_t> kQuadIndices = { 0, 1, 2, 0, 2, 3 };

static void SubmitOnce(VKContext &ctx, vk::raii::CommandBuffer &cmd,
                       vk::raii::CommandPool &pool, vk::raii::Queue &queue)
{
    cmd.end();
    vk::SubmitInfo info{};
    info.setCommandBufferCount(1).setPCommandBuffers(&*cmd);
    queue.submit(info);
    ctx.GetDevice().waitIdle();
}

VKSpriteRenderer::VKSpriteRenderer(VKContext &context, VKPipeline &pipeline, std::uint32_t maxSprites)
    : m_context(context)
    , m_pipeline(pipeline)
    , m_maxSprites(maxSprites)
{
    m_spriteData.resize(maxSprites);
    m_textures.reserve(VKPipeline::MaxTextures);
    m_texDescriptors.reserve(VKPipeline::MaxTextures);
}

bool VKSpriteRenderer::Initialize() {
    if (!CreateQuadMesh()) return false;
    if (!CreateDescriptorPool()) return false;
    if (!CreateSSBO()) return false;
    return true;
}

// ========== 四边形网格（不变）==========
bool VKSpriteRenderer::CreateQuadMesh() {
    try {
        auto &dev = m_context.GetDevice();
        auto& allocator = m_context.GetAllocator();
        vk::DeviceSize vbSize = kQuadVertices.size() * sizeof(SpriteVertex);
        vk::DeviceSize ibSize = kQuadIndices.size() * sizeof(std::uint16_t);

        vk::BufferCreateInfo stagingInfo{};
        stagingInfo.setSize(vbSize + ibSize)
                   .setUsage(vk::BufferUsageFlagBits::eTransferSrc);

        vma::AllocationCreateInfo stagingAlloc{};
        stagingAlloc.setUsage(vma::MemoryUsage::eAutoPreferHost)
                    .setFlags(vma::AllocationCreateFlagBits::eHostAccessSequentialWrite
                            | vma::AllocationCreateFlagBits::eMapped);

        vma::AllocationInfo stagingMapInfo{};
        vma::raii::Buffer staging(allocator, stagingInfo, stagingAlloc, &stagingMapInfo);

        void *ptr = stagingMapInfo.pMappedData;
        std::memcpy(ptr, kQuadVertices.data(), static_cast<std::size_t>(vbSize));
        std::memcpy(static_cast<char *>(ptr) + vbSize, kQuadIndices.data(), static_cast<std::size_t>(ibSize));

        vk::BufferCreateInfo vbInfo{};
        vbInfo.setSize(vbSize)
              .setUsage(vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer);
        vma::AllocationCreateInfo gpuAlloc{};
        gpuAlloc.setUsage(vma::MemoryUsage::eAutoPreferDevice);
        m_vertexBuffer = vma::raii::Buffer(allocator, vbInfo, gpuAlloc);

        vk::BufferCreateInfo ibInfo{};
        ibInfo.setSize(ibSize)
              .setUsage(vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer);
        m_indexBuffer = vma::raii::Buffer(allocator, ibInfo, gpuAlloc);

        vk::CommandPoolCreateInfo poolInfo{};
        poolInfo.setQueueFamilyIndex(m_context.GetQueueFamilyIndices().graphicsFamily.value());
        vk::raii::CommandPool cmdPool(dev, poolInfo);
        vk::CommandBufferAllocateInfo cmdAlloc{};
        cmdAlloc.setCommandPool(*cmdPool).setLevel(vk::CommandBufferLevel::ePrimary).setCommandBufferCount(1);
        vk::raii::CommandBuffers cmdBufs(dev, cmdAlloc);
        auto &cmd = cmdBufs[0];
        vk::CommandBufferBeginInfo beginInfo{};
        beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        cmd.begin(beginInfo);

        vk::BufferCopy vbCopy{};
        vbCopy.setSize(vbSize);
        cmd.copyBuffer(*staging, *m_vertexBuffer, vbCopy);

        vk::BufferCopy ibCopy{};
        ibCopy.setSize(ibSize).setSrcOffset(vbSize);
        cmd.copyBuffer(*staging, *m_indexBuffer, ibCopy);

        SubmitOnce(m_context, cmd, cmdPool, m_context.GetGraphicsQueue());
        return true;
    }
    catch (const std::exception &e)
    {
        Log::Error("[VK] Failed to create quad mesh: {}", e.what());
        return false;
    }
}

// ========== 描述符池 + 共享描述符集 ==========
bool VKSpriteRenderer::CreateDescriptorPool()
{
    try
    {
        auto &dev = m_context.GetDevice();

        std::array poolSizes = {
            vk::DescriptorPoolSize{vk::DescriptorType::eStorageBuffer, 1},
            vk::DescriptorPoolSize{vk::DescriptorType::eCombinedImageSampler, VKPipeline::MaxTextures}
        };

        vk::DescriptorPoolCreateInfo poolInfo{};
        poolInfo.setPoolSizeCount(static_cast<std::uint32_t>(poolSizes.size()))
                .setPPoolSizes(poolSizes.data())
                .setMaxSets(1)
                .setFlags(vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind
                       | vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);

        m_descriptorPool = vk::raii::DescriptorPool(dev, poolInfo);

        // 分配唯一的共享描述符集
        vk::DescriptorSetLayout layout = m_pipeline.GetDescriptorSetLayout();
        vk::DescriptorSetAllocateInfo setInfo{};
        setInfo.setDescriptorPool(*m_descriptorPool)
               .setDescriptorSetCount(1)
               .setPSetLayouts(&layout);
        vk::raii::DescriptorSets sets(dev, setInfo);
        m_sharedSet = std::move(sets[0]);

        return true;
    }
    catch (const std::exception &e)
    {
        Log::Error("[VK] Failed to create descriptor pool: {}", e.what());
        return false;
    }
}

// ========== SSBO ==========
bool VKSpriteRenderer::CreateSSBO() {
    try {
        auto& allocator = m_context.GetAllocator();
        vk::DeviceSize size = m_maxSprites * sizeof(SpriteData);

        vk::BufferCreateInfo ssboInfo{};
        ssboInfo.setSize(size)
                .setUsage(vk::BufferUsageFlagBits::eStorageBuffer);

        vma::AllocationCreateInfo ssboAlloc{};
        ssboAlloc.setUsage(vma::MemoryUsage::eAutoPreferHost)
                 .setFlags(vma::AllocationCreateFlagBits::eHostAccessSequentialWrite
                         | vma::AllocationCreateFlagBits::eMapped);

        vma::AllocationInfo mapInfo{};
        m_ssbo = vma::raii::Buffer(allocator, ssboInfo, ssboAlloc, &mapInfo);
        m_ssboMapped = mapInfo.pMappedData;

        // 写入 SSBO 到描述符集 (binding 0)
        vk::DescriptorBufferInfo ssboDesc{};
        ssboDesc.setBuffer(*m_ssbo).setOffset(0).setRange(size);
        vk::WriteDescriptorSet ssboWrite{};
        ssboWrite.setDstSet(*m_sharedSet).setDstBinding(0).setDescriptorCount(1)
                 .setDescriptorType(vk::DescriptorType::eStorageBuffer).setPBufferInfo(&ssboDesc);
        m_context.GetDevice().updateDescriptorSets(ssboWrite, {});

        return true;
    }
    catch (const std::exception &e)
    {
        Log::Error("[VK] Failed to create SSBO: {}", e.what());
        return false;
    }
}

// ========== 纹理注册 ==========
void VKSpriteRenderer::RegisterTexture(std::uint32_t slot, const VKTexture& tex) {
    // 复用已有纹理
    for (std::size_t i = 0; i < m_textures.size(); ++i)
    {
        if (m_textures[i] == &tex)
        {
            m_spriteData[slot].textureId = static_cast<std::uint32_t>(i);
            return;
        }
    }

    // 新纹理
    std::uint32_t texId = static_cast<std::uint32_t>(m_textures.size());
    m_textures.push_back(&tex);
    m_texDescriptors.push_back({*tex.sampler, *tex.view, vk::ImageLayout::eShaderReadOnlyOptimal});
    m_spriteData[slot].textureId = texId;

    // 更新描述符集
    vk::WriteDescriptorSet texWrite{};
    texWrite.setDstSet(*m_sharedSet).setDstBinding(1)
            .setDstArrayElement(texId).setDescriptorCount(1)
            .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
            .setPImageInfo(&m_texDescriptors.back());
    m_context.GetDevice().updateDescriptorSets(texWrite, {});
}

// ========== 绑定精灵 ==========
bool VKSpriteRenderer::BindSprite(std::uint32_t index, const VKTexture &texture) {
    if (index >= m_maxSprites) return false;
    RegisterTexture(index, texture);
    if (index >= m_activeCount) m_activeCount = index + 1;
    return true;
}

void VKSpriteRenderer::SetScreenSize(float width, float height) {
    m_orthoProj = Matrix4x4::createOrtho(0.0f, width, 0.0f, height);
}

void VKSpriteRenderer::UpdateSprite(std::uint32_t index, const Matrix4x4 &model) {
    if (index >= m_maxSprites) return;
    m_spriteData[index].mvp = m_orthoProj * model;
}

void VKSpriteRenderer::Draw(vk::CommandBuffer cmd) {
    if (m_activeCount == 0) return;

    // 整块上传到 SSBO
    vk::DeviceSize size = m_activeCount * sizeof(SpriteData);
    std::memcpy(m_ssboMapped, m_spriteData.data(), static_cast<std::size_t>(size));

    cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, *m_pipeline.GetGraphicsPipeline());
    cmd.bindVertexBuffers(0, *m_vertexBuffer, {0});
    cmd.bindIndexBuffer(*m_indexBuffer, 0, vk::IndexType::eUint16);
    cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                           *m_pipeline.GetPipelineLayout(), 0,
                           *m_sharedSet, {});
    cmd.drawIndexed(6, m_activeCount, 0, 0, 0);
}

} // namespace Crumb::Render
