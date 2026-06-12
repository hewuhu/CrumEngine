// 应最先被编译
export module crumb.platform.events:event;

import std;

export namespace Crumb {

enum class EventType : int {
    None = 0,
    WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
    AppTick, AppUpdate, AppRender,
    KeyPressed, KeyReleased, KeyTyped,
    MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
};

// 位域掩码
// 一共五个字节位
enum EventCategory : std::uint8_t {
    None                     = 0,
    EventCategoryApplication = 1U << 0,
    EventCategoryInput       = 1U << 1,
    EventCategoryKeyboard    = 1U << 2,
    EventCategoryMouse       = 1U << 3,
    EventCategoryMouseButton = 1U << 4
};

class Event {
public:
    virtual ~Event() = default;

    bool Handled = false; // 是否已处理

    virtual EventType GetEventType() const = 0;
    virtual const char* GetName() const = 0;
    virtual int GetCategoryFlags() const = 0;
    virtual std::string ToString() const { return GetName(); }

    // 掩码过滤
    inline bool IsInCategory(EventCategory category) const {
        return GetCategoryFlags() & static_cast<int>(category);
    }
};

// CRTP 基类 — 子类提供三个 static constexpr 字段，模板自动实现 Event 的纯虚接口。
//
// 用法：
//     class XxxEvent : public EventImpl<XxxEvent> {
//         static constexpr auto        s_Type     = EventType::XXX;
//         static constexpr const char* s_Name     = "XXX";
//         static constexpr int         s_Category = XXX;
//     };
template<typename Derived>
class EventImpl : public Event {
public:
    static EventType GetStaticType() { return Derived::s_Type; }

    [[nodiscard]] EventType GetEventType() const final { return Derived::s_Type; }
    [[nodiscard]] const char* GetName() const final { return Derived::s_Name; }
    [[nodiscard]] int GetCategoryFlags() const final { return Derived::s_Category; }
};

// 把Event& 还原成原来具体类型
class EventDispatcher {
public:
    explicit EventDispatcher(Event& event) : m_Event(event) {}

    template<typename T, typename F>
    bool Dispatch(const F& func) {
        if (m_Event.GetEventType() == T::GetStaticType()) {
            m_Event.Handled |= func(static_cast<T&>(m_Event));
            return true;
        }
        return false;
    }

private:
    Event& m_Event;
};

} // namespace Crumb


// 特化 std::formatter —— 注入到 std 命名空间（标准允许）
// 用法：std::println("{}", event)
template <>
struct std::formatter<Crumb::Event> : std::formatter<std::string> {
    auto format(const Crumb::Event& e, std::format_context& ctx) const {
        return std::formatter<std::string>::format(e.ToString(), ctx);
    }
};
