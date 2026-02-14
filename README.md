# Easyplot

## 简介

这是一个功能完善、模块化设计的 3D/2D 渲染框架，采用分层架构，从底层基础工具到高层应用逐步构建。框架集成了 OpenGL 资源管理、几何数据处理、相机控制、交互事件处理、拾取、字体渲染、图像加载等功能，并提供了 Qt 集成部件，便于快速开发科学可视化、CAD 或游戏工具。

## 主要特性

- **分层模块化**：各模块职责清晰，依赖关系明确，可独立替换或升级。
- **数据驱动**：基于时间戳的增量更新机制，避免不必要的计算和数据上传。
- **可观察对象**：统一的属性系统（`XDataAttribute`）和对象树（`XDataObject`），支持属性变化信号。
- **灵活相机**：支持透视/正交投影、轨道球/直立旋转两种交互风格，并提供丰富的坐标系转换函数。
- **OpenGL 资源 RAII**：Buffer、Texture、FBO、VAO 等均封装为 RAII 对象，自动管理生命周期，支持状态追踪。
- **几何管线**：数据源（`XShapeSource`）- 过滤器（变换/合并）- 映射器（`XPolyDataMapper`）- 渲染节点（`XGeometryNode`）的完整管线，支持程序化几何生成。
- **2D/3D 节点体系**：统一的场景图（父子关系），支持变换、可见性、渲染属性等。
- **交互事件处理**：多模式事件处理器（相机漫游、拾取、操纵器），支持信号槽连接。
- **拾取支持**：基于深度剥离的像素级精确拾取，返回对象 ID 和图元 ID。
- **图像加载**：基于 stb_image 的简单图像加载工具，支持 2D 纹理和立方体贴图。
- **字体处理**：基于 FreeType 的离线工具，生成普通/SDF 图集，提供字形度量信息。
- **算法工具**：提供常用数学算法（线性插值、布局计算、几何相交、数组合并等）。
- **Qt 集成**：`XGLWidget` 作为 Qt 部件，轻松嵌入 Qt 应用程序。
- **示例应用**：`EasyPlot` 部件展示多视口、实时更新、视锥体可视化等核心能力。

## 模块列表

| 模块 | 描述 |
|------|------|
| `lib00_utilty` | 基础工具库：数学工具、并行算法、typelist、工厂模板、类型萃取等 |
| `xsignal` | 信号槽库，基于 Boost.Signals2，支持智能指针 |
| `xalgo` | 算法工具：线性插值、布局计算、线段与平面交点、视锥与 XOZ 平面相交、循环索引、数组合并等 |
| `dataBase` | 数据基础：对象模型、属性系统、多维数组、时间戳、数学向量 |
| `lib02_camera` | 相机模块：`XBaseCamera` 抽象，`XTrackBallCamera` 实现 |
| `lib03_stbImage` | 图像加载：基于 stb_image 加载 2D 纹理和立方体贴图，返回 OpenGL 纹理 ID |
| `lib04_opengl` | OpenGL 资源封装：Buffer、Texture、FBO、VAO、Context |
| `lib05_shape` | 几何数据与渲染节点：数据源、过滤器、`XGeometryNode`、`XGraphicsItem` |
| `lib08_freetype` | 字体处理：生成图集，提供字形信息 |
| `render` | 渲染驱动：窗口、视口、事件分发、拾取 |
| `XOpenGLWidget` | Qt 集成部件 |
| `easyPlot` | 示例应用部件，演示框架功能 |

## 构建要求

- **编译器**：支持 C++17 的编译器（如 MSVC 2019+、GCC 9+、Clang 10+）
- **依赖库**：
  - Eigen3（数学库）
  - Boost（仅 Signals2）
  - GLEW
  - FreeType（可选，用于字体处理）
  - stb_image（已包含在 3rdParty 中）
  - Qt5（Core、Widgets、Concurrent，用于集成层和示例）
- **构建系统**：CMake 3.12+

## 快速开始

### 1. 克隆项目并初始化子模块（如果有）
```bash
git clone <repository-url>
cd <project-root>
```
### 2.配置依赖
确保已安装上述依赖，并设置好环境变量（如 `Qt5_DIR`、`Boost_ROOT`、`EIGEN3_INCLUDE_DIR` 等）。

### 3. 使用 CMake 构建
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```
### 4. 运行示例

构建后，在 `output/bin` 目录下生成动态库和可执行文件（若存在示例应用程序）。可运行 `easyPlot` 的测试程序查看效果。

### 5.创建渲染窗口并添加一个立方体

```c++
#include <XOpenGLWidget/XGLWidget.h>
#include <render/XRender.h>
#include <lib05_shape/XGeometryNode.h>
#include <lib05_shape/datasource/xchamferCubeSource.h>
// 创建 Qt 部件
XGLWidget* widget = new XGLWidget(parent);
auto renderWindow = widget->getRenderWindow();
// 创建渲染器（视口）
auto render = makeShareDbObject<XRender>();
renderWindow->addRender(render);
// 创建立方体数据源
auto cubeSource = makeShareDbObject<xchamferCubeSource>();
cubeSource->Modified();
// 创建几何节点
auto cubeNode = makeShareDbObject<XGeometryNode>();
cubeNode->setInput(cubeSource);
cubeNode->setColorMode(ColorMode::FaceColor);
cubeNode->scale(100, 100, 100);
// 添加到渲染器
render->addRenderNode3D(cubeNode);
// 调整相机视角
render->fitView();
```
### 5.响应鼠标事件
```c++
// 在自定义部件中连接信号
connect(renderWindow->getEventDispatcher(), &XRenderWindowEventDispatch::SigMouseMove,
        [](XQ::Vec2i pos, XQ::KeyboardModifier mod) {
            // 处理鼠标移动
        });
```

### 6.加载纹理

```c++
unsigned int texID = stbImage::loadTexture2D("path/to/image.png");
```

### 7.生成字体图集（离线工具）
```c++
xfreetype* ft = xfreetype::Instance();
ft->generateFontSdf("output_font_dir", false, false);  // 生成 SDF 图集
```
## 贡献指南

欢迎提交 issue 和 pull request。请确保代码风格一致，并添加必要的单元测试。

