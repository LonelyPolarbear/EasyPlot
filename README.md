# EasyPlot

> 高性能 2D/3D 可视化与交互平台（基于 Qt + OpenGL）

## 项目概述

EasyPlot 是一个模块化、数据驱动的可视化与交互平台，专为科研、工程与教学场景设计。它提供了高性能、可扩展且工程化的 2D/3D 可视化基础设施，覆盖渲染、场景管理、对象/属性检查、日志与测试等核心能力。

## 目录结构

```
EasyPlot/
├── source/            # 主要源代码，按功能拆分为多个子模块
├── 3rdParty/          # 第三方依赖库
├── Python38/          # 内置 Python 解释器
├── config_cmake/      # CMake 配置脚本
├── doxygen1.16.1/     # 文档生成工具
├── examples/          # 示例代码
├── ext/               # 扩展工具
├── output/            # 构建产物
├── CMakeLists.txt     # 主构建脚本
└── README_NEW.md      # 项目说明文档
```

### 核心模块

| 模块 | 功能 | 描述 |
|------|------|------|
| **lib00_utilty** | 通用工具库 | 提供数学计算、矩阵操作、时间测量等基础工具 |
| **lib01_shader** | 着色器管理 | 负责着色器的加载、缓存与热重载 |
| **lib02_camera** | 相机与视图变换 | 提供透视和正交相机，支持视角变换和坐标转换 |
| **lib04_opengl** | OpenGL 资源封装 | 封装纹理、FBO、PBO 等 OpenGL 资源 |
| **lib05_shape** | 几何体与图形项 | 定义 2D/3D 图元，支持渲染节点层次结构 |
| **lib06_select** | 选择/拾取逻辑 | 实现点选和框选功能 |
| **lib07_scene** | 场景管理 | 场景容器，提供坐标变换和渲染排序 |
| **lib08_freetype** | 字体与 SDF 支持 | 字体渲染和 SDF 生成 |
| **lib09_panel** | 面板 | UI 面板和控件 |
| **XOpenGLWidget** | OpenGL 控件基类 | 基础 OpenGL 控件 |
| **easyPlot** | 主控件 | 核心渲染控件，集成场景管理和交互 |
| **render** | 渲染模块 | 渲染管线管理，支持 2D/3D 渲染 |
| **dataBase** | 数据管理 | 数据对象和属性管理，支持信号通知 |
| **xalgo** | 算法库 | 提供各种算法支持，如颜色插值、数组操作等 |
| **xlog** | 日志系统 | 统一日志接口，基于 spdlog |
| **xmvd** | 对象/属性视图 | 对象树、属性表和检查器 |
| **xpython** | Python 集成 | Python 脚本支持 |
| **xsignal** | 信号系统 | 自定义信号槽实现，基于 boost.signals2 |
| **xtest** | 测试框架 | 命令行测试应用框架 |
| **main** | 应用入口 | 主窗口和应用启动 |

## 技术栈

| 技术/库 | 用途 | 来源 |
|---------|------|------|
| Qt5 | UI 框架、事件处理 | 外部依赖 |
| OpenGL | 图形渲染 | 外部依赖 |
| Eigen3 | 线性代数 | 3rdParty/ |
| Boost | 通用库、信号系统 | 3rdParty/ |
| Freetype | 字体处理 | 3rdParty/ |
| Assimp | 3D 模型导入 | 3rdParty/ |
| GLEW | OpenGL 扩展 | 3rdParty/ |
| GLM | 数学库 | 3rdParty/ |
| HDF5 | 数据存储 | 3rdParty/ |
| spdlog | 日志系统 | 3rdParty/ |
| pybind11 | Python 绑定 | 3rdParty/ |
| Python 3.8 | 脚本集成 | Python38/ |

## 核心功能

### 1. 可视化能力

- **2D 绘图**：支持折线、柱状图、文本、矩形等 2D 图元
- **3D 渲染**：支持立方体、球体、锥体等 3D 几何体
- **网格与坐标轴**：提供 2D/3D 网格和坐标轴显示
- **颜色管理**：支持颜色插值和自定义颜色

### 2. 交互能力

- **相机控制**：支持旋转、平移、缩放操作
- **选择系统**：支持点选和框选功能
- **视图适配**：支持自动适配视图范围
- **鼠标事件**：完整的鼠标事件处理机制

### 3. 数据管理

- **数据对象**：树形结构组织数据
- **属性系统**：支持动态添加和修改属性
- **信号通知**：数据变化自动通知相关组件
- **序列化**：支持数据序列化和反序列化

### 4. 扩展能力

- **Python 集成**：支持 Python 脚本扩展
- **插件架构**：支持运行时扩展
- **命令行工具**：提供交互式命令行测试框架
- **代码生成**：基于 Clang 的代码分析和生成工具

## 构建与运行

### 先决条件

- Visual Studio 2022
- CMake ≥ 3.18
- Qt 5 开发包

### 快速构建

在项目根目录运行以下命令：

```bash
# 创建 Debug 配置（Visual Studio 2022）
create_debug_2022.bat

# 或创建 Release 配置（Visual Studio 2022）
create_release_2022.bat
```

### 手动构建

```bash
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -A x64
cmake --build . --config Debug
```

### 运行

构建完成后，可执行文件将输出到 `output/bin` 目录。

## 使用示例

### 1. 创建基本应用

```cpp
#include <QApplication>
#include <easyPlot/XEasyPlotWidget.h>

int main(int argc, char** argv) {
    QApplication a(argc, argv);
    
    // 创建主窗口
    QMainWindow window;
    
    // 创建 EasyPlot 控件
    XEasyPlotWidget* plotWidget = new XEasyPlotWidget(&window);
    window.setCentralWidget(plotWidget);
    
    // 显示窗口
    window.resize(800, 600);
    window.show();
    
    return a.exec();
}
```

### 2. 添加图形项

```cpp
// 添加 2D 折线
plotWidget->slotAddLine2D(0); // 0 表示正弦曲线

// 添加 3D 立方体
plotWidget->slotCreateCube();

// 添加文本
plotWidget->slotAddText();
```

### 3. 使用 Python 脚本

```cpp
#include <xpython/XPython.h>

// 执行 Python 代码
auto python = XPython::Instance();
python->execute("print('Hello from Python!')");
```

### 4. 使用命令行测试工具

```cpp
#include <xtest/XTest.h>

int main() {
    XTestApp app("MyTestApp");
    
    app.addCmd("test_command", "测试命令", []() {
        std::cout << "执行测试命令" << std::endl;
    });
    
    return app.run();
}
```

## 扩展与开发

### 1. 添加新的图形项

1. 继承 `XGraphicsItem` 或 `XRenderNode`
2. 实现必要的方法，如 `render()`
3. 在场景中添加图形项：

```cpp
auto scene = plotWidget->getScene();
auto shape = std::make_shared<MyCustomShape>();
scene->addShape(shape);
```

### 2. 自定义属性编辑器

1. 继承 `XAttrItemDelegate`
2. 注册到 `XAttrItemDelegateFactory`

### 3. 添加新的着色器

1. 创建顶点着色器和片段着色器文件
2. 使用 `xShaderManger` 加载着色器：

```cpp
auto shaderMgr = scene->getShaderManger();
auto shader = shaderMgr->createShader("vertex.glsl", "fragment.glsl");
```

## 项目特点

1. **模块化设计**：高度模块化的架构，便于维护和扩展
2. **数据驱动**：数据与渲染分离，使系统更加灵活和可测试
3. **高性能**：深度优化的 OpenGL 渲染，支持大数据场景
4. **工程化**：完善的构建系统、日志和测试框架
5. **可扩展**：支持 Python 脚本和插件架构

## 许可证

本项目采用 MIT 许可证，详见 `LICENSE` 文件。

## 贡献

欢迎提交 Issue 和 PR。贡献流程：
1. Fork 本仓库
2. 创建特性分支
3. 提交更改
4. 发起 PR

## 联系方式

如有问题或建议，请通过 GitHub Issues 与我们联系。