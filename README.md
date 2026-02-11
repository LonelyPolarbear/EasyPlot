

# EasyPlot

基于 Qt 和 OpenGL 的图形渲染框架，支持 2D/3D 图形绘制、交互操作及多种可视化效果。

## 项目简介

EasyPlot 是一个功能强大的图形渲染框架，基于 Qt 和 OpenGL 技术构建。该项目提供了丰富的图形绘制功能和可视化组件，支持创建复杂的 2D/3D 场景，具备良好的交互性和可扩展性。无论是科学数据可视化、工程制图还是游戏开发，EasyPlot 都能提供灵活的解决方案。

## 核心功能

### 图形渲染
- **2D/3D 图形渲染**：支持二维和三维图形的混合渲染，满足各种复杂场景需求
- **几何图形绘制**：内置多种基础几何体，包括立方体、圆柱体、圆锥体、球体等
- **自定义形状**：支持用户自定义顶点数据和纹理，创建独特的图形效果

### 数据可视化
- **折线图**：支持动态数据更新和交互式缩放平移
- **柱状图**：可配置颜色、间距和动画效果
- **散点图**：支持大数据量的高效渲染
- **图表组件**：提供坐标轴、图例、网格线等辅助组件

### 文本与字体
- **FreeType 字体渲染**：高质量的矢量字体渲染支持
- **SDF 距离场字体**：支持远处清晰显示的 SDF 技术
- **中文字体支持**：完善的 Unicode 和中文显示能力

### 图形效果
- **光照系统**：支持环境光、平行光、点光源等多种光照模型
- **材质系统**：支持 Phong、PBR 等多种材质效果
- **着色器编程**：灵活的 GLSL 着色器管理，支持自定义 Shader
- **计算着色器**：支持 GPU 并行计算加速

### 交互功能
- **相机控制**：轨道球相机操作，支持旋转、缩放、平移
- **图形拾选**：鼠标点击精确选择图形对象
- **UI 组件**：复选框、滑块、按钮等交互控件

## 项目结构

```
EasyPlot/
├── 3rdParty/                    # 第三方依赖库
│   ├── Eigen3/                  # 数学库（矩阵、向量计算）
│   ├── assimp/                  # 模型加载库（支持多种 3D 格式）
│   ├── freetype/                # 字体渲染库
│   ├── glm/                     # OpenGL 数学库
│   └── stb_image/               # 图像加载库
├── source/                      # 源代码目录
│   ├── dataBase/               # 数据基础类和结构
│   ├── easyPlot/               # 主要可视化组件
│   ├── lib00_utilty/           # 工具类和辅助函数
│   ├── lib01_shader/           # 着色器管理系统
│   ├── lib02_camera/           # 相机控制模块
│   ├── lib03_stbImage/         # 图像加载封装
│   ├── lib04_opengl/           # OpenGL 底层封装
│   └── lib05_shape/            # 图形对象定义
└── CMakeLists.txt              # CMake 构建配置
```

## 环境要求

- **Qt5/Qt6**：版本 5.15 或更高
- **CMake**：版本 3.16 或更高
- **C++ 编译器**：支持 C++17 的编译器（GCC 9+、MSVC 2019+、Clang 10+）
- **OpenGL**：支持 OpenGL 3.3 及以上版本
- **依赖库**：Eigen3、GLM、Assimp、FreeType、GLEW、stb_image

## 快速开始

### 编译项目

```bash
# 创建构建目录
mkdir build && cd build

# 配置 CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# 编译项目
cmake --build . --config Release
```

### 基础使用示例

```cpp
#include "easyPlotWidget.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // 创建主窗口
    easyPlotWidget plotWidget;
    plotWidget.show();
    
    // 添加 3D 立方体
    plotWidget.slotCreateCube();
    
    // 添加 2D 折线图
    plotWidget.slotAddLine2D(XGL::Line);
    
    return app.exec();
}
```

## 示例程序

项目包含多个示例程序，展示不同功能：

| 示例 | 功能说明 |
|------|---------|
| `01triangle` | 基础三角形渲染入门 |
| `02cube` | 简单立方体绘制 |
| `05cube/06cube` | 几何着色器高级效果 |
| `easyPlot` | 综合可视化示例 |
| `sdf` | SDF 距离场字体渲染 |
| `simsun_ttc` | 中文字体渲染示例 |

## API 概览

### 图形创建
- `slotCreateCube()` - 创建立方体
- `slotCreateCylinder()` - 创建圆柱体
- `slotCreateCone()` - 创建圆锥体
- `slotCreateSphere()` - 创建球体

### 图表操作
- `slotAddLine2D()` - 添加折线图
- `slotAddBarChart()` - 添加柱状图
- `slotSetData()` - 更新图表数据
- `slotClear()` - 清空所有图形

### 相机控制
- `camera.setDistance()` - 设置观察距离
- `camera.setRotation()` - 设置旋转角度
- `camera.lookAt()` - 设置观察点

## 技术特点

### 高性能渲染
- **多级缓冲**：支持多级帧缓冲和缓存机制
- **实例化渲染**：高效渲染大量相同几何体
- **LOD 支持**：根据距离动态调整细节层次

### 灵活扩展
- **模块化设计**：各模块独立，可按需使用
- **插件机制**：支持运行时加载扩展功能
- **事件系统**：完善的事件处理和分发机制

### 跨平台支持
- **Windows**：完整的 Visual Studio 支持
- **Linux**：GCC/Clang 编译器支持
- **macOS**：Metal 和 OpenGL 双后端支持

## 许可证

本项目采用开源许可证，具体信息请查看项目根目录下的 LICENSE 文件。

## 贡献指南

欢迎社区贡献代码和文档：

1. Fork 本项目
2. 创建特性分支 (`git checkout -b feature/amazing-feature`)
3. 提交更改 (`git commit -m 'Add amazing feature'`)
4. 推送到分支 (`git push origin feature/amazing-feature`)
5. 创建 Pull Request

## 致谢

感谢以下开源项目为本项目提供的支持：

- [Qt](https://www.qt.io/) - 跨平台 GUI 框架
- [OpenGL](https://www.opengl.org/) - 图形渲染 API
- [Eigen](https://eigen.tuxfamily.org/) - C++ 线性代数库
- [GLM](https://glm.g-truc.net/) - OpenGL 数学库
- [Assimp](https://github.com/assimp/assimp) - 模型加载库
- [FreeType](https://www.freetype.org/) - 字体渲染库
- [GLEW](http://glew.sourceforge.net/) - OpenGL 扩展库
- [stb](https://github.com/nothings/stb) - 单文件图像库

## 联系方式

- 项目主页：[Gitee - EasyPlot](https://gitee.com/LonelyPolarbear/EasyPlot)
- 问题反馈：请在项目页面提交 Issue

---

*Happy Plotting! 🎨*