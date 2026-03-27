project(lib01_shader)
set(proName lib01_shader)
set(outPutName "xshader")

include(${CMAKE_SOURCE_DIR}/config_cmake/public.cmake)
include(${CMAKE_SOURCE_DIR}/config_cmake/function.cmake)
include(${CMAKE_SOURCE_DIR}/config_cmake/var.cmake)		


file(GLOB_RECURSE HEADERS_FILES *.h *.hpp)
file(GLOB_RECURSE SOURCES_FILES *.cpp)

# 调用函数进行分组
list(APPEND ALL_SOURCES 
	${HEADERS_FILES} 
	${SOURCES_FILES}
) 
group_source_files_by_directory(${ALL_SOURCES})

#-------------------------------------------------------
#设置依赖的动态库头文件目录
#-------------------------------------------------------
list(APPEND LIB_INCLUDE_DIRECTORY 
	${CMAKE_SOURCE_DIR}/source 	
	${CMAKE_SOURCE_DIR}/3rdParty/Eigen3	
	${CMAKE_SOURCE_DIR}/3rdParty/glew/include
)

#-------------------------------------------------------
#设置依赖的动态库名称
#-------------------------------------------------------
list(APPEND LIB_NAMES 	
					glew32
					opengl32
					dataBase${DLL_DEBUG_POSTFIX}
					XUtilty${DLL_DEBUG_POSTFIX}
)

list(APPEND LIB_DIRECTORIES 
	${CMAKE_SOURCE_DIR}/output/bin
	${CMAKE_SOURCE_DIR}/3rdParty/glew/lib
)


						
#生成动态库
add_library(${proName}  SHARED)
target_sources(${proName} PRIVATE ${SOURCES_FILES} ${HEADERS_FILES})
#将动态库名字设为add
target_compile_definitions(${proName} PRIVATE LIB01_SHADER_DLL)
set_target_properties(${proName} PROPERTIES OUTPUT_NAME ${outPutName})
set_property(GLOBAL APPEND PROPERTY MY_PROJECT_LIB_TARGETS ${proName})
set_property(GLOBAL APPEND PROPERTY MY_PROJECT_LIBS ${outPutName})
set_target_properties(${proName} PROPERTIES VERSION 1.0 SOVERSION 1)

target_link_directories(${proName} PRIVATE ${LIB_DIRECTORIES})											#库目录
target_include_directories(${proName} PRIVATE ${LIB_INCLUDE_DIRECTORY})							#包含目录
target_link_libraries(${proName} ${LIB_NAMES})		