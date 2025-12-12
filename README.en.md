# QtOpenGL

This is a graphics rendering project implemented using Qt and OpenGL, supporting 2D and 3D graphics drawing, interactive operations, and various visualization effects.

## Project Overview

QtOpenGL is a graphics rendering framework based on Qt and OpenGL, providing rich graphics drawing capabilities and visualization components. This project supports creating complex 2D/3D scenes with excellent interactivity and extensibility.

## Features

- Supports 2D and 3D graphics rendering
- Provides various geometric shapes (cube, cylinder, cone, etc.)
- Supports chart visualization (line charts, bar charts, etc.)
- Includes text rendering functionality
- Supports lighting and material effects
- Provides camera control and interactive operations
- Supports shader programming and compute shaders
- Includes multiple UI components (checkboxes, legends, etc.)

## Dependencies

- Qt5/Qt6
- OpenGL
- Eigen3
- GLM
- Assimp (for model loading)
- FreeType (for font rendering)
- GLEW (for OpenGL extension management)
- stb_image (for image loading)

## Directory Structure

```
.
├── 3rdParty/            # Third-party libraries
│   ├── Eigen3/            # Eigen mathematical library
│   ├── assimp/            # Model loading library
│   ├── freetype/          # Font rendering library
│   ├── glm/               # GLM mathematical library
│   └── stb_image/         # Image loading library
├── source/                # Source code
│   ├── dataBase/          # Data base classes
│   ├── easyPlot/          # Main visualization components
│   ├── lib00_utilty/      # Utility classes
│   ├── lib01_shader/      # Shader management
│   ├── lib02_camera/      # Camera control
│   ├── lib03_stbImage/    # Image loading
│   ├── lib04_opengl/      # OpenGL wrapper
│   └── lib05_shape/       # Graphic objects and visualization components
└── CMakeLists.txt         # Build configuration
```

## Usage

### Building the Project

1. Install the Qt development environment
2. Install CMake
3. Configure the build environment
4. Use CMake to generate build files
5. Compile the project

### Example Code

```cpp
#include "easyPlotWidget.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    easyPlotWidget plotWidget;
    plotWidget.show();
    
    // Add a cube
    plotWidget.slotCreateCube();
    
    // Add a line chart
    plotWidget.slotAddLine2D(XGL::Line);
    
    return app.exec();
}
```

## Example Features

- `01triangle`: Basic triangle rendering
- `02cube`: Cube rendering
- `05cube/06cube`: Cube rendering with geometry shaders
- `easyPlot`: Comprehensive visualization example including charts, text, and picking
- `sdf`: SDF font rendering example
- `simsun_ttc`: Chinese font rendering example

## Key Features

- **Object Picking**: Supports mouse click selection of graphical objects
- **Font Rendering**: Supports standard fonts and SDF distance-field fonts
- **Chart Visualization**: Provides 2D line charts, bar charts, etc.
- **Camera Control**: Supports orbit camera controls
- **Shader System**: Flexible shader management and usage
- **Multi-level Rendering**: Supports multi-level caching and framebuffer objects

## License

This project uses an open-source license. Please refer to the LICENSE file in the project root directory for details.

## Contribution Guidelines

Contributions of code and documentation are welcome. Please follow these steps:

1. Fork the project
2. Create a new branch
3. Commit your changes
4. Submit a Pull Request

## Feedback

If you encounter any issues, please submit an issue on the project page.

## Acknowledgments

Thanks to the following open-source projects for their contributions to this project:

- [Qt](https://www.qt.io/)
- [OpenGL](https://www.opengl.org/)
- [Eigen](https://eigen.tuxfamily.org/)
- [GLM](https://glm.g-truc.net/)
- [Assimp](https://github.com/assimp/assimp)
- [FreeType](https://www.freetype.org/)
- [GLEW](http://glew.sourceforge.net/)
- [stb_image](https://github.com/nothings/stb)

For further assistance, please refer to the project documentation or contact the developers.