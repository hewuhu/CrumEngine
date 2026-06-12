module crumb.platform.window;

import sdl;
import crumb;
import crumb.platform.events;
import crumb.platform.input;

namespace Crumb {

using namespace sdl;

// ============================================================================
// 构造 / 析构
// ============================================================================

Window::Window(const WindowCreateInfo& desc) {
    SDL_WindowFlags flags = WINDOW_HIDDEN;
    if (desc.resizable) flags |= WINDOW_RESIZABLE;
    if (desc.fullscreen) flags |= WINDOW_FULLSCREEN;

    m_handle = SDL_CreateWindow(desc.title.c_str(), desc.width, desc.height, flags);

    if (!m_handle) {
        Log::Error("Failed to create window: {}", SDL_GetError());
        return;
    }

    m_vsync = desc.vsync;
    Log::Info("Window created: {} ({}x{})", desc.title, desc.width, desc.height);
}

Window::~Window() {
    if (m_handle) {
        SDL_DestroyWindow(static_cast<SDL_Window*>(m_handle));
        m_handle = nullptr;
        Log::Info("Window destroyed");
    }
}

// ============================================================================
// 属性查询
// ============================================================================

int Window::Width() const {
    int w = 0, h = 0;
    SDL_GetWindowSize(static_cast<SDL_Window*>(m_handle), &w, &h);
    return w;
}

int Window::Height() const {
    int w = 0, h = 0;
    SDL_GetWindowSize(static_cast<SDL_Window*>(m_handle), &w, &h);
    return h;
}

bool Window::IsMinimized() const {
    Uint64 flags = SDL_GetWindowFlags(static_cast<SDL_Window*>(m_handle));
    return (flags & WINDOW_MINIMIZED) != 0;
}

// ============================================================================
// 窗口操作
// ============================================================================

void Window::SetTitle(const std::string& title) {
    SDL_SetWindowTitle(static_cast<SDL_Window*>(m_handle), title.c_str());
}

void Window::SetSize(int width, int height) {
    SDL_SetWindowSize(static_cast<SDL_Window*>(m_handle), width, height);
}

void Window::SetResizable(bool resizable) {
    SDL_SetWindowResizable(static_cast<SDL_Window*>(m_handle), resizable);
}

void Window::SetFullscreen(bool fullscreen) {
    SDL_SetWindowFullscreen(
        static_cast<SDL_Window*>(m_handle),
        fullscreen ? WINDOW_FULLSCREEN : 0
    );
}

void Window::SetVSync(bool vsync) {
    m_vsync = vsync;
}

void Window::ToggleFullscreen() {
    Uint64 flags = SDL_GetWindowFlags(static_cast<SDL_Window*>(m_handle));
    bool isFull = (flags & WINDOW_FULLSCREEN) != 0;
    SetFullscreen(!isFull);
}

void Window::Show() { SDL_ShowWindow(static_cast<SDL_Window*>(m_handle)); }
void Window::Hide() { SDL_HideWindow(static_cast<SDL_Window*>(m_handle)); }

void* Window::GetNativeHandle() const {
    return m_handle;
}

// ============================================================================
// Vulkan
// ============================================================================

auto Window::GetInstanceExtensions(std::uint32_t* count) const -> const char* const* {
    return SDL_Vulkan_GetInstanceExtensions(count);
}

auto Window::CreateSurface(void* instance) const -> void* {
    VkSurfaceKHR surface = nullptr;
    SDL_Window* w = static_cast<SDL_Window*>(m_handle);
    if (!SDL_Vulkan_CreateSurface(w, reinterpret_cast<VkInstance>(instance), nullptr, &surface)) {
        Log::Error("Failed to create Vulkan surface: {}", SDL_GetError());
        return nullptr;
    }
    return surface;
}

// ============================================================================
// 事件 — SDL → 引擎事件桥接 + 输入状态更新
// (Input::NewFrame() 由 Application 在主循环中调用，这里只做事件转换)
// ============================================================================

void Window::SetEventCallback(const EventCallbackFn& callback) {
    m_callback = callback;
}

void Window::PollEvents() {
    SDL_Event sdl;
    while (SDL_PollEvent(&sdl)) {
        switch (sdl.type) {

        // ---- 窗口 ---------------------------------------------------------
        case SDL_EVENT_QUIT: {
            WindowCloseEvent e;
            if (m_callback) m_callback(e);
            break;
        }
        case SDL_EVENT_WINDOW_RESIZED: {
            WindowResizeEvent e(sdl.window.data1, sdl.window.data2);
            if (m_callback) m_callback(e);
            break;
        }
        case SDL_EVENT_WINDOW_FOCUS_GAINED: {
            WindowFocusEvent e;
            if (m_callback) m_callback(e);
            break;
        }
        case SDL_EVENT_WINDOW_FOCUS_LOST: {
            WindowLostFocusEvent e;
            if (m_callback) m_callback(e);
            break;
        }
        case SDL_EVENT_WINDOW_MOVED: {
            WindowMovedEvent e(sdl.window.data1, sdl.window.data2);
            if (m_callback) m_callback(e);
            break;
        }

        // ---- 键盘 ---------------------------------------------------------
        case SDL_EVENT_KEY_DOWN: {
            KeyPressedEvent pressed(static_cast<Key::KeyCode>(sdl.key.scancode), sdl.key.repeat);
            Input::OnEvent(pressed);
            if (m_callback) m_callback(pressed);

            if (!sdl.key.repeat) {
                KeyTypedEvent typed(static_cast<Key::KeyCode>(sdl.key.scancode));
                Input::OnEvent(typed);
                if (m_callback) m_callback(typed);
            }
            break;
        }
        case SDL_EVENT_KEY_UP: {
            KeyReleasedEvent e(static_cast<Key::KeyCode>(sdl.key.scancode));
            Input::OnEvent(e);
            if (m_callback) m_callback(e);
            break;
        }

        // ---- 鼠标 ---------------------------------------------------------
        case SDL_EVENT_MOUSE_MOTION: {
            MouseMovedEvent e(sdl.motion.x, sdl.motion.y);
            Input::OnEvent(e);
            if (m_callback) m_callback(e);
            break;
        }

        case SDL_EVENT_MOUSE_BUTTON_DOWN: {
            MouseButtonPressedEvent e(static_cast<Mouse::MouseCode>(sdl.button.button));
            Input::OnEvent(e);
            if (m_callback) m_callback(e);
            break;
        }
        case SDL_EVENT_MOUSE_BUTTON_UP: {
            MouseButtonReleasedEvent e(static_cast<Mouse::MouseCode>(sdl.button.button));
            Input::OnEvent(e);
            if (m_callback) m_callback(e);
            break;
        }

        case SDL_EVENT_MOUSE_WHEEL: {
            MouseScrolledEvent e(sdl.wheel.x, sdl.wheel.y);
            if (m_callback) m_callback(e);
            break;
        }

        default:
            break;
        }
    }
}

} // namespace Crumb
