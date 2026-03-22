# EasyPlot

> 高性能 2D / 3D 可视化与交互平台（基于 Qt + OpenGL）

## 目录

- 项目概述
- 作者设计理念
- 架构与模块
- 关键类与实现亮点
- 模块详细说明（xmvd / xlog / xtest）
- 依赖
- 构建与运行
- 使用说明（快速上手）
- 开发与扩展指南
- 贡献
- 许可证

---

## 项目概述

EasyPlot 是作者为科研、工程与教学场景设计的模块化可视化与交互组件集合，目标是提供高性能、可扩展且工程化的 2D/3D 可视化基础设施，覆盖渲染、场景管理、对象/属性检查、日志与测试等能力。

## 作者设计理念

作者的核心思路：模块化 + 数据驱动 + 工程化。

- 模块化分层：每个功能拆分为单独库（例：`lib05_shape`、`lib07_scene`），降低耦合，便于维护与重用；
- 数据驱动：渲染层与数据层解耦，图形对象由数据驱动（`XData*`、`XGraphicsItem`、`XGeometryNode`）。
- 工程化：使用 CMake 管理项目，配置清晰，内置日志与测试（`xlog`、`xtest`）提高可靠性；
- 可扩展性：采用 Qt 信号槽与自定义 `xsignal`，并以工厂模式（如 `XAttrItemDelegateFactory`）支持运行时扩展；
- 用户体验：重视交互（旋转/平移/缩放/框选/右键菜单）、字体渲染（SDF）、截屏与异步加载。

作者偏重长期可维护性与工程实践，而非短期功能堆叠，因此代码结构、模块边界与构建脚本都体现“可演进”的设计。

## 架构与模块

顶层目录与核心模块：

- `source/`：主要源代码，按功能拆分子模块；
- `3rdParty/`：第三方依赖（包含 Boost、Eigen、Freetype、stb_image 等）；
- `output/`：构建产物；
- `build/`：CMake / VS 生成文件；
- `config_cmake/`：复用的 CMake 脚本。

主要子模块（摘要）：

- `lib00_utilty`：通用工具；
- `lib01_shader`：着色器管理；
- `lib02_camera`：相机与视图变换；
- `lib04_opengl`：OpenGL 资源封装（纹理 / FBO / PBO）；
- `lib05_shape`：几何体与图形项；
- `lib06_select`：选择/拾取逻辑；
- `lib07_scene`：场景管理；
- `lib08_freetype`：字体与 SDF 支持；
- `xmvd`：对象/属性视图（对象树、属性表、检查器）；
- `xlog`：日志系统；
- `xtest`：示例与测试用例；
- `XOpenGLWidget`：OpenGL 控件基类。

## 关键类与实现亮点

- `easyPlotWidget`（`source/easyPlot/easyPlotWidget.{h,cpp}`）：主控件，负责渲染循环、事件处理、场景交互（添加/删除图元、拾取、框选等）；
- `XScene`：场景容器，提供坐标变换、拾取、渲染排序和相机控制接口；
- `xShaderManger`：统一着色器加载、缓存、管理与热重载；
- `XOpenGLTexture` / `XOpenGLFramebufferObject` / `XOpenGLBuffer`：封装了纹理、FBO、PBO 的创建与读写，示例见 `slotFboTest`；
- `XGraphicsItem` 及其子类（`XRectItem`、`XTextItem`、`XLineItem`、`XChartItem`）：2D/3D 图元，支持层级、变换与属性；
- `xmvd` 中的 `XDataObjectTreeView` / `XDataObjectTableView` / `XObjectInspectorView`：构建对象/属性的可视化与编辑面板。

实现亮点：

- 深度使用 OpenGL 特性（纹理数组、深度/模板测试、PBO 映射）以优化大数据渲染与读回；
- 将字体渲染与 SDF 生成异步化（`QtConcurrent`），提升启动/运行体验；
- 数据与渲染完全解耦，便于单元测试与模块替换；
- 场景层实现统一的点选/框选机制，支持 2D 与 3D 混合选择。

## 模块详细说明（xmvd / xlog / xtest）

### xmvd

位置：`source/xmvd/`

作用：对象/属性可视化与编辑面板。实现对象树（`XDataObjectTreeView`）、属性表（`XDataObjectTableView`）与检查器（`XObjectInspectorView`），支持：

- 将 `XDataObject` 映射为树形结构并显示属性；
- 使用 `XAttrItemDelegateFactory` 注册不同类型的属性编辑器（运行时扩展）；
- 基于 `xsignal` 做视图间联动（当前选中对象变化时同步属性表）。

结论：`xmvd` 不是算法模块，而是工程化的“对象与属性可视化/编辑”模块，便于调试、运行时检查与交互式修改数据。

### xlog

位置：`source/xlog/`

作用：统一日志接口（集成 `spdlog`），提供多级别日志记录与输出策略，便于定位渲染与运行时问题。

### xtest

位置：`source/xtest/`

作用：示例与测试用例集合，用于回归测试与功能演示，帮助新开发者理解模块用法。

## 依赖

- Qt5 (Core, Widgets, Concurrent, Gui)
- OpenGL (glew, opengl32)
- Eigen3
- Boost
- Freetype
- Assimp
- HDF5（可选）
- spdlog, MagicEnum, stb_image

（仓库内 `3rdParty/` 包含多数依赖，部分平台或配置可能需要额外安装）

## 构建与运行（Windows / Visual Studio）

先决条件：Visual Studio 2019/2022、CMake ≥ 3.18、Qt 开发包。

快速构建示例（在仓库根目录）：

```bat
create_debug_2022.bat
```

或使用 CMake 手动生成：

```bat
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -A x64
cmake --build . --config Debug
```

构建产物会输出到 `output/bin`。

## 使用说明（快速上手）

1. 在 Qt 应用中包含 `easyPlotWidget` 或 `XEasyPlotWidget`；
2. 通过 `d->scene->addGraphicsItem(...)` 或 `d->scene->addShape(...)` 管理图元；
3. 使用 `XObjectInspectorView`（`xmvd`）绑定 `XDataObject`，实时查看/编辑属性；
4. 字体与 SDF：通过 `xfreetype::Instance()` 生成并加载字体纹理。

## 开发与扩展指南

- 新功能以独立模块实现，遵循 `config_cmake` 下的模版；
- 属性编辑器注册到 `XAttrItemDelegateFactory`；
- Shader 由 `xShaderManger` 管理，建议集中维护并支持热重载；
- 大数据渲染使用纹理/缓冲分批上传与 PBO 映射以减少拷贝开销。

## 贡献

欢迎提交 Issue 与 PR。流程：Fork -> 分支 -> Commit -> PR，PR 请包含功能描述、示例与必要说明。

## 许可证

本项目采用 MIT 许可证，详见 `LICENSE` 文件。

---

如需我将 README 分拆为多份模块级文档、生成类图或示例工程，我可以继续为你自动生成。
