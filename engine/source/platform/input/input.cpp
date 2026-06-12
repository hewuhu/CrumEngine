module crumb.platform.input;

import crumb.platform.events;
import std;

namespace Crumb {

// ============================================================================
// 内部状态
// ============================================================================

// 键盘 — 双缓冲，检测按下/释放边沿
static std::bitset<512> s_CurrentKeys;
static std::bitset<512> s_PreviousKeys;

// 鼠标按钮 — 同上
static std::bitset<8> s_CurrentMouseButtons;
static std::bitset<8> s_PreviousMouseButtons;

// 鼠标位置
static float s_MouseX = 0.0f;
static float s_MouseY = 0.0f;

// ============================================================================
// Initialize / Shutdown
// ============================================================================

void Input::Initialize() {
    s_CurrentKeys.reset();
    s_PreviousKeys.reset();
    s_CurrentMouseButtons.reset();
    s_PreviousMouseButtons.reset();
    s_MouseX = 0.0f;
    s_MouseY = 0.0f;
}

// ============================================================================
// NewFrame — 把当前状态"归档"到上一帧
// ============================================================================

void Input::NewFrame() {
    s_PreviousKeys        = s_CurrentKeys;
    s_PreviousMouseButtons = s_CurrentMouseButtons;
}

// ============================================================================
// OnEvent — 由窗口 PollEvents 调用，把事件写入当前状态
// ============================================================================

void Input::OnEvent(Event& e) {
    switch (e.GetEventType()) {

    // ---- 键盘 ---------------------------------------------------------------
    case EventType::KeyPressed: {
        auto& ke = static_cast<KeyPressedEvent&>(e);
        s_CurrentKeys.set(static_cast<std::size_t>(ke.GetKeyCode()));
        break;
    }
    case EventType::KeyReleased: {
        auto& ke = static_cast<KeyReleasedEvent&>(e);
        s_CurrentKeys.reset(static_cast<std::size_t>(ke.GetKeyCode()));
        break;
    }

    // ---- 鼠标按钮 -----------------------------------------------------------
    case EventType::MouseButtonPressed: {
        auto& me = static_cast<MouseButtonPressedEvent&>(e);
        s_CurrentMouseButtons.set(static_cast<std::size_t>(me.GetMouseButton()));
        break;
    }
    case EventType::MouseButtonReleased: {
        auto& me = static_cast<MouseButtonReleasedEvent&>(e);
        s_CurrentMouseButtons.reset(static_cast<std::size_t>(me.GetMouseButton()));
        break;
    }

    // ---- 鼠标移动 -----------------------------------------------------------
    case EventType::MouseMoved: {
        auto& me = static_cast<MouseMovedEvent&>(e);
        s_MouseX = me.GetX();
        s_MouseY = me.GetY();
        break;
    }

    // 其他事件不改状态
    default:
        break;
    }
}

// ============================================================================
// 键盘查询
// ============================================================================

bool Input::IsKeyPressed(Key::KeyCode key) {
    auto idx = static_cast<std::size_t>(key);
    return s_CurrentKeys[idx] && !s_PreviousKeys[idx];
}

bool Input::IsKeyDown(Key::KeyCode key) {
    return s_CurrentKeys[static_cast<std::size_t>(key)];
}

bool Input::IsKeyReleased(Key::KeyCode key) {
    auto idx = static_cast<std::size_t>(key);
    return !s_CurrentKeys[idx] && s_PreviousKeys[idx];
}

// ============================================================================
// 鼠标按钮查询
// ============================================================================

bool Input::IsMouseButtonPressed(Mouse::MouseCode button) {
    auto idx = static_cast<std::size_t>(button);
    return s_CurrentMouseButtons[idx] && !s_PreviousMouseButtons[idx];
}

bool Input::IsMouseButtonDown(Mouse::MouseCode button) {
    return s_CurrentMouseButtons[static_cast<std::size_t>(button)];
}

bool Input::IsMouseButtonReleased(Mouse::MouseCode button) {
    auto idx = static_cast<std::size_t>(button);
    return !s_CurrentMouseButtons[idx] && s_PreviousMouseButtons[idx];
}

// ============================================================================
// 鼠标位置
// ============================================================================

float Input::GetMouseX() { return s_MouseX; }
float Input::GetMouseY() { return s_MouseY; }

std::pair<float, float> Input::GetMousePosition() {
    return { s_MouseX, s_MouseY };
}

} // namespace Crumb
