# EasyPlot

> High-performance 2D/3D visualization and interaction platform (Qt + OpenGL)

## Project Overview

EasyPlot is a modular, data-driven visualization and interaction platform designed for scientific research, engineering, and educational scenarios. It provides high-performance, extensible, and engineering-grade 2D/3D visualization infrastructure, covering rendering, scene management, object/property inspection, logging, and testing capabilities.

## Directory Structure

```
EasyPlot/
├── source/            # Main source code, organized into submodules
├── 3rdParty/          # Third-party dependency libraries
├── Python38/          # Built-in Python interpreter
├── config_cmake/      # CMake configuration scripts
├── doxygen1.16.1/     # Documentation generation tool
├── examples/          # Example code
├── ext/               # Extension tools
├── output/            # Build artifacts
├── CMakeLists.txt     # Main build script
└── README_NEW.md      # Project documentation
```

### Core Modules

| Module | Function | Description |
|--------|----------|-------------|
| **lib00_utilty** | Utility library | Provides basic tools for mathematical calculations, matrix operations, time measurement, etc. |
| **lib01_shader** | Shader management | Responsible for shader loading, caching, and hot-reloading |
| **lib02_camera** | Camera and view transforms | Provides perspective and orthographic cameras, supports view transformations and coordinate conversions |
| **lib04_opengl** | OpenGL resource wrapping | Wraps OpenGL resources such as textures, FBO, PBO, etc. |
| **lib05_shape** | Geometry and graphics items | Defines 2D/3D primitives, supports render node hierarchy |
| **lib06_select** | Selection/picking logic | Implements point selection and box selection functionality |
| **lib07_scene** | Scene management | Scene container providing coordinate transformations and render ordering |
| **lib08_freetype** | Font and SDF support | Font rendering and SDF generation |
| **lib09_panel** | Panels | UI panels and controls |
| **XOpenGLWidget** | OpenGL widget base class | Basic OpenGL widget |
| **easyPlot** | Main widget | Core rendering widget integrating scene management and interaction |
| **render** | Rendering module | Rendering pipeline management, supports 2D/3D rendering |
| **dataBase** | Data management | Data object and property management, supports signal notifications |
| **xalgo** | Algorithm library | Provides various algorithmic support, such as color interpolation, array operations, etc. |
| **xlog** | Logging system | Unified logging interface based on spdlog |
| **xmvd** | Object/property view | Object tree, property table, and inspector |
| **xpython** | Python integration | Python script support |
| **xsignal** | Signal system | Custom signal-slot implementation based on boost.signals2 |
| **xtest** | Testing framework | Command-line test application framework |
| **main** | Application entry | Main window and application startup |

## Technology Stack

| Technology/Library | Purpose | Source |
|-------------------|---------|--------|
| Qt5 | UI framework, event handling | External dependency |
| OpenGL | Graphics rendering | External dependency |
| Eigen3 | Linear algebra | 3rdParty/ |
| Boost | General-purpose library, signal system | 3rdParty/ |
| Freetype | Font processing | 3rdParty/ |
| Assimp | 3D model import | 3rdParty/ |
| GLEW | OpenGL extensions | 3rdParty/ |
| GLM | Mathematics library | 3rdParty/ |
| HDF5 | Data storage | 3rdParty/ |
| spdlog | Logging system | 3rdParty/ |
| pybind11 | Python binding | 3rdParty/ |
| Python 3.8 | Script integration | Python38/ |

## Core Features

### 1. Visualization Capabilities

- **2D Plotting**: Supports line charts, bar charts, text, rectangles, and other 2D primitives
- **3D Rendering**: Supports cubes, spheres, cones, and other 3D geometries
- **Grid and Axes**: Provides 2D/3D grid and axis display
- **Color Management**: Supports color interpolation and custom colors

### 2. Interaction Capabilities

- **Camera Control**: Supports rotation, translation, and zoom operations
- **Selection System**: Supports point selection and box selection functionality
- **View Fitting**: Supports automatic view range fitting
- **Mouse Events**: Complete mouse event handling mechanism

### 3. Data Management

- **Data Objects**: Tree-structured data organization
- **Property System**: Supports dynamic addition and modification of properties
- **Signal Notifications**: Automatic notification of data changes to related components
- **Serialization**: Supports data serialization and deserialization

### 4. Extension Capabilities

- **Python Integration**: Supports Python script extensions
- **Plugin Architecture**: Supports runtime extensions
- **Command-line Tools**: Provides interactive command-line testing framework
- **Code Generation**: Clang-based code analysis and generation tools

## Build and Run

### Prerequisites

- Visual Studio 2022
- CMake ≥ 3.18
- Qt 5 SDK

### Quick Build

Run the following command in the project root directory:

```bash
# Create Debug configuration (Visual Studio 2022)
create_debug_2022.bat

# Or create Release configuration (Visual Studio 2022)
create_release_2022.bat
```

### Manual Build

```bash
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -A x64
cmake --build . --config Debug
```

### Run

After building, the executable files will be output to the `output/bin` directory.

## Usage Examples

### 1. Creating a Basic Application

```cpp
#include <QApplication>
#include <easyPlot/XEasyPlotWidget.h>

int main(int argc, char** argv) {
    QApplication a(argc, argv);
    
    // Create main window
    QMainWindow window;
    
    // Create EasyPlot widget
    XEasyPlotWidget* plotWidget = new XEasyPlotWidget(&window);
    window.setCentralWidget(plotWidget);
    
    // Show window
    window.resize(800, 600);
    window.show();
    
    return a.exec();
}
```

### 2. Adding Graphics Items

```cpp
// Add 2D line plot
plotWidget->slotAddLine2D(0); // 0 represents sine curve

// Add 3D cube
plotWidget->slotCreateCube();

// Add text
plotWidget->slotAddText();
```

### 3. Using Python Scripts

```cpp
#include <xpython/XPython.h>

// Execute Python code
auto python = XPython::Instance();
python->execute("print('Hello from Python!')");
```

### 4. Using Command-line Testing Tool

```cpp
#include <xtest/XTest.h>

int main() {
    XTestApp app("MyTestApp");
    
    app.addCmd("test_command", "Test command", []() {
        std::cout << "Executing test command" << std::endl;
    });
    
    return app.run();
}
```

## Extension and Development

### 1. Adding New Graphics Items

1. Inherit from `XGraphicsItem` or `XRenderNode`
2. Implement necessary methods, such as `render()`
3. Add the graphics item to the scene:

```cpp
auto scene = plotWidget->getScene();
auto shape = std::make_shared<MyCustomShape>();
scene->addShape(shape);
```

### 2. Custom Property Editors

1. Inherit from `XAttrItemDelegate`
2. Register with `XAttrItemDelegateFactory`

### 3. Adding New Shaders

1. Create vertex shader and fragment shader files
2. Load shaders using `xShaderManger`:

```cpp
auto shaderMgr = scene->getShaderManger();
auto shader = shaderMgr->createShader("vertex.glsl", "fragment.glsl");
```

## Project Features

1. **Modular Design**: Highly modular architecture for easy maintenance and extension
2. **Data-driven**: Separation of data and rendering for greater flexibility and testability
3. **High Performance**: Deeply optimized OpenGL rendering for large data scenarios
4. **Engineering-grade**: Comprehensive build system, logging, and testing framework
5. **Extensible**: Support for Python scripts and plugin architecture

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.

## Contributing

Contributions are welcome. Contribution process:
1. Fork this repository
2. Create a feature branch
3. Commit your changes
4. Submit a PR

## Contact

For questions or suggestions, please contact us through GitHub Issues.