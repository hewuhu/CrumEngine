module crumb.runtime;
import :application;

import crumb.core;
import crumb.platform;

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
    m_Window->Show();
    OnInit();

    // ── 主循环 ─────────────────────────────────────────────────────────
    while (m_Running) {
        float deltaTime = m_Time.Tick();

        // ── Logical tick ──────────────────────────────────────────────
        Input::NewFrame();
        m_Window->PollEvents();
        OnInput(deltaTime);
        OnUpdate(deltaTime);

        // ── Render tick ───────────────────────────────────────────────
        OnRender(deltaTime);
    }

    OnShutdown();
    Log::Info("Application stopped.");
}

} // namespace Crumb
