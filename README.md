# CrumEngine

一个 C++23 模块构建的游戏引擎

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

## 说明

- 本项目为学习目的而构建，使用 AI 辅助编程。`vendor/` 下的 SDL / GLM / Vulkan / ImGui 模块封装均由 AI 生成。
- 引擎以 C++23 标准编译，使用 C++20 模块（`.cppm`）组织代码。
