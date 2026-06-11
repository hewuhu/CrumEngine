import crumb;
import std;


class Sandbox : public Crumb::Application {
public:
    Sandbox() {

    }

    ~Sandbox() {

    }
};

auto CreateApplication() {
    return std::make_unique<Sandbox>();
}
