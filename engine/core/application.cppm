export module crumb.core:app;

export namespace Crumb {

class Application {
public:
    Application();
    virtual ~Application();

    void Run();
};

} // namespace Crumb