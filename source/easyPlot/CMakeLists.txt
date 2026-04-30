project(easyPlot)
set(proName easyPlot)

include(${CMAKE_SOURCE_DIR}/config_cmake/public.cmake)
include(${CMAKE_SOURCE_DIR}/config_cmake/qt.cmake)
include(${CMAKE_SOURCE_DIR}/config_cmake/function.cmake)

#-------------------------------------------------------
#设置包含的头文件
#-------------------------------------------------------
#include_directories(${PROJECT_SOURCE_DIR}/source/libPanle01_XOpenGLWidget)

#递归目录获取所有源文件
file(GLOB_RECURSE HERDERS_FILES *.h)
file(GLOB_RECURSE SOURCES_FILES *.cpp)
#-------------------------------------------------------
#Qt支持
#-------------------------------------------------------
#查找所有的需要moc预处理的文件
set(MOC_FILES easyPlotWidget.h XEasyPlotWidget.h)

#GLOB_RECURSE 
FILE(GLOB UI_FILES "ui/*.ui")
FILE(GLOB QRC_FILES "qrc/*.qrc")
find_package(Qt5 REQUIRED COMPONENTS Core )
find_package(Qt5 REQUIRED COMPONENTS Widgets )
find_package(Qt5 REQUIRED COMPONENTS Concurrent )
qt5_wrap_cpp(MOC_SOURCES ${MOC_FILES})
qt5_wrap_ui(UI_SOURCES ${UI_FILES})	
qt5_add_resources(RCC_SOURCES ${QRC_FILES})

#-------------------------------------------------------
#分组
#-------------------------------------------------------
assign_source_group(${QRC_FILES} ${UI_FILES})
assign_source_group(${SOURCES_FILES} ${HERDERS_FILES} ${MOC_SOURCES} ${UI_SOURCES} ${RCC_SOURCES})

#设为应用程序

add_library(${proName}  SHARED)

target_sources(${proName} PRIVATE ${SOURCES_FILES} 
								${HERDERS_FILES}
								${MOC_SOURCES}
								${UI_SOURCES}
								${RCC_SOURCES}
			)

#设置exe名称						 
#set_target_properties(${proName} PROPERTIES OUTPUT_NAME "out")	
if(CMAKE_BUILD_TYPE  MATCHES "Debug")
SET(DLL_DEBUG_POSTFIX "")  
else() 
SET(DLL_DEBUG_POSTFIX)
endif()

#-------------------------------------------------------
#设置依赖的动态库头文件目录
#-------------------------------------------------------
SET(LIB_INCLUDE_DIRECTORY 	${CMAKE_SOURCE_DIR}/source 
							${CMAKE_SOURCE_DIR}/source
							${CMAKE_SOURCE_DIR}/3rdParty/glew/include
							${CMAKE_SOURCE_DIR}/3rdParty/Eigen3
	)

#-------------------------------------------------------
#设置依赖的动态库名称
#-------------------------------------------------------
SET(LIB_NAMES 	
				PRIVATE MyLib::HeaderOnly
				MyLib::HDF5
				Qt5::Widgets
				Qt5::Core
				Qt5::Concurrent
				glew32
				opengl32
				xutilty${DLL_DEBUG_POSTFIX}
				xshader${DLL_DEBUG_POSTFIX}
				xcamera${DLL_DEBUG_POSTFIX}
				xopenglwidget${DLL_DEBUG_POSTFIX}
				xopengl${DLL_DEBUG_POSTFIX}
				xrendernode${DLL_DEBUG_POSTFIX}
				dataBase${DLL_DEBUG_POSTFIX}
				xselect${DLL_DEBUG_POSTFIX}
				xscene${DLL_DEBUG_POSTFIX}
				xfreetype${DLL_DEBUG_POSTFIX}
				stbImage${DLL_DEBUG_POSTFIX}
				xpanel${DLL_DEBUG_POSTFIX}
				xrender${DLL_DEBUG_POSTFIX}
				xalgo${DLL_DEBUG_POSTFIX}
				xsharevar${DLL_DEBUG_POSTFIX}
	)

target_link_directories(${proName} PRIVATE ${CMAKE_SOURCE_DIR}/output/bin
										${CMAKE_SOURCE_DIR}/3rdParty/glew/lib
)		#库目录

target_include_directories(${proName} PRIVATE ${LIB_INCLUDE_DIRECTORY})			#包含目录

target_link_libraries(${proName} ${LIB_NAMES})									#库名称


target_compile_definitions(${proName} PRIVATE LIB_EASYPLOT_DLL)
set_target_properties(${proName} PROPERTIES OUTPUT_NAME "xeasyPlot")
set_target_properties(${proName} PROPERTIES VERSION 1.0 SOVERSION 1)

#cmake -B build -DCMAKE_BUILD_TYPE=Debug -G  "Visual Studio 17 2022"
#cmake -B build -DCMAKE_BUILD_TYPE=Release -G  "Visual Studio 17 2022"