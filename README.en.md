# EasyPlot

An industrial-grade, mixed 2D/3D scientific visualization platform.

---

## 📖 Introduction

EasyPlot is a high-performance 2D/3D scientific data visualization platform built from the ground up. It is more than just a simple 3D renderer; it is a complete **industrial-grade graphics engine**. It features a scene graph, a dynamic property system, real-time interactive manipulators, high-quality SDF text rendering, 2D charting, and Python scripting capabilities.

Designed for visualization needs in **scientific computing, engineering simulation, and data analysis**, EasyPlot strives for the optimal balance between **performance, extensibility, and ease of use**.

---

## ✨ Core Features

- **Hybrid 2D/3D Rendering Pipeline**  
  Independent scene graph branches and dedicated rendering passes enable seamless overlay of 2D HUDs/charts onto 3D scenes.
- **Custom Runtime Meta-Object System**  
  Built with C++ template metaprogramming and Boost.Describe, it supports class name registration, inheritance chain tracking, factory creation, and enum reflection, powering serialization and automatic UI binding.
- **Dynamic Property System & Two-Way Data Binding**  
  Supports dynamic attachment of properties of any type. Property changes automatically trigger signals to refresh the UI, and conversely, UI modifications are automatically written back to the object.
- **Modern OpenGL Low-Level Encapsulation**  
  Comprehensive encapsulation of OpenGL 4.6 core features: textures (including MSAA), buffers (UBO/SSBO/PBO), FBOs, VAOs, context management, state save/restore, and sync objects.
- **High-Quality SDF Text Rendering**  
  Generates Signed Distance Field (SDF) atlases using FreeType, achieving crisp, alias-free text rendering at any scale.
- **2D Charting System**  
  Interactive chart nodes supporting zoom and pan. Uses a geometry shader to render anti-aliased curves with a fixed pixel width and supports various line styles.
- **Multi-Mode Interaction Handlers**  
  Integrates various interaction modes like camera navigation, object picking, and 3D manipulators (translate/rotate/scale), with support for mode combinations and hotkey switching.
- **Property Inspector (MVD Pattern)**  
  A property panel built on the Qt Model/View/Delegate pattern, supporting custom editors for colors, enums, and booleans, and is dynamically extensible.
- **Python Scripting Support**  
  Embeds a Python interpreter via pybind11, providing an interactive console and script execution capabilities for secondary development.

---

## 🏗️ Architecture Overview

EasyPlot follows a **strict layered and interface-separation** design, divided into 5 layers from top to bottom:

| Layer | Modules | Responsibilities |
| :--- | :--- | :--- |
| **Application Layer** | `main`, `XMainWindow`, `easyPlot` | Qt UI integration, menus/toolbars, main window layout, rendering view encapsulation. |
| **Rendering Engine Layer** | `render`, `xopenglwidget`, `xbaserender` | Multi-viewport rendering pipeline, MSAA + MRT, SMAA post-processing, interaction handlers, scene graph traversal. |
| **Scene Graph Layer** | `xrendernode`, `lib02_camera` | Scene graph nodes (geometry/text/grid/manipulators/charts), camera models, property/state management, data sources & mappers. |
| **Graphics API Layer** | `lib04_opengl`, `lib01_shader`, `lib03_stbImage` | OpenGL object encapsulation, shader compilation & uniform management, texture loading. |
| **Infrastructure Layer** | `dataBase`, `lib00_utilty`, `xsignal`, `xlog`, `xcodecvt`, `xpython`, `xmvd` | Object model, property system, signals/slots, logging, encoding conversion, Python integration, property inspector. |

Layers are decoupled through **abstract interfaces** and **signals/slots**, ensuring the system's maintainability and extensibility.

---

## 🛠️ Technology Stack

| Domain | Technology |
| :--- | :--- |
| **Language Standard** | C++17 |
| **GUI Framework** | Qt 5.15 (Widgets, Core, Concurrent) |
| **Graphics API** | OpenGL 4.6 Core Profile |
| **Extension Loading** | GLEW + WGL |
| **Linear Algebra** | Eigen 3.4 |
| **Font Engine** | FreeType 2.10+ (SDF support) |
| **Signals/Slots** | Boost.Signals2 (custom encapsulation) |
| **Logging System** | spdlog (custom forwarding Sink) |
| **Serialization** | HDF5 (HighFive encapsulation) |
| **Python Binding** | pybind11 2.10+ |
| **Image Loading** | stb_image + SOIL2 |
| **Build Tool** | CMake 3.8+ |

---

## 🚀 Quick Start

### Prerequisites

- Windows 10/11 (Currently the main supported platform, Linux support is planned for the future)
- Visual Studio 2022 (MSVC 143) or MinGW-w64
- CMake 3.8+
- Qt 5.15.2
- Python 3.8 (Optional, for scripting support)

### Build Steps

```bash
git clone https://gitee.com/LonelyPolarbear/EasyPlot.git
cd EasyPlot
cmake -B build -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 17 2022"
cmake --build build --config Release