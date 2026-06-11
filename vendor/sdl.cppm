module;
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

export module sdl;
import std;

export {
    // ── core types ──
    using ::Uint32;
    using ::Uint64;
    using ::Sint32;

    using ::SDL_Window;
    using ::SDL_WindowFlags;
    using ::SDL_WindowID;
    using ::SDL_DisplayID;
    using ::SDL_DisplayMode;

    // ── rect & geometry ──
    using ::SDL_Point;
    using ::SDL_FPoint;
    using ::SDL_Rect;
    using ::SDL_FRect;

    // ── init ──
    using ::SDL_InitFlags;
    using ::SDL_Init;
    using ::SDL_InitSubSystem;
    using ::SDL_Quit;
    using ::SDL_QuitSubSystem;
    using ::SDL_WasInit;

    // ── error ──
    using ::SDL_GetError;

    // ── timer ──
    using ::SDL_GetTicks;
    using ::SDL_GetTicksNS;
    using ::SDL_GetPerformanceCounter;
    using ::SDL_GetPerformanceFrequency;
    using ::SDL_Delay;
    using ::SDL_DelayNS;
    using ::SDL_TimerID;
    using ::SDL_AddTimer;
    using ::SDL_RemoveTimer;

    // ── video / window ──
    using ::SDL_CreateWindow;
    using ::SDL_CreateWindowWithProperties;
    using ::SDL_DestroyWindow;
    using ::SDL_GetWindowTitle;
    using ::SDL_SetWindowTitle;
    using ::SDL_GetWindowSize;
    using ::SDL_SetWindowSize;
    using ::SDL_GetWindowSizeInPixels;
    using ::SDL_GetWindowPosition;
    using ::SDL_SetWindowPosition;
    using ::SDL_ShowWindow;
    using ::SDL_HideWindow;
    using ::SDL_MaximizeWindow;
    using ::SDL_MinimizeWindow;
    using ::SDL_RestoreWindow;
    using ::SDL_RaiseWindow;
    using ::SDL_SetWindowFullscreen;
    using ::SDL_SetWindowResizable;
    using ::SDL_SyncWindow;
    using ::SDL_GetWindowFlags;
    using ::SDL_GetWindowPixelDensity;
    using ::SDL_GetWindowDisplayScale;
    using ::SDL_SetWindowFullscreenMode;
    using ::SDL_GetWindowFullscreenMode;
    using ::SDL_GetWindowPixelFormat;
    using ::SDL_GetWindows;
    using ::SDL_GetDisplayForWindow;
    using ::SDL_GetDisplays;
    using ::SDL_GetDisplayName;
    using ::SDL_GetDisplayBounds;
    using ::SDL_GetDisplayUsableBounds;
    using ::SDL_GetNaturalDisplayOrientation;
    using ::SDL_GetCurrentDisplayOrientation;
    using ::SDL_GetClosestFullscreenDisplayMode;
    using ::SDL_GetFullscreenDisplayModes;
    using ::SDL_GetDesktopDisplayMode;
    using ::SDL_GetCurrentDisplayMode;
    using ::SDL_GetPrimaryDisplay;
    using ::SDL_SetWindowModal;
    using ::SDL_SetWindowFocusable;
    using ::SDL_GetWindowParent;
    using ::SDL_SetWindowHitTest;
    using ::SDL_GetWindowOpacity;
    using ::SDL_SetWindowOpacity;
    using ::SDL_SetWindowShape;

    // ── events ──
    using ::SDL_Event;
    using ::SDL_EventType;
    using ::SDL_CommonEvent;
    using ::SDL_DisplayEvent;
    using ::SDL_WindowEvent;
    using ::SDL_KeyboardDeviceEvent;
    using ::SDL_KeyboardEvent;
    using ::SDL_TextEditingEvent;
    using ::SDL_TextEditingCandidatesEvent;
    using ::SDL_TextInputEvent;
    using ::SDL_MouseDeviceEvent;
    using ::SDL_MouseMotionEvent;
    using ::SDL_MouseButtonEvent;
    using ::SDL_MouseWheelEvent;
    using ::SDL_JoyAxisEvent;
    using ::SDL_JoyBallEvent;
    using ::SDL_JoyHatEvent;
    using ::SDL_JoyButtonEvent;
    using ::SDL_JoyDeviceEvent;
    using ::SDL_JoyBatteryEvent;
    using ::SDL_GamepadAxisEvent;
    using ::SDL_GamepadButtonEvent;
    using ::SDL_GamepadDeviceEvent;
    using ::SDL_GamepadTouchpadEvent;
    using ::SDL_GamepadSensorEvent;
    using ::SDL_AudioDeviceEvent;
    using ::SDL_CameraDeviceEvent;
    using ::SDL_RenderEvent;
    using ::SDL_TouchFingerEvent;
    using ::SDL_PinchFingerEvent;

    using ::SDL_PollEvent;
    using ::SDL_WaitEvent;
    using ::SDL_WaitEventTimeout;
    using ::SDL_PumpEvents;
    using ::SDL_PushEvent;
    using ::SDL_PeepEvents;
    using ::SDL_AddEventWatch;
    using ::SDL_RemoveEventWatch;

    // ── keyboard ──
    using ::SDL_Keycode;
    using ::SDL_Keymod;
    using ::SDL_Scancode;
    using ::SDL_KeyboardID;
    using ::SDL_HasKeyboard;
    using ::SDL_GetKeyboards;
    using ::SDL_GetKeyboardFocus;
    using ::SDL_GetKeyboardState;
    using ::SDL_ResetKeyboard;
    using ::SDL_GetModState;
    using ::SDL_SetModState;
    using ::SDL_GetKeyFromScancode;
    using ::SDL_GetScancodeFromKey;
    using ::SDL_GetScancodeName;
    using ::SDL_GetScancodeFromName;
    using ::SDL_GetKeyName;
    using ::SDL_GetKeyFromName;

    // ── mouse ──
    using ::SDL_MouseButtonFlags;
    using ::SDL_MouseID;
    using ::SDL_Cursor;
    using ::SDL_SystemCursor;
    using ::SDL_HasMouse;
    using ::SDL_GetMice;
    using ::SDL_GetMouseFocus;
    using ::SDL_GetMouseState;
    using ::SDL_GetGlobalMouseState;
    using ::SDL_GetRelativeMouseState;
    using ::SDL_WarpMouseInWindow;
    using ::SDL_WarpMouseGlobal;
    using ::SDL_SetWindowRelativeMouseMode;
    using ::SDL_GetWindowRelativeMouseMode;
    using ::SDL_CaptureMouse;
    using ::SDL_CreateCursor;
    using ::SDL_CreateColorCursor;
    using ::SDL_CreateSystemCursor;
    using ::SDL_SetCursor;
    using ::SDL_GetCursor;
    using ::SDL_GetDefaultCursor;
    using ::SDL_DestroyCursor;
    using ::SDL_ShowCursor;
    using ::SDL_HideCursor;
    using ::SDL_CursorVisible;

    // ── gamepad ──
    using ::SDL_Gamepad;
    using ::SDL_GamepadType;
    using ::SDL_GamepadButton;
    using ::SDL_GamepadAxis;
    using ::SDL_JoystickID;
    using ::SDL_GUID;
    using ::SDL_HasGamepad;
    using ::SDL_GetGamepads;
    using ::SDL_IsGamepad;
    using ::SDL_GetGamepadNameForID;
    using ::SDL_OpenGamepad;
    using ::SDL_CloseGamepad;
    using ::SDL_GetGamepadName;
    using ::SDL_GetGamepadType;
    using ::SDL_GetGamepadPlayerIndex;
    using ::SDL_SetGamepadPlayerIndex;
    using ::SDL_GetGamepadGUIDForID;
    using ::SDL_GamepadConnected;
    using ::SDL_GetGamepadButton;
    using ::SDL_GetGamepadAxis;
    using ::SDL_RumbleGamepad;
    using ::SDL_RumbleGamepadTriggers;
    using ::SDL_SetGamepadLED;
    using ::SDL_SendGamepadEffect;
    using ::SDL_GetGamepadMappings;
    using ::SDL_GetGamepadMapping;
    using ::SDL_AddGamepadMapping;
    using ::SDL_AddGamepadMappingsFromFile;
    using ::SDL_ReloadGamepadMappings;

    // ── audio ──
    using ::SDL_AudioDeviceID;
    using ::SDL_AudioSpec;
    using ::SDL_AudioStream;
    using ::SDL_AudioFormat;
    using ::SDL_GetNumAudioDrivers;
    using ::SDL_GetAudioDriver;
    using ::SDL_GetCurrentAudioDriver;
    using ::SDL_GetAudioPlaybackDevices;
    using ::SDL_GetAudioRecordingDevices;
    using ::SDL_GetAudioDeviceName;
    using ::SDL_GetAudioDeviceFormat;
    using ::SDL_OpenAudioDevice;
    using ::SDL_IsAudioDevicePhysical;
    using ::SDL_IsAudioDevicePlayback;
    using ::SDL_PauseAudioDevice;
    using ::SDL_ResumeAudioDevice;
    using ::SDL_AudioDevicePaused;
    using ::SDL_CloseAudioDevice;
    using ::SDL_BindAudioStream;
    using ::SDL_UnbindAudioStream;
    using ::SDL_CreateAudioStream;
    using ::SDL_DestroyAudioStream;
    using ::SDL_PutAudioStreamData;
    using ::SDL_GetAudioStreamData;
    using ::SDL_GetAudioStreamAvailable;
    using ::SDL_GetAudioStreamQueued;
    using ::SDL_FlushAudioStream;
    using ::SDL_ClearAudioStream;
    using ::SDL_PauseAudioStreamDevice;
    using ::SDL_ResumeAudioStreamDevice;
    using ::SDL_AudioStreamDevicePaused;

    // ── vulkan ──
    using ::VkInstance;
    using ::VkSurfaceKHR;

    using ::SDL_Vulkan_LoadLibrary;
    using ::SDL_Vulkan_UnloadLibrary;
    using ::SDL_Vulkan_GetVkGetInstanceProcAddr;
    using ::SDL_Vulkan_GetInstanceExtensions;
    using ::SDL_Vulkan_CreateSurface;
    using ::SDL_Vulkan_DestroySurface;
    using ::SDL_Vulkan_GetPresentationSupport;

    // ── filesystem ──
    using ::SDL_free;
    using ::SDL_GetBasePath;
    using ::SDL_GetPrefPath;
    using ::SDL_GetUserFolder;
    using ::SDL_CreateDirectory;
    using ::SDL_RemovePath;
    using ::SDL_RenamePath;
    using ::SDL_CopyFile;
    using ::SDL_GetPathInfo;
    using ::SDL_GetCurrentDirectory;

} // export

// macro wrappers — SDL macros can't pass through module boundaries
export namespace sdl {
    // init flags
    inline constexpr auto INIT_AUDIO   = SDL_INIT_AUDIO;
    inline constexpr auto INIT_VIDEO   = SDL_INIT_VIDEO;
    inline constexpr auto INIT_GAMEPAD = SDL_INIT_GAMEPAD;
    inline constexpr auto INIT_JOYSTICK = SDL_INIT_JOYSTICK;
    inline constexpr auto INIT_HAPTIC  = SDL_INIT_HAPTIC;
    inline constexpr auto INIT_SENSOR  = SDL_INIT_SENSOR;
    inline constexpr auto INIT_EVENTS  = SDL_INIT_EVENTS;
    inline constexpr auto INIT_CAMERA  = SDL_INIT_CAMERA;

    // window flags
    inline constexpr SDL_WindowFlags WINDOW_FULLSCREEN           = SDL_WINDOW_FULLSCREEN;
    inline constexpr SDL_WindowFlags WINDOW_OPENGL               = SDL_WINDOW_OPENGL;
    inline constexpr SDL_WindowFlags WINDOW_OCCLUDED             = SDL_WINDOW_OCCLUDED;
    inline constexpr SDL_WindowFlags WINDOW_HIDDEN               = SDL_WINDOW_HIDDEN;
    inline constexpr SDL_WindowFlags WINDOW_BORDERLESS           = SDL_WINDOW_BORDERLESS;
    inline constexpr SDL_WindowFlags WINDOW_RESIZABLE            = SDL_WINDOW_RESIZABLE;
    inline constexpr SDL_WindowFlags WINDOW_MINIMIZED            = SDL_WINDOW_MINIMIZED;
    inline constexpr SDL_WindowFlags WINDOW_MAXIMIZED            = SDL_WINDOW_MAXIMIZED;
    inline constexpr SDL_WindowFlags WINDOW_MOUSE_GRABBED        = SDL_WINDOW_MOUSE_GRABBED;
    inline constexpr SDL_WindowFlags WINDOW_INPUT_FOCUS          = SDL_WINDOW_INPUT_FOCUS;
    inline constexpr SDL_WindowFlags WINDOW_MOUSE_FOCUS          = SDL_WINDOW_MOUSE_FOCUS;
    inline constexpr SDL_WindowFlags WINDOW_EXTERNAL             = SDL_WINDOW_EXTERNAL;
    inline constexpr SDL_WindowFlags WINDOW_MODAL                = SDL_WINDOW_MODAL;
    inline constexpr SDL_WindowFlags WINDOW_HIGH_PIXEL_DENSITY   = SDL_WINDOW_HIGH_PIXEL_DENSITY;
    inline constexpr SDL_WindowFlags WINDOW_MOUSE_CAPTURE        = SDL_WINDOW_MOUSE_CAPTURE;
    inline constexpr SDL_WindowFlags WINDOW_MOUSE_RELATIVE_MODE  = SDL_WINDOW_MOUSE_RELATIVE_MODE;
    inline constexpr SDL_WindowFlags WINDOW_ALWAYS_ON_TOP        = SDL_WINDOW_ALWAYS_ON_TOP;
    inline constexpr SDL_WindowFlags WINDOW_UTILITY              = SDL_WINDOW_UTILITY;
    inline constexpr SDL_WindowFlags WINDOW_TOOLTIP              = SDL_WINDOW_TOOLTIP;
    inline constexpr SDL_WindowFlags WINDOW_POPUP_MENU           = SDL_WINDOW_POPUP_MENU;
    inline constexpr SDL_WindowFlags WINDOW_KEYBOARD_GRABBED     = SDL_WINDOW_KEYBOARD_GRABBED;
    inline constexpr SDL_WindowFlags WINDOW_VULKAN               = SDL_WINDOW_VULKAN;
    inline constexpr SDL_WindowFlags WINDOW_METAL                = SDL_WINDOW_METAL;
    inline constexpr SDL_WindowFlags WINDOW_TRANSPARENT           = SDL_WINDOW_TRANSPARENT;
    inline constexpr SDL_WindowFlags WINDOW_NOT_FOCUSABLE        = SDL_WINDOW_NOT_FOCUSABLE;

    // mouse button flags
    inline constexpr auto BUTTON_LEFT   = SDL_BUTTON_LEFT;
    inline constexpr auto BUTTON_RIGHT  = SDL_BUTTON_RIGHT;
    inline constexpr auto BUTTON_MIDDLE = SDL_BUTTON_MIDDLE;
    inline constexpr auto BUTTON_X1     = SDL_BUTTON_X1;
    inline constexpr auto BUTTON_X2     = SDL_BUTTON_X2;

    // event types
    inline constexpr SDL_EventType EVENT_QUIT           = SDL_EVENT_QUIT;
    inline constexpr SDL_EventType EVENT_KEY_DOWN       = SDL_EVENT_KEY_DOWN;
    inline constexpr SDL_EventType EVENT_KEY_UP         = SDL_EVENT_KEY_UP;
    inline constexpr SDL_EventType EVENT_MOUSE_MOTION   = SDL_EVENT_MOUSE_MOTION;
    inline constexpr SDL_EventType EVENT_MOUSE_BUTTON_DOWN = SDL_EVENT_MOUSE_BUTTON_DOWN;
    inline constexpr SDL_EventType EVENT_MOUSE_BUTTON_UP   = SDL_EVENT_MOUSE_BUTTON_UP;
    inline constexpr SDL_EventType EVENT_MOUSE_WHEEL       = SDL_EVENT_MOUSE_WHEEL;
    inline constexpr SDL_EventType EVENT_TEXT_INPUT        = SDL_EVENT_TEXT_INPUT;
    inline constexpr SDL_EventType EVENT_WINDOW_FOCUS_LOST = SDL_EVENT_WINDOW_FOCUS_LOST;
    inline constexpr SDL_EventType EVENT_WINDOW_RESIZED    = SDL_EVENT_WINDOW_RESIZED;

    // system cursors
    inline constexpr auto SYSTEM_CURSOR_DEFAULT    = SDL_SYSTEM_CURSOR_DEFAULT;
    inline constexpr auto SYSTEM_CURSOR_TEXT        = SDL_SYSTEM_CURSOR_TEXT;
    inline constexpr auto SYSTEM_CURSOR_WAIT        = SDL_SYSTEM_CURSOR_WAIT;
    inline constexpr auto SYSTEM_CURSOR_CROSSHAIR   = SDL_SYSTEM_CURSOR_CROSSHAIR;
    inline constexpr auto SYSTEM_CURSOR_PROGRESS    = SDL_SYSTEM_CURSOR_PROGRESS;
    inline constexpr auto SYSTEM_CURSOR_NWSE_RESIZE = SDL_SYSTEM_CURSOR_NWSE_RESIZE;
    inline constexpr auto SYSTEM_CURSOR_NESW_RESIZE = SDL_SYSTEM_CURSOR_NESW_RESIZE;
    inline constexpr auto SYSTEM_CURSOR_EW_RESIZE   = SDL_SYSTEM_CURSOR_EW_RESIZE;
    inline constexpr auto SYSTEM_CURSOR_NS_RESIZE   = SDL_SYSTEM_CURSOR_NS_RESIZE;
    inline constexpr auto SYSTEM_CURSOR_MOVE        = SDL_SYSTEM_CURSOR_MOVE;
    inline constexpr auto SYSTEM_CURSOR_NOT_ALLOWED  = SDL_SYSTEM_CURSOR_NOT_ALLOWED;
    inline constexpr auto SYSTEM_CURSOR_POINTER     = SDL_SYSTEM_CURSOR_POINTER;
    inline constexpr auto SYSTEM_CURSOR_NWSE_RESIZE_ALT = SDL_SYSTEM_CURSOR_NWSE_RESIZE;
    inline constexpr auto SYSTEM_CURSOR_NESW_RESIZE_ALT = SDL_SYSTEM_CURSOR_NESW_RESIZE;
    inline constexpr auto SYSTEM_CURSOR_EW_RESIZE_ALT   = SDL_SYSTEM_CURSOR_EW_RESIZE;
    inline constexpr auto SYSTEM_CURSOR_NS_RESIZE_ALT   = SDL_SYSTEM_CURSOR_NS_RESIZE;
    inline constexpr auto SYSTEM_CURSOR_NW_RESIZE   = SDL_SYSTEM_CURSOR_NW_RESIZE;
    inline constexpr auto SYSTEM_CURSOR_SE_RESIZE   = SDL_SYSTEM_CURSOR_SE_RESIZE;
    inline constexpr auto SYSTEM_CURSOR_NE_RESIZE   = SDL_SYSTEM_CURSOR_NE_RESIZE;
    inline constexpr auto SYSTEM_CURSOR_SW_RESIZE   = SDL_SYSTEM_CURSOR_SW_RESIZE;
    inline constexpr auto SYSTEM_CURSOR_N_RESIZE    = SDL_SYSTEM_CURSOR_N_RESIZE;
    inline constexpr auto SYSTEM_CURSOR_S_RESIZE    = SDL_SYSTEM_CURSOR_S_RESIZE;
    inline constexpr auto SYSTEM_CURSOR_E_RESIZE    = SDL_SYSTEM_CURSOR_E_RESIZE;
    inline constexpr auto SYSTEM_CURSOR_W_RESIZE    = SDL_SYSTEM_CURSOR_W_RESIZE;

} // namespace sdl