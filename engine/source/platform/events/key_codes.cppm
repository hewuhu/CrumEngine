export module crumb.platform.events:key_codes;
import std;

export namespace Key {

using KeyCode = std::uint16_t;


enum : KeyCode {
    Unknown = 0,

    // 字母
    A = 4, B = 5, C = 6, D = 7, E = 8, F = 9, G = 10, H = 11,
    I = 12, J = 13, K = 14, L = 15, M = 16, N = 17, O = 18,
    P = 19, Q = 20, R = 21, S = 22, T = 23, U = 24, V = 25,
    W = 26, X = 27, Y = 28, Z = 29,

    // 数字行
    Num1 = 30, Num2 = 31, Num3 = 32, Num4 = 33, Num5 = 34,
    Num6 = 35, Num7 = 36, Num8 = 37, Num9 = 38, Num0 = 39,

    Return     = 40,
    Escape     = 41,
    Backspace  = 42,
    Tab        = 43,
    Space      = 44,

    Minus        = 45,
    Equals       = 46,
    LeftBracket  = 47,
    RightBracket = 48,
    Backslash    = 49,
    Semicolon    = 51,
    Apostrophe   = 52,
    Grave        = 53,
    Comma        = 54,
    Period       = 55,
    Slash        = 56,
    CapsLock     = 57,

    // 功能键
    F1  = 58, F2  = 59, F3  = 60, F4  = 61,
    F5  = 62, F6  = 63, F7  = 64, F8  = 65,
    F9  = 66, F10 = 67, F11 = 68, F12 = 69,

    PrintScreen = 70,
    ScrollLock  = 71,
    Pause       = 72,
    Insert      = 73,
    Home        = 74,
    PageUp      = 75,
    Delete      = 76,
    End         = 77,
    PageDown    = 78,
    Right       = 79,
    Left        = 80,
    Down        = 81,
    Up          = 82,
    NumLock     = 83,

    // 小键盘
    KP_Divide   = 84,
    KP_Multiply = 85,
    KP_Minus    = 86,
    KP_Plus     = 87,
    KP_Enter    = 88,
    KP_1 = 89, KP_2 = 90, KP_3 = 91, KP_4 = 92, KP_5 = 93,
    KP_6 = 94, KP_7 = 95, KP_8 = 96, KP_9 = 97, KP_0 = 98,
    KP_Period = 99,

    // 国际键
    NonUSBackslash = 100,
    Application    = 101,
    KP_Equals      = 103,

    // F13–F24
    F13 = 104, F14 = 105, F15 = 106, F16 = 107,
    F17 = 108, F18 = 109, F19 = 110, F20 = 111,
    F21 = 112, F22 = 113, F23 = 114, F24 = 115,

    Execute  = 116,
    Help     = 117,
    Menu     = 118,
    Select   = 119,
    Stop     = 120,
    Again    = 121,
    Undo     = 122,
    Cut      = 123,
    Copy     = 124,
    Paste    = 125,
    Find     = 126,

    Mute        = 127,
    VolumeUp    = 128,
    VolumeDown  = 129,

    KP_Comma       = 133,
    KP_EqualsAS400 = 134,

    // 修饰键
    LCtrl  = 224,
    LShift = 225,
    LAlt   = 226,
    LGui   = 227,
    RCtrl  = 228,
    RShift = 229,
    RAlt   = 230,
    RGui   = 231,
};

} // namespace Key
