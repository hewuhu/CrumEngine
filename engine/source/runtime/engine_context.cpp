module crumb.runtime;
import :engine_context;

import crumb.core;
import crumb.platform;
import crumb.render;
import crumb.render.vulkan;
import std;
import vulkan;

namespace Crumb {

bool EngineContext::startup(Window& window) {
    // 1. 创建 Vulkan 设备和队列
    if (!vkContext.CreateContext(window, "Crumb Engine", true)) {
        Log::Error("[Engine] VKContext init failed");
        return false;
    }

    // 2. 创建交换链（依赖 VKContext 的 device/physicalDevice/surface）
    swapChain = std::make_unique<Render::SwapChain>(
        vkContext.GetDevice(),
        vkContext.GetPhysicalDevice(),
        vkContext.GetSurface());
    swapChain->Create();
    swapChain->CreateImageViews();

    // 3. 初始化 RHI（依赖 VKContext + SwapChain）
    rhi.AttachToContext(vkContext, *swapChain);
    Render::RHIInitInfo info;
    if (!rhi.initialize(info)) {
        Log::Error("[Engine] RHI init failed");
        return false;
    }

    // 4. 初始化 RenderSystem（依赖 VKContext + SwapChain + Pipeline）
    auto vkRS = std::make_unique<Render::VulkanRenderSystem>();
    if (!vkRS->startup(vkContext, *swapChain)) {
        Log::Error("[Engine] RenderSystem init failed");
        return false;
    }
    renderSystem = std::move(vkRS);

    Log::Info("[Engine] Context started");
    return true;
}

void EngineContext::shutdown() {
    vkContext.GetDevice().waitIdle();  // 等 GPU 完成所有工作，再销毁资源
    renderSystem.reset();
    swapChain.reset();
    Log::Info("[Engine] Context shutdown");
}

bool EngineContext::beginFrame() {
    return rhi.prepareBeforePass();
}

void EngineContext::renderFrame() {
    if (!renderSystem) return;
    auto* rhiCmd = rhi.getCurrentCommandBuffer();
    auto vkCmd  = static_cast<Render::VulkanCommandBuffer*>(rhiCmd)->GetRaw();
    auto extent = swapChain->GetExtent();

    // 1. 设置 viewport + scissor
    vk::Viewport vp{0, 0, (float)extent.width, (float)extent.height, 0, 1};
    vkCmd.setViewport(0, vp);
    vk::Rect2D scissor{{0, 0}, extent};
    vkCmd.setScissor(0, scissor);

    // 2. 开始动态渲染
    auto& swapImages = swapChain->GetImageViews();
    std::uint32_t imgIdx = rhi.getCurrentImageIndex();
    vk::ClearValue cv;
    cv.color.float32[0] = 0.0f; cv.color.float32[1] = 0.0f;
    cv.color.float32[2] = 0.0f; cv.color.float32[3] = 1.0f;
    vk::RenderingAttachmentInfo colorAtt{};
    colorAtt.setImageView(*swapImages[imgIdx])
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore)
            .setClearValue(cv);

    vk::RenderingInfo renderInfo{};
    renderInfo.setRenderArea(scissor)
              .setLayerCount(1)
              .setColorAttachments(colorAtt);
    vkCmd.beginRendering(renderInfo);

    // 3. 渲染精灵
    renderSystem->beginFrame(rhiCmd, extent.width, extent.height);
    renderSystem->render(rhiCmd);

    // 4. 结束动态渲染
    vkCmd.endRendering();
}

void EngineContext::endFrame() {
    rhi.submitRendering();
}

} // namespace Crumb
