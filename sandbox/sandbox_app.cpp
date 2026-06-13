import crumb;
import std;

class Sandbox : public Crumb::Application {
public:
    using Crumb::Application::Application;

    Crumb::Render::TextureID m_tex   = Crumb::Render::kInvalidTexture;
    Crumb::Render::SpriteID  m_sprite = Crumb::Render::kInvalidSprite;
    Crumb::Vector2 m_pos{100, 100};
    float m_speed = 500.0f;

    void OnInit() override {
        auto& rs = m_engine.renderSystem;

        // -- 验证反射 + AssetManager 链路 --
        Crumb::AppConfig cfg;
        if (m_assets.loadAsset("assets/app_config.json", cfg)) {
            Crumb::Log::Info("[Sandbox] Loaded app_config.json via reflection:");
            Crumb::Log::Info("  rootFolder: {}", cfg.rootFolder);
            Crumb::Log::Info("  window: {}x{}", cfg.windowWidth, cfg.windowHeight);
        } else {
            // 配置文件不存在，创建一个默认的并保存
            cfg.rootFolder = ".";
            cfg.assetFolder = "assets";
            cfg.windowWidth = 1280;
            cfg.windowHeight = 720;
            if (m_assets.saveAsset(cfg, "assets/app_config.json")) {
                Crumb::Log::Info("[Sandbox] Created app_config.json (reflection test OK)");
            }
        }

        // -- 通过 resource 层加载纹理（stb → CPU → Vulkan staging → GPU） --
        m_tex = rs->loadTexture("assets/player.png");
        if (m_tex == Crumb::Render::kInvalidTexture) {
            Crumb::Log::Warn("No player.png found, creating blank placeholder");
            m_tex = rs->createTexture(64, 64, nullptr);
        }

        // -- 创建精灵 --
        m_sprite = rs->createSprite(m_tex, {64, 64});
        Crumb::Log::Info("[Sandbox] Ready — sprite {} with tex {}", m_sprite, m_tex);
    }

    void OnInput(float dt) override {
        if (Crumb::Input::IsKeyPressed(Key::Escape)) Quit();

        float step = m_speed * dt;
        if (Crumb::Input::IsKeyDown(Key::W)) m_pos.y -= step;
        if (Crumb::Input::IsKeyDown(Key::S)) m_pos.y += step;
        if (Crumb::Input::IsKeyDown(Key::A)) m_pos.x -= step;
        if (Crumb::Input::IsKeyDown(Key::D)) m_pos.x += step;
    }

    void OnUpdate(float dt) override {
        if (m_sprite == Crumb::Render::kInvalidSprite) return;

        auto& rs = m_engine.renderSystem;
        auto transform = Crumb::Matrix4x4::getTrans(m_pos.x, m_pos.y, 0);
        rs->updateSprite(m_sprite, transform, {64, 64});
    }

    void OnRender(float dt) override {
        // 引擎自动渲染所有精灵
    }

    void OnShutdown() override {
        Crumb::Log::Info("[Sandbox] Shutdown.");
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
