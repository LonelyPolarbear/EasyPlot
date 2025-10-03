cmake_minimum_required(VERSION 3.8)
project(QtOpenGL)		#放在根目录下可以理解为解决方案的名称


add_subdirectory(source/lib00_utilty)
add_subdirectory(source/lib01_shader)
add_subdirectory(source/lib02_camera)
add_subdirectory(source/lib03_stbImage)
add_subdirectory(source/lib04_opengl)
add_subdirectory(source/dataBase)
add_subdirectory(source/lib05_shape)
add_subdirectory(source/lib06_select)
add_subdirectory(source/lib07_scene)
add_subdirectory(source/lib08_freetype)

add_subdirectory(source/libPanle01_XOpenGLWidget)
#add_subdirectory(source/libPanle02_OpenGLWidget1)
#add_subdirectory(source/libPanle03_OpenGLWidget2)
#add_subdirectory(source/libPanle04_OpenGLWidgetCube)
#add_subdirectory(source/libPanle05_multiPolygonMode)

add_subdirectory(source/main)

add_subdirectory(source/easyPlot)

add_dependencies(lib01_shader  dataBase)
add_dependencies(lib04_opengl  lib00_utilty lib01_shader lib02_camera lib03_stbImage dataBase)
add_dependencies(lib05_shape  lib04_opengl)
add_dependencies(lib06_select  lib04_opengl lib02_camera lib01_shader lib00_utilty)
add_dependencies(lib07_scene  lib04_opengl lib02_camera lib01_shader lib00_utilty lib06_select)
add_dependencies(libPanle01_XOpenGLWidget  lib00_utilty lib01_shader lib02_camera lib04_opengl)
#add_dependencies(libPanle02_OpenGLWidget1  libPanle01_XOpenGLWidget)
#add_dependencies(libPanle03_OpenGLWidget2  libPanle01_XOpenGLWidget)
#add_dependencies(libPanle04_OpenGLWidgetCube  libPanle01_XOpenGLWidget)

#add_dependencies(libPanle05_multiPolygonMode  libPanle01_XOpenGLWidget)
add_dependencies(easyPlot  libPanle01_XOpenGLWidget lib04_opengl lib05_shape lib06_select lib07_scene)


#									libPanle02_OpenGLWidget1 
#									libPanle03_OpenGLWidget2
#									libPanle04_OpenGLWidgetCube
#									libPanle05_multiPolygonMode
add_dependencies(main easyPlot)