project(dataBase)
set(proName dataBase)
set(outPutName "dataBase")
include(${CMAKE_SOURCE_DIR}/config_cmake/public.cmake)
include(${CMAKE_SOURCE_DIR}/config_cmake/function.cmake)
include(${CMAKE_SOURCE_DIR}/config_cmake/var.cmake)			


#递归目录获取所有c文件
file(GLOB_RECURSE HERDERS_FILES  *.h *.hpp)
file(GLOB_RECURSE SOURCES_FILES  *.cpp *.c)

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
	${CMAKE_SOURCE_DIR}/3rdParty/boost/include
)

#-------------------------------------------------------
#设置依赖的动态库名称
#-------------------------------------------------------
list(APPEND LIB_NAMES 
	xlog${DLL_DEBUG_POSTFIX}
	MyLib::HDF5
	MyLib::pybind
)

list(APPEND LIB_DIRECTORIES ${BIN_DIR})
														
#生成动态库
add_library(${proName}  SHARED)
generate_dllmain_if_needed(${proName})
generate_dllapi_if_needed(${proName} )
get_dll_api_macro(${proName} DLL_API_NAME)

target_sources(${proName} PRIVATE  ${SOURCES_FILES} ${HERDERS_FILES})

#设置
target_compile_definitions(${proName} PRIVATE ${DLL_API_NAME})
set_target_properties(${proName} PROPERTIES 
OUTPUT_NAME ${outPutName}
)

set_property(GLOBAL APPEND PROPERTY MY_PROJECT_LIB_TARGETS ${proName})
set_property(GLOBAL APPEND PROPERTY MY_PROJECT_LIBS ${outPutName})
set_target_properties(${proName} PROPERTIES VERSION 1.0 SOVERSION 1) 

target_link_directories(${proName} PRIVATE ${LIB_DIRECTORIES})												#库目录
target_include_directories(${proName} PRIVATE ${LIB_INCLUDE_DIRECTORY})							#包含目录
target_link_libraries(${proName}  PRIVATE ${LIB_NAMES})																#库lib名

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