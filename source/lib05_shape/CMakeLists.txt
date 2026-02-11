project(lib05_shape)
set(proName lib05_shape)
set(outPutName "xshape")
include(${CMAKE_SOURCE_DIR}/config_cmake/public.cmake)
include(${CMAKE_SOURCE_DIR}/config_cmake/function.cmake)
include(${CMAKE_SOURCE_DIR}/config_cmake/var.cmake)	
include(${CMAKE_SOURCE_DIR}/config_cmake/qt.cmake)

#递归目录获取所有c文件
file(GLOB_RECURSE HEADERS_FILES *.h *.hpp)
file(GLOB_RECURSE SOURCES_FILES *.cpp)

# 调用函数进行分组
list(APPEND ALL_SOURCES 
	${HEADERS_FILES} 
	${SOURCES_FILES}
) 
group_source_files_by_directory(${ALL_SOURCES})

find_package(Qt5 REQUIRED COMPONENTS Core )
#-------------------------------------------------------
#设置依赖的动态库头文件目录
#-------------------------------------------------------
LIST(APPEND LIB_INCLUDE_DIRECTORY 	
	${CMAKE_SOURCE_DIR}/source 
	${CMAKE_SOURCE_DIR}/3rdParty/glew/include
	${CMAKE_SOURCE_DIR}/3rdParty/stb_image/		
	${CMAKE_SOURCE_DIR}/3rdParty/Eigen3
)

#-------------------------------------------------------
#设置依赖的动态库名称
#-------------------------------------------------------
LIST(APPEND LIB_NAMES 	
	Qt5::Core
	glew32
	opengl32
	XUtilty${DLL_DEBUG_POSTFIX}
	xshader${DLL_DEBUG_POSTFIX}
	stbImage${DLL_DEBUG_POSTFIX}
	dataBase${DLL_DEBUG_POSTFIX}
	xopengl${DLL_DEBUG_POSTFIX}
	xcamera${DLL_DEBUG_POSTFIX}
	xfreetype${DLL_DEBUG_POSTFIX}
	xalgo${DLL_DEBUG_POSTFIX}
)

LIST(APPEND LIB_DIRECTORIES 
${CMAKE_SOURCE_DIR}/output/bin 
${CMAKE_SOURCE_DIR}/3rdParty/glew/lib
)
														
#生成动态库
add_library(${proName}  SHARED)
target_sources(${proName} PRIVATE ${SOURCES_FILES} ${HEADERS_FILES})
#将动态库名字设为add
target_compile_definitions(${proName} PRIVATE LIB05_SHAPE_DLL)
set_target_properties(${proName} PROPERTIES OUTPUT_NAME ${outPutName})
set_property(GLOBAL APPEND PROPERTY MY_PROJECT_LIB_TARGETS ${proName})
set_property(GLOBAL APPEND PROPERTY MY_PROJECT_LIBS ${outPutName})
set_target_properties(${proName} PROPERTIES VERSION 1.0 SOVERSION 1)

#target_compile_definitions(${proName} PRIVATE STB_IMAGE_IMPLEMENTATION)				#该宏会使得编译器编译stb_image.h文件
target_link_directories(${proName} PRIVATE ${LIB_DIRECTORIES})									#库目录
target_include_directories(${proName} PRIVATE ${LIB_INCLUDE_DIRECTORY})				#包含目录
target_link_libraries(${proName} ${LIB_NAMES})		