# CrumEngine

一个学习 [The Cherno](https://www.youtube.com/@TheCherno) 游戏引擎系列时搭建的 C++23 游戏引擎，利用现代 C++ 模块特性进行开发。

---

## 环境准备

- **编译器**: MinGW + Clang（或 GCC）
- **构建工具**: [xmake](https://github.com/xmake-io/xmake)
- **编辑器**: VS Code

---

## 依赖库

| 库 | 说明 | 链接 |
|---|---|---|
| [SDL3](https://github.com/libsdl-org/SDL) | 跨平台窗口与输入管理 | <https://github.com/libsdl-org/SDL> |
| [Vulkan 1.4](https://www.vulkan.org/) | 高性能图形 API | <https://github.com/KhronosGroup/Vulkan-Headers> |
| [VulkanMemoryAllocator-Hpp](https://github.com/YaaZ/VulkanMemoryAllocator-Hpp) | Vulkan 内存分配器（C++17 封装） | <https://github.com/YaaZ/VulkanMemoryAllocator-Hpp> |
| [GLM](https://github.com/g-truc/glm) | OpenGL 数学库（仅头文件） | <https://github.com/g-truc/glm> |
| [Dear ImGui](https://github.com/ocornut/imgui) | 即时模式 GUI 框架 | <https://github.com/ocornut/imgui> |
| [stb_image](https://github.com/nothings/stb) | 单头文件图像加载库 | <https://github.com/nothings/stb> |
| [tinyobjloader](https://github.com/tinyobjloader/tinyobjloader) | 轻量级 OBJ 模型加载器 | <https://github.com/tinyobjloader/tinyobjloader> |
| [spdlog](https://github.com/gabime/spdlog) | 快速 C++ 日志库 | <https://github.com/gabime/spdlog> |
| [fmt](https://github.com/fmtlib/fmt) | C++ 格式化库（spdlog 依赖） | <https://github.com/fmtlib/fmt> |

> `spdlog` 和 `fmt` 由 xmake 内置仓库直接拉取，无需手动配置。

---

## 快速开始

1. 将 `xmake.lua` 中所有 `C:/third_party/` 开头的路径替换为你本机对应库的实际路径。
2. 在终端执行：

```bash
xmake build
```

3. 构建完成后运行：

```bash
xmake run
```

---

## 项目结构

```text
CrumEngine/
├── engine/          # 引擎核心 (C++ 模块)
│   └── core/        # 核心模块：Application 等
├── vendor/          # 第三方库的 C++20 模块封装
│   ├── imgui/       # Dear ImGui + SDL3 / Vulkan 后端
│   └── vulkan/      # Vulkan API 模块绑定
├── sandbox/         # 示例 / 测试应用
└── xmake.lua        # 构建配置
```

---

## 说明

- 本项目为学习目的而构建，使用了 AI 辅助编程，`vendor/` 下的 SDL 和 GLM 模块封装均由 AI 生成。
- 使用 C++20 模块（`.cppm`）作为第三方库的接口层，引擎本身以 C++23 标准编译。

