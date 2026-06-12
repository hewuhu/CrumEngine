export module crumb.platform.window;

import std;
import crumb.platform.events;

export namespace Crumb {

struct WindowCreateInfo {
    std::string title = "Crumb Engine";
    int width = 1280;
    int height = 720;
    bool resizable = true;
    bool fullscreen = false;
    bool vsync = true;
};

class Window {
public:
    using EventCallbackFn = std::function<void(Event&)>;

    explicit Window(const WindowCreateInfo& CreateInfo = {});
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

    auto Width() const -> int;
    auto Height() const -> int;
    bool IsMinimized() const;

    void SetTitle(const std::string& title);
    void SetSize(int width, int height);
    void SetResizable(bool resizable);
    void SetFullscreen(bool fullscreen);
    void SetVSync(bool vsync);
    bool IsVSync() const { return m_vsync; }
    void ToggleFullscreen();
    void Show();
    void Hide();

    void SetEventCallback(const EventCallbackFn& callback);
    void PollEvents();

    auto GetInstanceExtensions(std::uint32_t* count) const -> const char* const*;
    auto CreateSurface(void* instance) const -> void*;

    void* GetNativeHandle() const;

private:
    void* m_handle = nullptr;
    bool m_vsync = true;
    EventCallbackFn m_callback;
};

} // namespace Crumb
