project(xsignal)
set(proName xsignal)
include(${CMAKE_SOURCE_DIR}/config_cmake/public.cmake)

#递归目录获取所有c文件
#file(GLOB_RECURSE SOURCES  *.cpp *.c *.h *.hpp)
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
														#${CMAKE_SOURCE_DIR}/3rdParty/boost/include
														
)

#-------------------------------------------------------
#设置依赖的动态库名称
#-------------------------------------------------------
SET(LIB_NAMES 	
	#dataBase${DLL_DEBUG_POSTFIX}
	PRIVATE MyLib::HeaderOnly
)

set(LIB_DIRECTORIES 
${CMAKE_SOURCE_DIR}/output/bin 
)
														
#生成动态库
add_library(${proName}  SHARED)
target_sources(${proName} PUBLIC ${SOURECS_FILES} ${HERDERS_FILES})
#将动态库名字设为add
target_compile_definitions(${proName} PRIVATE LIB_XSIGNAL_DLL)
set_target_properties(${proName} PROPERTIES OUTPUT_NAME "xsignal")
set_target_properties(${proName} PROPERTIES VERSION 1.0 SOVERSION 1)

target_link_directories(${proName} PRIVATE ${LIB_DIRECTORIES})											#库目录
target_include_directories(${proName} PRIVATE ${LIB_INCLUDE_DIRECTORY})							#包含目录
target_link_libraries(${proName} ${LIB_NAMES})		