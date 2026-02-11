project(lib02_camera)
set(proName lib02_camera)
set(outPutName "xcamera")
include(${CMAKE_SOURCE_DIR}/config_cmake/public.cmake)
include(${CMAKE_SOURCE_DIR}/config_cmake/function.cmake)
include(${CMAKE_SOURCE_DIR}/config_cmake/var.cmake)			

#递归目录获取所有c文件
file(GLOB_RECURSE HERDERS_FILES *.h)
file(GLOB_RECURSE SOURCES_FILES *.cpp)

#-------------------------------------------------------
#设置依赖的动态库头文件目录
#-------------------------------------------------------
list(APPEND LIB_INCLUDE_DIRECTORY 	${CMAKE_SOURCE_DIR}/source 
														${CMAKE_SOURCE_DIR}/3rdParty/Eigen3
)

#-------------------------------------------------------
#设置依赖的动态库名称
#-------------------------------------------------------
lIST(APPEND LIB_NAMES 	
	XUtilty${DLL_DEBUG_POSTFIX}
	dataBase${DLL_DEBUG_POSTFIX}
)

LIST(APPEND LIB_DIRECTORIES 
${CMAKE_SOURCE_DIR}/output/bin 
)
														
#生成动态库
add_library(${proName}  SHARED)
target_sources(${proName} PUBLIC ${SOURCES_FILES} ${HERDERS_FILES})
#将动态库名字设为add
target_compile_definitions(${proName} PRIVATE LIB02_CAMERA_DLL)
set_target_properties(${proName} PROPERTIES OUTPUT_NAME  ${outPutName})
set_property(GLOBAL APPEND PROPERTY MY_PROJECT_LIB_TARGETS ${proName})
set_property(GLOBAL APPEND PROPERTY MY_PROJECT_LIBS  ${outPutName})
set_target_properties(${proName} PROPERTIES VERSION 1.0 SOVERSION 1)

target_link_directories(${proName} PRIVATE ${LIB_DIRECTORIES})											#库目录
target_include_directories(${proName} PRIVATE ${LIB_INCLUDE_DIRECTORY})							#包含目录
target_link_libraries(${proName} ${LIB_NAMES})		