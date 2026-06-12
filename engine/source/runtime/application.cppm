export module crumb.runtime:application;

import crumb.core;
import crumb.platform;
import std;

export namespace Crumb {

class Application {
public:
    Application(const WindowCreateInfo& wci = {});
    virtual ~Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    virtual void OnInit()    {}
    virtual void OnInput(float deltaTime) {}
    virtual void OnUpdate(float deltaTime) {}
    virtual void OnRender(float deltaTime) {}
    virtual void OnShutdown() {}

    void Run();

    void Quit() { m_Running = false; }
    bool IsRunning() const { return m_Running; }

    Window& GetWindow() { return *m_Window; }
    Time&   GetTime()   { return m_Time; }
    float   GetFPS() const;

protected:
    std::unique_ptr<Window> m_Window;
    Time  m_Time;
    bool  m_Running = true;
};

} // namespace Crumb
