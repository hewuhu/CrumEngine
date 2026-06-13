export module crumb.render:render_system;

import :rhi_types;
import :rhi_struct;
import crumb.core;
import std;

// ============================================================================
// RenderSystem — 高层渲染抽象（后端无关）
// Sandbox 只通过此接口操作渲染，不碰任何 Vulkan/D3D12 具体类型
// ============================================================================

export namespace Crumb::Render {

using TextureID = std::uint32_t;
using SpriteID  = std::uint32_t;

constexpr TextureID kInvalidTexture = ~0u;
constexpr SpriteID  kInvalidSprite  = ~0u;

class RenderSystem {
public:
    virtual ~RenderSystem() = default;

    // --- 纹理 ---
    virtual TextureID loadTexture(const std::string& path) = 0;
    virtual TextureID createTexture(std::uint32_t width, std::uint32_t height,
                                    const void* pixels) = 0;
    virtual void getTextureSize(TextureID id, std::uint32_t& w, std::uint32_t& h) const = 0;

    // --- 精灵 ---
    virtual SpriteID createSprite(TextureID tex,
                                   const Vector2& size = {64, 64}) = 0;
    virtual void updateSprite(SpriteID id,
                               const Matrix4x4& transform,
                               const Vector2& size,
                               const Color& tint = {1,1,1,1}) = 0;
    virtual void setSpriteTexture(SpriteID id, TextureID tex) = 0;

    // --- 每帧渲染（引擎调用） ---
    virtual void beginFrame(RHICommandBuffer* cmd, std::uint32_t frameWidth, std::uint32_t frameHeight) = 0;
    virtual void render(RHICommandBuffer* cmd) = 0;
    virtual void endFrame() = 0;
};

} // namespace Crumb::Render
