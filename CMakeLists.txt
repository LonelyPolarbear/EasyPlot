cmake_minimum_required(VERSION 3.8)
project(QtOpenGL)		#放在根目录下可以理解为解决方案的名称


add_subdirectory(source/lib00_utilty)
add_subdirectory(source/lib01_shader)
add_subdirectory(source/lib02_camera)
add_subdirectory(source/lib03_stbImage)
add_subdirectory(source/lib04_opengl)
add_subdirectory(source/render)
add_subdirectory(source/dataBase)
add_subdirectory(source/lib05_shape)
add_subdirectory(source/lib06_select)
add_subdirectory(source/lib07_scene)
add_subdirectory(source/lib08_freetype)
add_subdirectory(source/lib09_panel)

add_subdirectory(source/XOpenGLWidget)

add_subdirectory(source/main)

add_subdirectory(source/easyPlot)
add_subdirectory(source/xsignal)

add_dependencies(lib01_shader  dataBase)
add_dependencies(lib00_utilty  dataBase)
add_dependencies(lib02_camera  lib00_utilty)
add_dependencies(lib04_opengl  lib00_utilty lib01_shader lib02_camera lib03_stbImage dataBase)
add_dependencies(render  lib00_utilty lib01_shader lib02_camera lib03_stbImage dataBase lib04_opengl lib05_shape)
add_dependencies(lib05_shape  lib04_opengl)
add_dependencies(lib06_select  lib04_opengl lib02_camera lib01_shader lib00_utilty lib05_shape)
add_dependencies(lib07_scene  lib04_opengl lib02_camera lib01_shader lib00_utilty lib06_select lib05_shape)
add_dependencies(lib08_freetype   lib00_utilty dataBase)
add_dependencies(XOpenGLWidget  lib00_utilty lib01_shader lib02_camera lib04_opengl render)

add_dependencies(easyPlot  XOpenGLWidget lib04_opengl lib05_shape lib06_select lib07_scene lib08_freetype lib09_panel render)
add_dependencies(lib09_panel  lib00_utilty)
add_dependencies(main easyPlot lib09_panel)