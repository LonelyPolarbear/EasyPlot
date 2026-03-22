# EasyPlot

> High-performance 2D/3D visualization and interaction platform (Qt + OpenGL)

## Contents

- Overview
- Author's Design Philosophy
- Architecture & Modules
- Core Classes & Implementation Highlights
- Detailed Modules (xmvd / xlog / xtest)
- Dependencies
- Build & Run
- Quick Start
- Development & Extension Guide
- Contributing
- License

---

## Overview

EasyPlot is a modular visualization and interaction toolkit designed by the author for research, engineering, and teaching scenarios. It aims to provide a performant, extensible and engineering-friendly infrastructure for 2D/3D plotting, scene management, object/property inspection, logging and testing.

## Author's Design Philosophy

The author's core ideas: modularization + data-driven + engineering-grade.

- Modular layers: each capability lives in its own library (e.g. `lib05_shape`, `lib07_scene`) to reduce coupling and ease reuse;
- Data-driven: rendering and data are decoupled; graphics objects are driven by data (`XData*`, `XGraphicsItem`, `XGeometryNode`);
- Engineering practices: CMake-based project, clear dependency declaration, built-in logging and tests (`xlog`, `xtest`);
- Extensibility: Qt signals/slots plus custom `xsignal`, runtime factories (e.g. `XAttrItemDelegateFactory`);
- UX focused: interactions (rotate/translate/zoom/box select/context menu), font SDF rendering, screenshot, asynchronous loading.

The author emphasizes long-term maintainability and engineering quality over short-term feature accumulation.

## Architecture & Modules

Top-level folders and core modules:

- `source/`: main source code organized into submodules;
- `3rdParty/`: third-party libraries (Boost, Eigen, Freetype, stb_image, etc.);
- `output/`: built artifacts;
- `build/`: generated CMake / Visual Studio files;
- `config_cmake/`: reusable CMake helpers.

Key submodules (summary):

- `lib00_utilty`: utilities;
- `lib01_shader`: shader manager;
- `lib02_camera`: camera and view transforms;
- `lib04_opengl`: OpenGL resource wrappers (textures / FBO / PBO);
- `lib05_shape`: geometry and graphic items;
- `lib06_select`: selection / picking;
- `lib07_scene`: scene management;
- `lib08_freetype`: font and SDF support;
- `xmvd`: data/object viewer and inspector (tree, table, inspector);
- `xlog`: logging system;
- `xtest`: examples and tests;
- `XOpenGLWidget`: OpenGL widget base class.

## Core Classes & Implementation Highlights

- `easyPlotWidget` (`source/easyPlot/easyPlotWidget.{h,cpp}`): main widget that handles render loop, events and scene interactions (add/remove items, picking, box selection).
- `XScene`: scene container providing coordinate transforms, picking, render ordering and camera control.
- `xShaderManger`: centralized shader loading, caching and hot-reload.
- `XOpenGLTexture` / `XOpenGLFramebufferObject` / `XOpenGLBuffer`: wrappers for GPU resources with examples in `slotFboTest`.
- `XGraphicsItem` subclasses (`XRectItem`, `XTextItem`, `XLineItem`, `XChartItem`): 2D/3D primitives supporting transforms and attributes.
- `xmvd` components (`XDataObjectTreeView`, `XDataObjectTableView`, `XObjectInspectorView`): runtime UI for data object inspection and editing.

Highlights:

- Extensive use of OpenGL features (texture arrays, depth/stencil separation, PBO mapping) to optimize large-data rendering and GPU-CPU transfers;
- Asynchronous SDF/font generation using `QtConcurrent` for better responsiveness;
- Full data-render decoupling enabling unit testing and modular replacement;
- Unified picking/box-selection supporting mixed 2D/3D selections at the scene layer.

## Detailed Modules (xmvd / xlog / xtest)

### xmvd

Location: `source/xmvd/`

Purpose: object/property visualization and editing UI (object tree, property table, inspector). It maps `XDataObject` and `XDataAttribute` to editable UI widgets and supports runtime extension via delegate factories and `xsignal` for inter-component communication. xmvd is a developer-focused toolset for runtime inspection and editing, not an algorithm library.

### xlog

Location: `source/xlog/`

Purpose: unified logging API (integrates with `spdlog`), multi-level log output for debugging and runtime tracing.

### xtest

Location: `source/xtest/`

Purpose: example apps and tests for regression and demonstrations; helps new contributors understand module usage.

## Dependencies

- Qt5 (Core, Widgets, Concurrent, Gui)
- OpenGL (glew, opengl32)
- Eigen3
- Boost
- Freetype
- Assimp
- HDF5 (optional)
- spdlog, MagicEnum, stb_image

Note: many dependencies are supplied in `3rdParty/`; platform-specific configuration may be required for optional components.

## Build & Run (Windows / Visual Studio)

Prerequisites: Visual Studio 2019/2022, CMake >= 3.18, Qt SDK.

Quick build (provided scripts):

```bat
create_debug_2022.bat
```

Manual CMake steps:

```bat
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -A x64
cmake --build . --config Debug
```

Artifacts appear under `output/bin`.

## Quick Start

1. Include `easyPlotWidget` or `XEasyPlotWidget` in your Qt app.
2. Use `d->scene->addGraphicsItem(...)` or `d->scene->addShape(...)` to manage items.
3. Bind an `XDataObject` to `XObjectInspectorView` to inspect/edit properties at runtime.
4. Use `xfreetype::Instance()` for font/SDF texture generation.

## Development & Extension Guide

- Add features as independent modules following `config_cmake` templates;
- Register property editors with `XAttrItemDelegateFactory`;
- Manage shaders via `xShaderManger` and support hot-reload during development;
- For large datasets, prefer batched texture/buffer uploads and PBO mapping to minimize copies.

## Contributing

Please open issues or PRs. Typical flow: Fork -> branch -> commit -> PR. Describe feature, usage example and screenshots if applicable.

## License

This project is licensed under the MIT License. See `LICENSE` for details.

---

If you want, I can split module-level READMEs, generate class diagrams, or create small example projects demonstrating common workflows.