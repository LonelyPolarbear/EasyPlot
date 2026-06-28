# EasyPlot

工业级混合 2D/3D 科学可视化平台

---

## 📖 简介

EasyPlot 是一个从零构建的高性能 2D/3D 科学数据可视化平台。它不仅仅是一个简单的 3D 渲染器，更是一套完整的**工业级图形引擎**，具备场景图管理、动态属性系统、实时交互操作柄、高质量 SDF 文字渲染、2D 曲线图表以及 Python 脚本扩展等核心能力。

该项目专为**科学计算、工程仿真、数据分析**等领域的可视化需求而设计，力求在**性能、扩展性与易用性**之间取得最佳平衡。

---

## ✨ 核心特性

- **混合 2D/3D 渲染管线**  
  独立的场景图分支与专用渲染 Pass，实现 2D HUD/图表与 3D 场景的无缝叠加。
- **自研运行时元对象系统**  
  基于 C++ 模板元编程与 Boost.Describe，实现类名注册、继承链追踪、工厂创建和枚举反射，支撑序列化与 UI 自动绑定。
- **动态属性系统与双向数据绑定**  
  支持任意类型属性的动态挂载，属性变化自动触发信号，UI 自动刷新；反之 UI 修改自动写回对象。
- **现代 OpenGL 底层封装**  
  全面封装 OpenGL 4.6 核心特性：纹理（含 MSAA）、缓冲（UBO/SSBO/PBO）、FBO、VAO、上下文管理、状态机保存/恢复、同步对象等。
- **高质量 SDF 文字渲染**  
  基于 FreeType 生成有符号距离场图集，实现任意缩放下的清晰无锯齿文字渲染。
- **2D 曲线图表系统**  
  支持缩放、平移交互的图表节点，使用几何着色器实现固定像素宽度的抗锯齿曲线，支持多种线型。
- **多模式交互处理器**  
  集成相机导航、物体拾取、3D 操作柄（平移/旋转/缩放）等多种交互模式，支持模式组合与快捷键切换。
- **属性检视器（MVD 模式）**  
  基于 Qt Model/View/Delegate 构建的属性面板，支持颜色、枚举、布尔等类型的自定义编辑器，并可动态扩展。
- **Python 脚本支持**  
  通过 pybind11 嵌入 Python 解释器，提供交互式控制台与脚本执行能力，支持二次开发。

---

## 🏗️ 架构总览

EasyPlot 采用**严格的分层与接口分离**设计，自上而下分为 5 层：

| 层级 | 模块 | 职责 |
| :--- | :--- | :--- |
| **应用层** | `main`, `XMainWindow`, `easyPlot` | Qt 界面集成，菜单/工具栏，主窗口布局，渲染视图封装。 |
| **渲染引擎层** | `render`, `xopenglwidget`, `xbaserender` | 多视口渲染管线、MSAA + MRT、SMAA 后处理、交互处理器、场景图遍历。 |
| **场景图形层** | `xrendernode`, `lib02_camera` | 场景图节点（几何体/文本/网格/操作柄/图表）、相机模型、属性/状态管理、数据源与映射器。 |
| **图形 API 层** | `lib04_opengl`, `lib01_shader`, `lib03_stbImage` | OpenGL 对象封装、着色器编译与 Uniform 管理、纹理加载。 |
| **基础设施层** | `dataBase`, `lib00_utilty`, `xsignal`, `xlog`, `xcodecvt`, `xpython`, `xmvd` | 对象模型、属性系统、信号槽、日志、编码转换、Python 集成、属性检视器。 |

各层之间通过**抽象接口**和**信号槽**解耦，保证了系统的可维护性与可扩展性。

---

## 🛠️ 技术栈

| 领域 | 技术 |
| :--- | :--- |
| **语言标准** | C++17 |
| **GUI 框架** | Qt 5.15 (Widgets, Core, Concurrent) |
| **图形 API** | OpenGL 4.6 Core Profile |
| **扩展加载** | GLEW + WGL |
| **线性代数** | Eigen 3.4 |
| **字体引擎** | FreeType 2.10+ (支持 SDF) |
| **信号槽** | Boost.Signals2 (自研封装) |
| **日志系统** | spdlog (自研转发 Sink) |
| **序列化** | HDF5 (HighFive 封装) |
| **Python 绑定** | pybind11 2.10+ |
| **图像加载** | stb_image + SOIL2 |
| **构建工具** | CMake 3.8+ |

---

## 🚀 快速开始

### 环境要求

- Windows 10/11（当前主要支持平台，后续将支持 Linux）
- Visual Studio 2022（MSVC 143）或 MinGW-w64
- CMake 3.8+
- Qt 5.15.2
- Python 3.8（可选，用于脚本支持）

### 构建步骤

```bash
git clone https://gitee.com/LonelyPolarbear/EasyPlot.git
cd EasyPlot
cmake -B build -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 17 2022"
cmake --build build --config Release
```

### 示例

![多窗口](https://gitee.com/LonelyPolarbear/EasyPlot/blob/master/assets/multiViewPort.png)
![2D 3D渲染](https://gitee.com/LonelyPolarbear/EasyPlot/blob/master/assets/chart.gif)