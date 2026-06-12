export module crumb.platform.events:mouse_codes;
import std;

export namespace Mouse {

using MouseCode = std::uint16_t;

// 与 SDL 对齐: SDL_BUTTON_LEFT=1, MIDDLE=2, RIGHT=3, X1=4, X2=5
enum : MouseCode {
    Left   = 1,
    Middle = 2,
    Right  = 3,
    X1     = 4,
    X2     = 5
};

}  // namespace Mouse
