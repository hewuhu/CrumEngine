import crumb;
import std;

extern std::unique_ptr<Crumb::Application> CreateApplication();

int main(int argc, char** argv) {
    auto app = CreateApplication();
    app->Run();
}