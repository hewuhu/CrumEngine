export module crumb.platform.events:mouse_event;

import :event;
import :mouse_codes;
import std;

export namespace Crumb {

using Mouse::MouseCode;

class MouseButtonEvent {
public:
    static constexpr int s_Category = EventCategoryMouse | EventCategoryInput | EventCategoryMouseButton;

    [[nodiscard]] MouseCode GetMouseButton() const { return m_button; }

protected:
    MouseButtonEvent(MouseCode button) : m_button(button) {}
    MouseCode m_button {};
};

class MouseButtonPressedEvent : public EventImpl<MouseButtonPressedEvent>, public MouseButtonEvent {
public:
    static constexpr auto s_Type = EventType::MouseButtonPressed;
    static constexpr const char* s_Name = "MouseButtonPressed";
    static constexpr int s_Category = MouseButtonEvent::s_Category;

    MouseButtonPressedEvent(MouseCode button) : MouseButtonEvent(button) {}

    std::string ToString() const override {
        return std::format("MouseButtonPressedEvent: {}", m_button);
    }
};

class MouseButtonReleasedEvent : public EventImpl<MouseButtonReleasedEvent>, public MouseButtonEvent {
public:
    static constexpr auto s_Type = EventType::MouseButtonReleased;
    static constexpr const char* s_Name = "MouseButtonReleased";
    static constexpr int s_Category = MouseButtonEvent::s_Category;

    MouseButtonReleasedEvent(MouseCode button) : MouseButtonEvent(button) {}

    std::string ToString() const override {
        return std::format("MouseButtonReleasedEvent: {}", m_button);
    }
};

class MouseMovedEvent : public EventImpl<MouseMovedEvent> {
public:
    MouseMovedEvent(float x, float y) : m_mouseX(x), m_mouseY(y) {}

    float GetX() const { return m_mouseX; }
    float GetY() const { return m_mouseY; }

    std::string ToString() const override {
        return std::format("MouseMovedEvent: {}, {}", m_mouseX, m_mouseY);
    }

    static constexpr auto s_Type = EventType::MouseMoved;
    static constexpr const char* s_Name = "MouseMoved";
    static constexpr int s_Category = EventCategoryMouse | EventCategoryInput;

private:
    float m_mouseX {}, m_mouseY {};
};

class MouseScrolledEvent : public EventImpl<MouseScrolledEvent> {
public:
    MouseScrolledEvent(float xOffset, float yOffset) : m_xOffset(xOffset), m_yOffset(yOffset) {}

    float GetXOffset() const { return m_xOffset; }
    float GetYOffset() const { return m_yOffset; }

    std::string ToString() const override {
        return std::format("MouseScrolledEvent: {}, {}", m_xOffset, m_yOffset);
    }

    static constexpr auto s_Type = EventType::MouseScrolled;
    static constexpr const char* s_Name = "MouseScrolled";
    static constexpr int s_Category = EventCategoryMouse | EventCategoryInput;

private:
    float m_xOffset {}, m_yOffset {};
};

} // namespace Crumb
