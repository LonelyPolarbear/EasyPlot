project(xbaserender)
set(proName xbaserender)
set(outPutName "xbaserender")

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
	${CMAKE_SOURCE_DIR}/3rdParty/glew/include
	${CMAKE_SOURCE_DIR}/3rdParty/stb_image/
	${CMAKE_SOURCE_DIR}/3rdParty/boost/include
	${CMAKE_SOURCE_DIR}/3rdParty/Eigen3		
	${CMAKE_SOURCE_DIR}/Python38/include
)

#-------------------------------------------------------
#设置依赖的动态库名称
#-------------------------------------------------------
LIST(APPEND LIB_NAMES 	
	MyLib::pybind
	glew32
	opengl32
	xutilty${DLL_DEBUG_POSTFIX}
	xshader${DLL_DEBUG_POSTFIX}
	dataBase${DLL_DEBUG_POSTFIX}
	xlog${DLL_DEBUG_POSTFIX}
	xopengl${DLL_DEBUG_POSTFIX}
)


LIST(APPEND LIB_DIRECTORIES 
${BIN_DIR}
${CMAKE_SOURCE_DIR}/3rdParty/glew/lib
)
														
#生成动态库
add_library(${proName}  SHARED)
generate_dllmain_if_needed(${proName})
generate_dllapi_if_needed(${proName} )
get_dll_api_macro(${proName} DLL_API_NAME)
generate_Init_if_needed(${proName})

target_sources(${proName} PRIVATE ${SOURCES_FILES} ${HEADERS_FILES})
#动态库名字
target_compile_definitions(${proName} PRIVATE ${DLL_API_NAME})
set_target_properties(${proName} PROPERTIES OUTPUT_NAME ${outPutName})

set_property(GLOBAL APPEND PROPERTY MY_PROJECT_LIB_TARGETS ${proName})
set_property(GLOBAL APPEND PROPERTY MY_PROJECT_LIBS ${outPutName})
set_target_properties(${proName} PROPERTIES VERSION 1.0 SOVERSION 1)

#target_compile_definitions(${proName} PRIVATE STB_IMAGE_IMPLEMENTATION)				#该宏会使得编译器编译stb_image.h文件
target_link_directories(${proName} PRIVATE ${LIB_DIRECTORIES})									#库目录
target_include_directories(${proName} PRIVATE ${LIB_INCLUDE_DIRECTORY})				#包含目录
target_link_libraries(${proName} PRIVATE ${LIB_NAMES})		

#INSTALL 配置
#[1] 安装目标
install(TARGETS ${proName}
    #EXPORT EasyPlotTargets                    # 导出目标（需与顶层配合）
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}   # Windows DLL 或可执行文件 → bin/
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}   # Linux/macOS 共享库 → lib/ 或 lib64/
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}   # 静态库或导入库 → lib/
)
#[2] 安装头文件
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
	DESTINATION ${PROJECT_INSTALL_INCLUDEDIR}/${proName}
	FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
	PATTERN "CMakeLists.txt" EXCLUDE
    PATTERN "*.cpp" EXCLUDE
)