SET(proName xopengl_example)
SET(libName xopengl)

include(${CMAKE_SOURCE_DIR}/config_cmake/public.cmake)
if(CMAKE_BUILD_TYPE  MATCHES "Debug")
SET(DLL_DEBUG_POSTFIX "_d")  
else() 
SET(DLL_DEBUG_POSTFIX)
endif()

get_property(ALL_LIBS GLOBAL PROPERTY MY_PROJECT_LIBS)
set(MAPPED_LIBS "")
foreach(lib ${ALL_LIBS})
     list(APPEND MAPPED_LIBS ${lib}${DLL_DEBUG_POSTFIX})
endforeach()

file(GLOB_RECURSE HEADERS_FILES *.h *.hpp)
file(GLOB_RECURSE SOURCES_FILES *.cpp *.c)

add_executable(${proName} xopengl_test.cpp)
set_target_properties(${proName} PROPERTIES FOLDER ${EXAMPLES_TOP_FOLDER})

# 将目标添加到全局属性
set_property(GLOBAL APPEND PROPERTY MY_EXAMPLE_TARGETS ${proName})

get_property(ALL_LIB_TARGETS GLOBAL PROPERTY MY_PROJECT_LIB_TARGETS)

add_dependencies(${proName} ${ALL_LIB_TARGETS})

SET(LIB_NAMES 	
	PRIVATE MyLib::HeaderOnly
	MyLib::example
	MyLib::HDF5
	${libName}${DLL_DEBUG_POSTFIX}
	glew32
	opengl32
	XUtilty${DLL_DEBUG_POSTFIX}
	xshader${DLL_DEBUG_POSTFIX}
	stbImage${DLL_DEBUG_POSTFIX}
	dataBase${DLL_DEBUG_POSTFIX}
	xtest${DLL_DEBUG_POSTFIX}
	xlog${DLL_DEBUG_POSTFIX}

)

SET(LIB_INCLUDE_DIRECTORY 	
														${CMAKE_SOURCE_DIR}/source 
														${CMAKE_SOURCE_DIR}/3rdParty/Eigen3
														${CMAKE_SOURCE_DIR}/3rdParty/glew/include		
)

set(LIB_DIRECTORIES 
${CMAKE_SOURCE_DIR}/output/bin
)

target_link_directories(${proName} PRIVATE ${LIB_DIRECTORIES})	
target_include_directories(${proName} PRIVATE ${LIB_INCLUDE_DIRECTORY})				#包含目录
target_link_libraries(${proName} PRIVATE  ${LIB_NAMES})