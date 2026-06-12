# CrumEngine

一个 C++23 模块构建的游戏引擎，采用 Piccolo 双 tick 架构。

感谢他们提供的免费教程：
[The Cherno](https://www.youtube.com/@TheCherno)
[Game104](https://games104.boomingtech.com/sc/)
[Piccolo](https://github.com/BoomingTech/Piccolo)

---

## 环境准备

- **编译器**: MinGW + Clang
- **构建工具**: [xmake](https://github.com/xmake-io/xmake)
- **编辑器**: VS Code

---

## 依赖库

| 库 | 说明 |
|---|---|
| [SDL3](https://github.com/libsdl-org/SDL) | 跨平台窗口与输入 |
| [Vulkan 1.4](https://www.vulkan.org/) | 图形 API |
| [VulkanMemoryAllocator-Hpp](https://github.com/YaaZ/VulkanMemoryAllocator-Hpp) | Vulkan 内存分配器 |
| [GLM](https://github.com/g-truc/glm) | 数学库 |
| [Dear ImGui](https://github.com/ocornut/imgui) | GUI 框架 |
| [stb_image](https://github.com/nothings/stb) | 图像加载 |
| [tinyobjloader](https://github.com/tinyobjloader/tinyobjloader) | OBJ 模型加载 |
| [spdlog](https://github.com/gabime/spdlog) | 日志 |
| [fmt](https://github.com/fmtlib/fmt) | 格式化 |

---

## 快速开始

1. 将 `xmake.lua` 中所有 `C:/third_party/` 路径替换为本机实际路径
2. 构建：

```bash
xmake build sandbox
```

3. 运行：

```bash
xmake run sandbox
```

---

## 项目结构

```
CrumEngine/
├── engine/source/
│   ├── entry.cpp                          # main() 入口
│   ├── crumb.cppm                         # 顶层模块 crumb (聚合所有子系统)
│   │
│   ├── core/
│   │   ├── core.cppm                      # crumb.core — 聚合 log + time
│   │   ├── log/log.cppm                   # crumb.core:log — spdlog 封装
│   │   └── time/time.cppm                 # crumb.core:time — 高精度计时 + FPS
│   │       time/time.cpp
│   │
│   ├── platform/
│   │   ├── platform.cppm                  # crumb.platform — 聚合 events, window, input
│   │   │
│   │   ├── events/
│   │   │   ├── events_mod.cppm            # crumb.platform.events — 事件模块聚合
│   │   │   ├── event.cppm                 #   基类 Event, EventImpl<>, EventDispatcher
│   │   │   ├── key_codes.cppm             #   Key:: 命名空间 — 键盘码 (HID 规范)
│   │   │   ├── mouse_codes.cppm           #   Mouse:: 命名空间 — 鼠标按钮 (对齐 SDL)
│   │   │   ├── key_event.cppm             #   KeyPressed, KeyReleased, KeyTyped
│   │   │   ├── mouse_event.cppm           #   MouseButtonPressed, MouseMoved, MouseScrolled …
│   │   │   └── application_event.cppm     #   WindowResize, WindowClose, WindowFocus …
│   │   │
│   │   ├── window/
│   │   │   ├── window.cppm                #   Window 类 — SDL3 窗口封装
│   │   │   └── window.cpp                 #   PollEvents — SDL 事件 → 引擎事件 桥接
│   │   │
│   │   └── input/
│   │       ├── input.cppm                 #   Input 静态类 — 键盘/鼠标 状态查询
│   │       └── input.cpp                  #   双缓冲 按下/释放 边沿检测
│   │
│   └── runtime/
│       ├── runtime.cppm                   # crumb.runtime — 聚合 application
│       ├── application.cppm               #   Application 类 — 引擎主循环
│       └── application.cpp                #   双 tick: OnInput → OnUpdate → OnRender
│
├── vendor/                                # 第三方库 C++20 模块封装
│   ├── sdl.cppm
│   ├── glm.cppm
│   ├── vulkan/
│   └── imgui/
│
├── sandbox/
│   └── sandbox_app.cpp                    # 测试 / 示例应用
│
└── xmake.lua                              # 构建配置
```

## 模块层次

```
crumb                               (crumb.cppm)
├── crumb.core                      (core/core.cppm)
│   ├── crumb.core:log              (core/log/log.cppm)
│   └── crumb.core:time             (core/time/time.cppm)
├── crumb.platform                  (platform/platform.cppm)
│   ├── crumb.platform.events       (platform/events/events_mod.cppm)
│   │   ├── :event
│   │   ├── :key_codes
│   │   ├── :mouse_codes
│   │   ├── :key_event
│   │   ├── :mouse_event
│   │   └── :application_event
│   ├── crumb.platform.window       (platform/window/window.cppm)
│   └── crumb.platform.input        (platform/input/input.cppm)
└── crumb.runtime                   (runtime/runtime.cppm)
    └── crumb.runtime:application  (runtime/application.cppm)
```

---

## 说明

- 本项目为学习目的而构建，使用 AI 辅助编程。`vendor/` 下的 SDL / GLM / Vulkan / ImGui 模块封装均由 AI 生成。
- 引擎以 C++23 标准编译，使用 C++20 模块（`.cppm`）组织代码。
