project(render)
set(proName render)
include(${CMAKE_SOURCE_DIR}/config_cmake/public.cmake)

#递归目录获取所有c文件
file(GLOB_RECURSE HERDERS_FILES *.h)
file(GLOB_RECURSE SOURECS_FILES *.cpp)

if(CMAKE_BUILD_TYPE  MATCHES "Debug")
SET(DLL_DEBUG_POSTFIX "_d")  
else() 
SET(DLL_DEBUG_POSTFIX)
endif()

#-------------------------------------------------------
#设置依赖的动态库头文件目录
#-------------------------------------------------------
SET(LIB_INCLUDE_DIRECTORY 	${CMAKE_SOURCE_DIR}/source 
														${CMAKE_SOURCE_DIR}/3rdParty/glew/include
														${CMAKE_SOURCE_DIR}/3rdParty/stb_image/
														${CMAKE_SOURCE_DIR}/3rdParty/boost/include
														${CMAKE_SOURCE_DIR}/3rdParty/Eigen3
														
)

#-------------------------------------------------------
#设置依赖的动态库名称
#-------------------------------------------------------
SET(LIB_NAMES 	
	glew32
	opengl32
	myUtilty${DLL_DEBUG_POSTFIX}
	xshader${DLL_DEBUG_POSTFIX}
	stbImage${DLL_DEBUG_POSTFIX}
	dataBase${DLL_DEBUG_POSTFIX}
	xopengl${DLL_DEBUG_POSTFIX}
	xcamera${DLL_DEBUG_POSTFIX}
	xshape${DLL_DEBUG_POSTFIX}
)

set(LIB_DIRECTORIES 
${CMAKE_SOURCE_DIR}/output/bin 
${CMAKE_SOURCE_DIR}/3rdParty/glew/lib
)
														
#生成动态库
add_library(${proName}  SHARED)
target_sources(${proName} PUBLIC ${SOURECS_FILES} ${HERDERS_FILES})
#将动态库名字设为add
target_compile_definitions(${proName} PRIVATE LIB_XRENDER_DLL)
set_target_properties(${proName} PROPERTIES OUTPUT_NAME "xrender")
set_target_properties(${proName} PROPERTIES VERSION 1.0 SOVERSION 1)

#target_compile_definitions(${proName} PRIVATE STB_IMAGE_IMPLEMENTATION)				#该宏会使得编译器编译stb_image.h文件
target_link_directories(${proName} PRIVATE ${LIB_DIRECTORIES})									#库目录
target_include_directories(${proName} PRIVATE ${LIB_INCLUDE_DIRECTORY})				#包含目录
target_link_libraries(${proName} ${LIB_NAMES})		