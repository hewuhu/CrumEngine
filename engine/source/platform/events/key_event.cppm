export module crumb.platform.events:key_event;

import :event;
import :key_codes;
import std;

export namespace Crumb {

using Key::KeyCode;

class KeyEvent {
public:
//事件：输入事件  输入设备：键盘（输入设备可以不止是键盘，鼠标和触摸屏的输入都算输入）
static constexpr int s_Category = EventCategoryKeyboard | EventCategoryInput;

    [[nodiscard]] KeyCode GetKeyCode() const { return m_keyCode; }
    
protected:
    KeyEvent(KeyCode keycode) : m_keyCode{keycode} {}
    KeyCode m_keyCode {};
};

class KeyPressedEvent : public EventImpl<KeyPressedEvent>, public KeyEvent {
public:
    static constexpr auto s_Type = EventType::KeyPressed;
    static constexpr const char* s_Name = "KeyPressed";
    static constexpr int s_Category = KeyEvent::s_Category;

    KeyPressedEvent(KeyCode keycode, bool isRepeat = false)
        : KeyEvent(keycode), m_isRepeat(isRepeat) {}

    [[nodiscard]] bool IsRepeat() const { return m_isRepeat; }

    std::string ToString() const override {
        return std::format("KeyPressedEvent: {} (repeat = {})", m_keyCode, m_isRepeat);
    }

private:
    bool m_isRepeat;
};

class KeyReleasedEvent : public EventImpl<KeyReleasedEvent>, public KeyEvent {
public:
    static constexpr auto s_Type = EventType::KeyReleased;
    static constexpr const char* s_Name = "KeyReleased";
    static constexpr int s_Category = KeyEvent::s_Category;

    KeyReleasedEvent(KeyCode keycode) : KeyEvent(keycode) {}

    std::string ToString() const override {
        return std::format("KeyReleasedEvent: {}", m_keyCode);
    }
};

class KeyTypedEvent : public EventImpl<KeyTypedEvent>, public KeyEvent {
public:
    static constexpr auto s_Type = EventType::KeyTyped;
    static constexpr const char* s_Name = "KeyTyped";
    static constexpr int s_Category = KeyEvent::s_Category;

    KeyTypedEvent(KeyCode keycode) : KeyEvent(keycode) {}

    std::string ToString() const override {
        return std::format("KeyTypedEvent: {}", m_keyCode);
    }
};

} // namespace Crumb
