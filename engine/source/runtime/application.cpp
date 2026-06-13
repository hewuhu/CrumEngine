module crumb.runtime;
import :application;

import crumb.core;
import crumb.platform;
import crumb.resource;

namespace Crumb {

Application::Application(const WindowCreateInfo& wci) {
    Log::Init();
    m_Window = std::make_unique<Window>(wci);

    Input::Initialize();

    m_Window->SetEventCallback([this](Event& e) -> void {
        EventDispatcher d(e);
        d.Dispatch<WindowCloseEvent>([this](auto&) {
            Quit();
            return true;
        });
    });
}

Application::~Application() = default;

float Application::GetFPS() const {
    return m_Time.FPS();
}

void Application::Run() {
    Log::Info("Application starting...");

    // -- 1. 加载引擎配置 --
    m_config.initialize("assets/config.ini");
    m_assets.setRootFolder(m_config.getRootFolder());
    Log::Info("[App] Root folder: {}", m_config.getRootFolder().generic_string());
    Log::Info("[App] Asset folder: {}", m_config.getAssetFolder().generic_string());

    // -- 2. 引擎启动（顺序初始化 VKContext → SwapChain → RHI） --
    if (!m_engine.startup(*m_Window)) {
        Log::Error("Engine startup failed.");
        return;
    }

    m_Window->Show();
    OnInit();

    // 主循环
    while (m_Running) {
        float deltaTime = m_Time.Tick();

        // 逻辑
        Input::NewFrame();
        m_Window->PollEvents();
        OnInput(deltaTime);
        OnUpdate(deltaTime);

        // 窗口最小化时跳过渲染，避免 swapchain 0x0 崩溃
        if (m_Window->IsMinimized()) continue;

        // 渲染（引擎自动驱动帧循环）
        if (m_engine.beginFrame()) {
            OnRender(deltaTime);
            m_engine.renderFrame();
            m_engine.endFrame();
        }
    }

    OnShutdown();
    m_engine.shutdown();
    Log::Info("Application stopped.");
}

} // namespace Crumb
