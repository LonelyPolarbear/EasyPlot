SET(proName xlog_example)
SET(libName xlog)

#set(PORTABLE_PYTHON_ROOT "${CMAKE_SOURCE_DIR}/Python38")
#set(Python_EXECUTABLE "${PORTABLE_PYTHON_ROOT}/python.exe") 
#find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)

include(${CMAKE_SOURCE_DIR}/config_cmake/public.cmake)
if(CMAKE_BUILD_TYPE  MATCHES "Debug")
SET(DLL_DEBUG_POSTFIX "")  
else() 
SET(DLL_DEBUG_POSTFIX)
endif()

get_property(ALL_LIBS GLOBAL PROPERTY MY_PROJECT_LIBS)
set(MAPPED_LIBS "")
foreach(lib ${ALL_LIBS})
     list(APPEND MAPPED_LIBS ${lib}${DLL_DEBUG_POSTFIX})
endforeach()

add_executable(${proName} xlog_test.cpp)
#target_compile_options(${proName} PRIVATE /utf-8)
set_target_properties(${proName} PROPERTIES FOLDER ${EXAMPLES_TOP_FOLDER})

# 将目标添加到全局属性
set_property(GLOBAL APPEND PROPERTY MY_EXAMPLE_TARGETS ${proName})

get_property(ALL_LIB_TARGETS GLOBAL PROPERTY MY_PROJECT_LIB_TARGETS)

add_dependencies(${proName} ${ALL_LIB_TARGETS})

SET(LIB_NAMES 	
	PRIVATE MyLib::HeaderOnly
	MyLib::HDF5
	XUtilty${DLL_DEBUG_POSTFIX}
	dataBase${DLL_DEBUG_POSTFIX}
	xtest${DLL_DEBUG_POSTFIX}
	xlog${DLL_DEBUG_POSTFIX}
	${libName}${DLL_DEBUG_POSTFIX}
	#python38
)

SET(LIB_INCLUDE_DIRECTORY 	
														${CMAKE_SOURCE_DIR}/source 
														${CMAKE_SOURCE_DIR}/3rdParty/Eigen3
														${CMAKE_SOURCE_DIR}/3rdParty/spdlog/include
														#${CMAKE_SOURCE_DIR}/Python38/include
														
)

set(LIB_DIRECTORIES 
${CMAKE_SOURCE_DIR}/output/bin
#${CMAKE_SOURCE_DIR}/Python38/libs
)

#target_compile_definitions(${proName} PRIVATE SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)
target_link_directories(${proName} PRIVATE ${LIB_DIRECTORIES})	
target_include_directories(${proName} PRIVATE ${LIB_INCLUDE_DIRECTORY})				#包含目录
target_link_libraries(${proName} PRIVATE  ${LIB_NAMES})