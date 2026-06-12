import crumb;
import std;

// ============================================================================
// Sandbox — 使用 Application 双 tick 架构的集成测试
// ============================================================================
class Sandbox : public Crumb::Application {
public:
    using Crumb::Application::Application;

    void OnInit() override {
    }

    void OnInput(float dt) override {
        if (Crumb::Input::IsKeyPressed(Key::Escape)) {
            Quit();
        }
    }

    void OnUpdate(float dt) override {
    }

    void OnRender(float dt) override {
        
    }

    void OnShutdown() override {
        Crumb::Log::Info("Sandbox shutdown.");
    }
};

auto CreateApplication() {
    Crumb::WindowCreateInfo wci {
        .title  = "Crumb Sandbox",
        .width  = 1280,
        .height = 720
    };
    return std::make_unique<Sandbox>(wci);
}
