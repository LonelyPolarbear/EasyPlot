project(lib00_utilty)
set(proName lib00_utilty)
include(${CMAKE_SOURCE_DIR}/config_cmake/public.cmake)
include(${CMAKE_SOURCE_DIR}/config_cmake/function.cmake)
#include(${CMAKE_SOURCE_DIR}/config_cmake/var.cmake)										#所有预定义的变量

file(GLOB_RECURSE HEADERS_FILES *.h *.hpp)
file(GLOB_RECURSE SOURECS_FILES *.cpp)

#[[ //3.0引入
file(GLOB_RECURSE ALL_SOURCES 
    "*.cpp" "*.cxx" "*.cc" 
    "*.h" "*.hpp" "*.hxx" "*.inl"
)
]]
# 调用函数进行分组
set(ALL_SOURCES ${HEADERS_FILES} ${SOURECS_FILES}) 
group_source_files_by_directory(${ALL_SOURCES})

#-------------------------------------------------------
#设置依赖的动态库头文件目录
#-------------------------------------------------------
SET(LIB_INCLUDE_DIRECTORY 	${CMAKE_SOURCE_DIR}/source 							
														${CMAKE_SOURCE_DIR}/3rdParty/Eigen3
)

#-------------------------------------------------------
#设置依赖的动态库名称
#-------------------------------------------------------
SET(LIB_NAMES 	
	#dataBase${DLL_DEBUG_POSTFIX}
	MyLib::HeaderOnly
)

set(LIB_DIRECTORIES 
	#${CMAKE_SOURCE_DIR}/3rdParty/glew/lib
)
														
#生成动态库
add_library(${proName}  SHARED)
target_sources(${proName} PUBLIC ${SOURECS_FILES} ${HEADERS_FILES})
#将动态库名字设为add
target_compile_definitions(${proName} PRIVATE LIB00_UTILTY_DLL)
set_target_properties(${proName} PROPERTIES OUTPUT_NAME "XUtilty")
set_target_properties(${proName} PROPERTIES VERSION 1.0 SOVERSION 1)
set_property(GLOBAL APPEND PROPERTY MY_PROJECT_LIB_TARGETS ${proName})
set_property(GLOBAL APPEND PROPERTY MY_PROJECT_LIBS "XUtilty")

target_link_directories(${proName} PRIVATE ${LIB_DIRECTORIES})											#库目录
target_include_directories(${proName} PRIVATE ${LIB_INCLUDE_DIRECTORY})							#包含目录
target_link_libraries(${proName}  PRIVATE ${LIB_NAMES})