import crumb;
import std;

extern std::unique_ptr<Crumb::Application> CreateApplication();

int main(int argc, char** argv) {
    Crumb::Log::Init();
    Crumb::Log::Info("initialize engine...");
    auto app = CreateApplication();
    app->Run();
    Crumb::Log::Shutdown();
}