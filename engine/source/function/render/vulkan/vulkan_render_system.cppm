export module crumb.render.vulkan:vulkan_render_system;

import :context;
import :pipeline;
import :sprite;
import :texture;
import :vulkan_rhi_resource;
import crumb.render;
import crumb.core;
import vulkan;
import std;

// ============================================================================
// VulkanRenderSystem — RenderSystem 的 Vulkan 后端实现
// ============================================================================

export namespace Crumb::Render {

class VulkanRenderSystem final : public RenderSystem {
public:
    VulkanRenderSystem() = default;
    ~VulkanRenderSystem() override = default;

    // --- 生命周期 ---
    bool startup(VKContext& ctx, SwapChain& sc);
    void shutdown();

    // --- 纹理 ---
    TextureID loadTexture(const std::string& path) override;
    TextureID createTexture(std::uint32_t width, std::uint32_t height,
                            const void* pixels) override;
    void getTextureSize(TextureID id, std::uint32_t& w, std::uint32_t& h) const override;

    // --- 精灵 ---
    SpriteID createSprite(TextureID tex, const Vector2& size = {64, 64}) override;
    void updateSprite(SpriteID id, const Matrix4x4& transform,
                      const Vector2& size, const Color& tint = {1,1,1,1}) override;
    void setSpriteTexture(SpriteID id, TextureID tex) override;

    // --- 渲染 ---
    void beginFrame(RHICommandBuffer* cmd, std::uint32_t frameWidth, std::uint32_t frameHeight) override;
    void render(RHICommandBuffer* cmd) override;
    void endFrame() override;

private:
    VKContext*  m_ctx = nullptr;
    SwapChain*  m_swapChain = nullptr;

    static constexpr std::uint32_t kMaxSprites = 1024;
    std::unique_ptr<VKPipeline>       m_pipeline;
    std::unique_ptr<VKSpriteRenderer> m_spriteRenderer;
    std::vector<VKTexture>            m_textures;
    SpriteID m_nextSpriteID = 0;

    std::uint32_t m_frameWidth  = 0;
    std::uint32_t m_frameHeight = 0;
};

} // namespace Crumb::Render
