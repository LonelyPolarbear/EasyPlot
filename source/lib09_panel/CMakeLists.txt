project(lib09_panel)
set(proName lib09_panel)
set(outPutName "xpanel")
include(${CMAKE_SOURCE_DIR}/config_cmake/public.cmake)
include(${CMAKE_SOURCE_DIR}/config_cmake/qt.cmake)
include(${CMAKE_SOURCE_DIR}/config_cmake/function.cmake)
include(${CMAKE_SOURCE_DIR}/config_cmake/var.cmake)		

#-------------------------------------------------------
#设置包含的头文件
#-------------------------------------------------------
include_directories(${PROJECT_SOURCE_DIR}/source/lib09_panel)

#递归目录获取所有源文件
file(GLOB_RECURSE HEADERS_FILES *.h *.hpp)
file(GLOB_RECURSE SOURCES_FILES *.cpp)

#-------------------------------------------------------
#Qt支持
#-------------------------------------------------------
#查找所有的需要moc预处理的文件
set(MOC_FILES FontSetDlg.h ColorPickButton.h CustomToolButton.h ScreenShotDlg.h FboTest.h GridSetDlg.h)

#GLOB_RECURSE 
FILE(GLOB UI_FILES "ui/*.ui")
FILE(GLOB QRC_FILES "qrc/*.qrc")
find_package(Qt5 REQUIRED COMPONENTS Core )
find_package(Qt5 REQUIRED COMPONENTS Widgets )
qt5_wrap_cpp(MOC_SOURCES ${MOC_FILES})
qt5_wrap_ui(UI_SOURCES ${UI_FILES})	
qt5_add_resources(RCC_SOURCES ${QRC_FILES})

#-------------------------------------------------------
#分组
#-------------------------------------------------------
assign_source_group(${QRC_FILES} ${UI_FILES})
assign_source_group(${SOURCES_FILES} ${HEADERS_FILES} ${MOC_SOURCES} ${UI_SOURCES} ${RCC_SOURCES})

#设为应用程序

add_library(${proName}  SHARED)

target_sources(${proName} PRIVATE ${SOURCES_FILES} 
								${HEADERS_FILES}
								${MOC_SOURCES}
								${UI_SOURCES}
								${RCC_SOURCES}
			)

#-------------------------------------------------------
#设置依赖的动态库头文件目录
#-------------------------------------------------------
LIST(APPEND LIB_INCLUDE_DIRECTORY 	
	${CMAKE_SOURCE_DIR}/source 
	${CMAKE_SOURCE_DIR}/source
	${CMAKE_SOURCE_DIR}/3rdParty/glew/include
	${CMAKE_SOURCE_DIR}/3rdParty/Eigen3
)

#-------------------------------------------------------
#设置依赖的动态库名称
#-------------------------------------------------------
LIST(APPEND LIB_NAMES 	
	Qt5::Widgets
	Qt5::Core
	glew32
	XUtilty${DLL_DEBUG_POSTFIX}
	dataBase${DLL_DEBUG_POSTFIX}
)

list(APPEND LIB_DIRECTORIES
	${CMAKE_SOURCE_DIR}/output/bin
	${CMAKE_SOURCE_DIR}/3rdParty/glew/lib
)

target_compile_definitions(${proName} PRIVATE LIBPANEL09_XPANEL)
set_target_properties(${proName} PROPERTIES OUTPUT_NAME ${outPutName})
set_property(GLOBAL APPEND PROPERTY MY_PROJECT_LIB_TARGETS ${proName})
set_property(GLOBAL APPEND PROPERTY MY_PROJECT_LIBS ${outPutName})
set_target_properties(${proName} PROPERTIES VERSION 1.0 SOVERSION 1)

target_link_directories(${proName} PRIVATE ${LIB_DIRECTORIES})								#库目录
target_include_directories(${proName} PRIVATE ${LIB_INCLUDE_DIRECTORY})			#包含目录
target_link_libraries(${proName} ${LIB_NAMES})																#库名称

#cmake -B build -DCMAKE_BUILD_TYPE=Debug -G  "Visual Studio 17 2022"
#cmake -B build -DCMAKE_BUILD_TYPE=Release -G  "Visual Studio 17 2022"