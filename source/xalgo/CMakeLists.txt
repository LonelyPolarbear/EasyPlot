project(xalgo)
set(proName xalgo)
include(${CMAKE_SOURCE_DIR}/config_cmake/public.cmake)
#include(${CMAKE_SOURCE_DIR}/config_cmake/var.cmake)

#递归目录获取所有c文件
file(GLOB_RECURSE HERDERS_FILES  *.h *.hpp)
file(GLOB_RECURSE SOURECS_FILES  *.cpp *.c)

#-------------------------------------------------------
#设置依赖的动态库头文件目录
#-------------------------------------------------------
SET(LIB_INCLUDE_DIRECTORY 	
													${CMAKE_SOURCE_DIR}/source 
)

#-------------------------------------------------------
#设置依赖的动态库名称
#-------------------------------------------------------
SET(LIB_NAMES 	
	PRIVATE MyLib::HeaderOnly
	dataBase${DLL_DEBUG_POSTFIX}
	
)

set(LIB_DIRECTORIES 
	#${CMAKE_SOURCE_DIR}/3rdParty/glew/lib
)
														
#生成动态库
add_library(${proName}  SHARED)
target_sources(${proName} PRIVATE  ${SOURECS_FILES} ${HERDERS_FILES})						#设置源文件和头文件
#将动态库名字设为add
target_compile_definitions(${proName} PRIVATE XALGO_DLL)														#预定义宏
set_target_properties(${proName} PROPERTIES OUTPUT_NAME "xalgo")									#设置库的输出名称
set_target_properties(${proName} PROPERTIES VERSION 1.0 SOVERSION 1)								#设置版本号
set_property(GLOBAL APPEND PROPERTY MY_PROJECT_LIBS "xalgo")												#添加到全局属性

target_link_directories(${proName} PRIVATE ${LIB_DIRECTORIES})											#库目录
target_include_directories(${proName} PRIVATE ${LIB_INCLUDE_DIRECTORY})						#包含目录
target_link_libraries(${proName} ${LIB_NAMES})																			#依赖的lib目标名称