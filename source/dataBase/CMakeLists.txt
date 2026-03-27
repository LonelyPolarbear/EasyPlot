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
)

list(APPEND LIB_DIRECTORIES ${CMAKE_SOURCE_DIR}/output/bin)
														
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

#[[
install(TARGETS ${proName}
    EXPORT dataBaseTargets                     # 导出目标集，供后续使用
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}  # 动态库安装到 lib/ 或 lib64/
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}  # 静态库（如果有）也放这里
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}  # Windows 上 .dll 会放到 bin/
)
]]