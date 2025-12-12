# QtOpenGL

这是一个使用 Qt 和 OpenGL 实现的图形渲染项目，支持 2D 和 3D 图形绘制、交互操作以及多种可视化效果。

## 项目概述

QtOpenGL 是一个基于 Qt 和 OpenGL 的图形渲染框架，提供了丰富的图形绘制功能和可视化组件。该项目支持创建复杂的 2D/3D 场景，并具备良好的交互性和可扩展性。

## 功能特性

- 支持 2D 和 3D 图形渲染
- 提供多种几何图形绘制（立方体、圆柱体、圆锥体等）
- 支持图表可视化（折线图、柱状图等）
- 具备文本渲染功能
- 支持光照和材质效果
- 提供相机控制和交互操作
- 支持着色器编程和计算着色器
- 包含多种 UI 组件（复选框、图例等）

## 依赖库

- Qt5/Qt6
- OpenGL
- Eigen3
- GLM
- Assimp (用于模型加载)
- FreeType (用于字体渲染)
- GLEW (用于 OpenGL 扩展管理)
- stb_image (用于图像加载)

## 目录结构

```
.
├── 3rdParty/            # 第三方库
│   ├── Eigen3/            # Eigen 数学库
│   ├── assimp/            # 模型加载库
│   ├── freetype/          # 字体渲染库
│   ├── glm/               # GLM 数学库
│   └── stb_image/         # 图像加载库
├── source/                # 源代码
│   ├── dataBase/          # 数据基础类
│   ├── easyPlot/          # 主要可视化组件
│   ├── lib00_utilty/      # 工具类
│   ├── lib01_shader/      # 着色器管理
│   ├── lib02_camera/      # 相机控制
│   ├── lib03_stbImage/    # 图像加载
│   ├── lib04_opengl/      # OpenGL 封装
│   └── lib05_shape/       # 图形对象和可视化组件
└── CMakeLists.txt         # 构建配置
```

## 使用方法

### 构建项目

1. 安装 Qt 开发环境
2. 安装 CMake
3. 配置构建环境
4. 使用 CMake 生成构建文件
5. 编译项目

### 示例代码

```cpp
#include "easyPlotWidget.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    easyPlotWidget plotWidget;
    plotWidget.show();
    
    // 添加一个立方体
    plotWidget.slotCreateCube();
    
    // 添加折线图
    plotWidget.slotAddLine2D(XGL::Line);
    
    return app.exec();
}
```

## 示例功能

- `01triangle`: 基础三角形渲染
- `02cube`: 立方体渲染
- `05cube/06cube`: 包含几何着色器的立方体渲染
- `easyPlot`: 综合可视化示例，包含图表、文本、拾取等功能
- `sdf`: 距离场字体渲染示例
- `simsun_ttc`: 中文字体渲染示例

## 特色功能

- **图形拾取**：支持鼠标点击选择图形对象
- **字体渲染**：支持常规字体和 SDF 距离场字体渲染
- **图表可视化**：提供 2D 折线图、柱状图等
- **相机控制**：支持轨道球相机操作
- **着色器系统**：灵活的着色器管理和使用
- **多级渲染**：支持多级缓存和帧缓冲对象

## 许可证

该项目使用开源许可证，请参阅项目根目录下的 LICENSE 文件了解详细信息。

## 贡献指南

欢迎贡献代码和文档。请遵循以下步骤：

1. Fork 项目
2. 创建新分支
3. 提交更改
4. 创建 Pull Request

## 问题反馈

如遇到任何问题，请在项目页面提交 issue。

## 致谢

感谢以下开源项目对本项目的贡献：

- [Qt](https://www.qt.io/)
- [OpenGL](https://www.opengl.org/)
- [Eigen](https://eigen.tuxfamily.org/)
- [GLM](https://glm.g-truc.net/)
- [Assimp](https://github.com/assimp/assimp)
- [FreeType](https://www.freetype.org/)
- [GLEW](http://glew.sourceforge.net/)
- [stb_image](https://github.com/nothings/stb)

如需更多帮助，请参阅项目文档或联系开发者。