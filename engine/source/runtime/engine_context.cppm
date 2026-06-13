export module crumb.runtime:engine_context;

import crumb.render.vulkan;
import crumb.render;
import crumb.platform;
import std;

// ============================================================================
// EngineContext — 管理所有引擎子系统的生命周期
// ============================================================================

export namespace Crumb {

struct EngineContext {
    Render::VKContext                     vkContext;
    std::unique_ptr<Render::SwapChain>    swapChain;
    Render::VulkanRHI                     rhi;
    std::unique_ptr<Render::RenderSystem> renderSystem;

    bool startup(Window& window);
    void shutdown();

    // 帧边界
    bool beginFrame();
    void renderFrame();
    void endFrame();
};

} // namespace Crumb
