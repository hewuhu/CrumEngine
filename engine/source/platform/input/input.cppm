export module crumb.platform.input;

import crumb.platform.events;
import std;

// ============================================================================
// Input 静态类 — 每帧查询键盘 / 鼠标状态
// ============================================================================
export namespace Crumb {

class Input {
public:
    static void Initialize();

    // 每帧开头调用一次，将当前状态"归档"到上一帧，以便检测按下/释放边沿
    static void NewFrame();

    // SDL 事件 → 输入状态更新
    static void OnEvent(Event& e);

    // ---- 键盘 ---------------------------------------------------------------
    // 用法: Input::IsKeyDown(Key::A)
    static bool IsKeyPressed (Key::KeyCode key);   // 本帧刚按下
    static bool IsKeyDown    (Key::KeyCode key);   // 持续按住
    static bool IsKeyReleased(Key::KeyCode key);   // 本帧刚释放

    // ---- 鼠标按钮 -----------------------------------------------------------
    // 用法: Input::IsMouseButtonDown(Mouse::Left)
    static bool IsMouseButtonPressed (Mouse::MouseCode button);
    static bool IsMouseButtonDown    (Mouse::MouseCode button);
    static bool IsMouseButtonReleased(Mouse::MouseCode button);

    // ---- 鼠标位置 -----------------------------------------------------------
    static float GetMouseX();
    static float GetMouseY();
    static std::pair<float, float> GetMousePosition();
};

} // namespace Crumb
