project(xlog)
set(proName xlog)
set(outPutName "xlog")

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
	${CMAKE_SOURCE_DIR}/3rdParty/spdlog/include
)


list(APPEND LIB_NAMES 
					#XUtilty${DLL_DEBUG_POSTFIX}
)


list(APPEND LIB_DIRECTORIES 
	${CMAKE_SOURCE_DIR}/output/bin
)

	
#生成动态库
add_library(${proName}  SHARED)
generate_dllapi_if_needed(${proName} )
generate_dllmain_if_needed(${proName})
get_dll_api_macro(${proName} DLL_API_NAME)

#target_compile_options(${proName} PRIVATE /utf-8)
#target_compile_definitions(${proName} PRIVATE SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)
target_sources(${proName} PRIVATE ${SOURCES_FILES} ${HEADERS_FILES})
#将动态库名字设为add
target_compile_definitions(${proName} PRIVATE ${DLL_API_NAME})
set_target_properties(${proName} PROPERTIES OUTPUT_NAME ${outPutName})
set_property(GLOBAL APPEND PROPERTY MY_PROJECT_LIB_TARGETS ${proName})
set_property(GLOBAL APPEND PROPERTY MY_PROJECT_LIBS ${outPutName})
set_target_properties(${proName} PROPERTIES VERSION 1.0 SOVERSION 1)

target_link_directories(${proName} PRIVATE ${LIB_DIRECTORIES})											#库目录
target_include_directories(${proName} PRIVATE ${LIB_INCLUDE_DIRECTORY})							#包含目录
target_link_libraries(${proName} PRIVATE ${LIB_NAMES})		