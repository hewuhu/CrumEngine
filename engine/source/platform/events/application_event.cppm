export module crumb.platform.events:application_event;

import :event;
import std;

export namespace Crumb {

class WindowResizeEvent : public EventImpl<WindowResizeEvent> {
public:
    WindowResizeEvent(unsigned int width, unsigned int height)
    : m_Width(width), m_Height(height) {}
    
    unsigned int GetWidth() const { return m_Width; }
    unsigned int GetHeight() const { return m_Height; }
    
    std::string ToString() const override {
        return std::format("WindowResizeEvent: {}, {}", m_Width, m_Height);
    }

    static constexpr auto s_Type = EventType::WindowResize;
    static constexpr const char* s_Name = "WindowResize";
    static constexpr int s_Category = EventCategoryApplication;

private:
    unsigned int m_Width, m_Height;
};

class WindowCloseEvent : public EventImpl<WindowCloseEvent> {
public:
    WindowCloseEvent() = default;

    static constexpr auto s_Type = EventType::WindowClose;
    static constexpr const char* s_Name = "WindowClose";
    static constexpr int s_Category = EventCategoryApplication;
};

class WindowFocusEvent : public EventImpl<WindowFocusEvent> {
public:
    WindowFocusEvent() = default;

    static constexpr auto s_Type = EventType::WindowFocus;
    static constexpr const char* s_Name = "WindowFocus";
    static constexpr int s_Category = EventCategoryApplication;
};

class WindowLostFocusEvent : public EventImpl<WindowLostFocusEvent> {
public:
    WindowLostFocusEvent() = default;

    static constexpr auto s_Type = EventType::WindowLostFocus;
    static constexpr const char* s_Name = "WindowLostFocus";
    static constexpr int s_Category = EventCategoryApplication;
};

class WindowMovedEvent : public EventImpl<WindowMovedEvent> {
public:
    WindowMovedEvent(int x, int y) : m_X(x), m_Y(y) {}

    int GetX() const { return m_X; }
    int GetY() const { return m_Y; }

    std::string ToString() const override {
        return std::format("WindowMovedEvent: {}, {}", m_X, m_Y);
    }

    static constexpr auto s_Type = EventType::WindowMoved;
    static constexpr const char* s_Name = "WindowMoved";
    static constexpr int s_Category = EventCategoryApplication;

private:
    int m_X, m_Y;
};

class AppTickEvent : public EventImpl<AppTickEvent> {
public:
    AppTickEvent() = default;

    static constexpr auto s_Type = EventType::AppTick;
    static constexpr const char* s_Name = "AppTick";
    static constexpr int s_Category = EventCategoryApplication;
};

class AppUpdateEvent : public EventImpl<AppUpdateEvent> {
public:
    AppUpdateEvent() = default;

    static constexpr auto s_Type = EventType::AppUpdate;
    static constexpr const char* s_Name = "AppUpdate";
    static constexpr int s_Category = EventCategoryApplication;
};

class AppRenderEvent : public EventImpl<AppRenderEvent> {
public:
    AppRenderEvent() = default;

    static constexpr auto s_Type = EventType::AppRender;
    static constexpr const char* s_Name = "AppRender";
    static constexpr int s_Category = EventCategoryApplication;
};

} // namespace Crumb
