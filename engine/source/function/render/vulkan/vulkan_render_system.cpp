module crumb.render.vulkan;
import :vulkan_render_system;

import crumb.core;
import crumb.resource;
import vulkan;
import std;

namespace Crumb::Render {

// ============================================================================
// 生命周期
// ============================================================================

bool VulkanRenderSystem::startup(VKContext& ctx, SwapChain& sc) {
    m_ctx = &ctx;
    m_swapChain = &sc;

    // 1. 创建管线描述符布局
    m_pipeline = std::make_unique<VKPipeline>(*m_ctx, *m_swapChain);
    if (!m_pipeline->CreateDescriptorSetLayout()) {
        Log::Error("[VulkanRS] CreateDescriptorSetLayout failed");
        return false;
    }

    // 2. 加载着色器并创建图形管线
    auto shaderCode = ReadFileBinary("assets/shaders/texturedMesh.spv");
    if (!shaderCode.empty()) {
        auto shaderModule = m_pipeline->CreateShaderModule(shaderCode);
        if (!m_pipeline->CreateGraphicsPipeline(shaderModule)) {
            Log::Warn("[VulkanRS] CreateGraphicsPipeline failed");
        }
    } else {
        Log::Warn("[VulkanRS] No shader found at assets/shaders/texturedMesh.spv");
    }

    // 3. 创建精灵渲染器
    m_spriteRenderer = std::make_unique<VKSpriteRenderer>(*m_ctx, *m_pipeline, kMaxSprites);
    if (!m_spriteRenderer->Initialize()) {
        Log::Error("[VulkanRS] SpriteRenderer init failed");
        return false;
    }

    Log::Info("[VulkanRS] Started (max {} sprites)", kMaxSprites);
    return true;
}

void VulkanRenderSystem::shutdown() {
    m_spriteRenderer.reset();
    m_pipeline.reset();
    m_textures.clear();
    Log::Info("[VulkanRS] Shutdown");
}

// ============================================================================
// 纹理
// ============================================================================

TextureID VulkanRenderSystem::loadTexture(const std::string& path) {
    VKTexture tex;
    if (!LoadTexture2D(*m_ctx, path, tex)) {
        Log::Error("[VulkanRS] loadTexture failed: {}", path);
        return kInvalidTexture;
    }
    TextureID id = static_cast<TextureID>(m_textures.size());
    m_textures.push_back(std::move(tex));
    return id;
}

TextureID VulkanRenderSystem::createTexture(std::uint32_t width, std::uint32_t height,
                                              const void* pixels) {
    VKTexture tex;
    if (!CreateTextureRGBA8(*m_ctx, width, height, pixels, tex)) {
        Log::Error("[VulkanRS] createTexture failed");
        return kInvalidTexture;
    }
    TextureID id = static_cast<TextureID>(m_textures.size());
    m_textures.push_back(std::move(tex));
    return id;
}

void VulkanRenderSystem::getTextureSize(TextureID id, std::uint32_t& w, std::uint32_t& h) const {
    if (id >= m_textures.size()) { w = 0; h = 0; return; }
    w = m_textures[id].width;
    h = m_textures[id].height;
}

// ============================================================================
// 精灵
// ============================================================================

SpriteID VulkanRenderSystem::createSprite(TextureID tex, const Vector2& /*size*/) {
    if (tex >= m_textures.size()) return kInvalidSprite;
    SpriteID id = m_nextSpriteID++;
    if (!m_spriteRenderer->BindSprite(id, m_textures[tex])) return kInvalidSprite;
    return id;
}

void VulkanRenderSystem::updateSprite(SpriteID id, const Matrix4x4& transform,
                                       const Vector2& size, const Color& /*tint*/) {
    // 拼 model 矩阵：平移 * 缩放到指定大小（quad 顶点是 1x1）
    auto model = Matrix4x4::getTrans(transform.getTrans()) *
                 Matrix4x4::getScale({size.x, size.y, 1.0f});
    m_spriteRenderer->UpdateSprite(id, model);
}

void VulkanRenderSystem::setSpriteTexture(SpriteID id, TextureID tex) {
    if (tex >= m_textures.size()) return;
    m_spriteRenderer->BindSprite(id, m_textures[tex]);
}

// ============================================================================
// 渲染
// ============================================================================

void VulkanRenderSystem::beginFrame(RHICommandBuffer* /*cmd*/,
                                     std::uint32_t frameWidth, std::uint32_t frameHeight) {
    if (frameWidth != m_frameWidth || frameHeight != m_frameHeight) {
        m_frameWidth  = frameWidth;
        m_frameHeight = frameHeight;
        m_spriteRenderer->SetScreenSize(static_cast<float>(frameWidth),
                                         static_cast<float>(frameHeight));
    }
}

void VulkanRenderSystem::render(RHICommandBuffer* cmd) {
    auto vkCmd = static_cast<VulkanCommandBuffer*>(cmd)->GetRaw();
    m_spriteRenderer->Draw(vkCmd);
}

void VulkanRenderSystem::endFrame() {
    // no-op for now
}

} // namespace Crumb::Render
