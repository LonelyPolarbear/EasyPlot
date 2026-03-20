cmake_minimum_required(VERSION 3.8)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)           # 生成编译命令数据库,需要在project之前设置,MSVC下不生效
project(QtOpenGL)		                                                        #放在根目录下可以理解为解决方案的名称

add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)
if(MSVC)
    add_compile_options(/utf-8)
    add_compile_options(/Zc:__cplusplus)
endif()
#--------------------------------------------------------------------------------------------------------------
# 创建一个 INTERFACE 库（只传递属性）可选：加个命名空间别名，更规范
#--------------------------------------------------------------------------------------------------------------
set(MYLIB_INCLUDE_DIR 
                ${CMAKE_SOURCE_DIR}/3rdParty/boost/include
                ${CMAKE_SOURCE_DIR}/3rdParty/Eigen3
                ${CMAKE_SOURCE_DIR}/3rdParty/MagicEnum/include
                ${CMAKE_SOURCE_DIR}/3rdParty/spdlog/include
)
add_library(mylib_header_only INTERFACE)
target_include_directories(mylib_header_only INTERFACE ${MYLIB_INCLUDE_DIR})
add_library(MyLib::HeaderOnly ALIAS mylib_header_only)


#--------------------------------------------------------------------------------------------------------------
# 创建一个hdf5的 INTERFACE 库
#--------------------------------------------------------------------------------------------------------------
set(hdf5_DIR ${CMAKE_SOURCE_DIR}/3rdParty/hdf5/cmake)
# 查找 HDF5（使用 CONFIG 模式）
find_package(hdf5 REQUIRED CONFIG COMPONENTS C CXX HL HL_CXX)
#get_property(hdf5_targets DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY IMPORTED_TARGETS)
#message(STATUS "HDF5 imported targets: ${hdf5_targets}")
add_library(MyHDF5Deps INTERFACE)
target_link_libraries(MyHDF5Deps INTERFACE
    hdf5_hl_cpp-shared   # 高级 C++ 库
    hdf5_cpp-shared       # C++ 库
    hdf5_hl-shared        # 高级 C 库
    hdf5-shared           # 核心 C 库
)

# 添加 HighFive 头文件路径（如果 HighFive 是 header-only 且未使用 CMake 目标）
target_include_directories(MyHDF5Deps INTERFACE
    ${CMAKE_SOURCE_DIR}/3rdParty/highFive/include
)

add_library(MyLib::HDF5 ALIAS MyHDF5Deps)

#--------------------------------------------------------------------------------------------------------------
# 所有的测试示例都要依赖
#--------------------------------------------------------------------------------------------------------------
add_library(MyExampleDeps INTERFACE)
target_link_libraries(MyExampleDeps INTERFACE
   linenoise_lib${DLL_DEBUG_POSTFIX}
)

# 添加 HighFive 头文件路径（如果 HighFive 是 header-only 且未使用 CMake 目标）
target_include_directories(MyExampleDeps INTERFACE
                                            ${CMAKE_SOURCE_DIR}/3rdParty/CLI/include
											${CMAKE_SOURCE_DIR}/3rdParty/linenoise/include
)

add_library(MyLib::example ALIAS MyExampleDeps)

#--------------------------------------------------------------------------------------------------------------
# 使用全局属性收集所有lib以便示例项目正确依赖
#--------------------------------------------------------------------------------------------------------------
set_property(GLOBAL PROPERTY MY_PROJECT_LIB_TARGETS "")
set_property(GLOBAL PROPERTY MY_PROJECT_LIBS "")

#--------------------------------------------------------------------------------------------------------------
# 以下是各个库的CMakeLists.txt文件，按照顺序依次添加
#--------------------------------------------------------------------------------------------------------------
add_subdirectory(source/lib00_utilty)
add_subdirectory(source/lib01_shader)
add_subdirectory(source/lib02_camera)
add_subdirectory(source/lib03_stbImage)
add_subdirectory(source/lib04_opengl)
add_subdirectory(source/render)
add_subdirectory(source/dataBase)
add_subdirectory(source/xalgo)
add_subdirectory(source/lib05_shape)
add_subdirectory(source/lib06_select)
add_subdirectory(source/lib07_scene)
add_subdirectory(source/lib08_freetype)
add_subdirectory(source/lib09_panel)
add_subdirectory(source/XOpenGLWidget)
add_subdirectory(source/main)
add_subdirectory(source/easyPlot)
add_subdirectory(source/xsignal)
add_subdirectory(source/xstorage)
add_subdirectory(source/xmvd)
add_subdirectory(source/xtest)
add_subdirectory(source/xlog)

#--------------------------------------------------------------------------------------------------------------
# 依赖关系
#--------------------------------------------------------------------------------------------------------------
add_dependencies(dataBase  xstorage xlog)
add_dependencies(xalgo  dataBase xlog)
add_dependencies(lib01_shader  dataBase xlog)
add_dependencies(lib00_utilty  dataBase xlog)
add_dependencies(lib02_camera  lib00_utilty xlog)
add_dependencies(lib04_opengl  lib00_utilty lib01_shader lib02_camera lib03_stbImage dataBase xlog)
add_dependencies(render  lib00_utilty lib01_shader lib02_camera lib03_stbImage dataBase lib04_opengl lib05_shape xlog)
add_dependencies(lib05_shape  lib04_opengl xalgo xlog)
add_dependencies(lib06_select  lib04_opengl lib02_camera lib01_shader lib00_utilty lib05_shape xlog)
add_dependencies(lib07_scene  lib04_opengl lib02_camera lib01_shader lib00_utilty lib06_select lib05_shape xlog)
add_dependencies(lib08_freetype   lib00_utilty dataBase xlog)
add_dependencies(XOpenGLWidget  lib00_utilty lib01_shader lib02_camera lib04_opengl render xlog)

add_dependencies(easyPlot  XOpenGLWidget lib04_opengl lib05_shape lib06_select lib07_scene lib08_freetype lib09_panel render xlog)
add_dependencies(lib09_panel  lib00_utilty xlog)
add_dependencies(xmvd  dataBase lib00_utilty xlog)
add_dependencies(main easyPlot lib09_panel xlog)


#--------------------------------------------------------------------------------------------------------------
# examples
#--------------------------------------------------------------------------------------------------------------
set_property(GLOBAL PROPERTY MY_EXAMPLE_TARGETS "")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
option(BUILD_EXAMPLES "Build the test suite" ON)		#用于定义可以被用户配置的布尔选项
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()