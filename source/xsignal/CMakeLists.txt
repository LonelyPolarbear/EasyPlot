project(xsignal)
set(proName xsignal)
include(${CMAKE_SOURCE_DIR}/config_cmake/public.cmake)

#递归目录获取所有c文件
#file(GLOB_RECURSE SOURCES  *.cpp *.c *.h *.hpp)
file(GLOB_RECURSE HERDERS_FILES *.h)
file(GLOB_RECURSE SOURECS_FILES *.cpp)

if(CMAKE_BUILD_TYPE  MATCHES "Debug")
SET(DLL_DEBUG_POSTFIX "")  
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
${BIN_DIR}
)
														
#生成动态库
add_library(${proName}  SHARED)
target_sources(${proName} PRIVATE ${SOURECS_FILES} ${HERDERS_FILES})
#将动态库名字设为add
target_compile_definitions(${proName} PRIVATE LIB_XSIGNAL_DLL)
set_target_properties(${proName} PROPERTIES OUTPUT_NAME "xsignal")
set_target_properties(${proName} PROPERTIES VERSION 1.0 SOVERSION 1)

target_link_directories(${proName} PRIVATE ${LIB_DIRECTORIES})											#库目录
target_include_directories(${proName} PRIVATE ${LIB_INCLUDE_DIRECTORY})							#包含目录
target_link_libraries(${proName} ${LIB_NAMES})		

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