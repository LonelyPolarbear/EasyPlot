SET(proName xutilty_example)
SET(libName xutilty)

include(${CMAKE_SOURCE_DIR}/config_cmake/public.cmake)
if(CMAKE_BUILD_TYPE  MATCHES "Debug")
SET(DLL_DEBUG_POSTFIX "_d")  
else() 
SET(DLL_DEBUG_POSTFIX)
endif()

add_executable(${proName} xutilty_test.cpp)
set_target_properties(${proName} PROPERTIES FOLDER ${EXAMPLES_TOP_FOLDER})

# 将目标添加到全局属性
set_property(GLOBAL APPEND PROPERTY MY_EXAMPLE_TARGETS ${proName})

get_property(ALL_LIB_TARGETS GLOBAL PROPERTY MY_PROJECT_LIB_TARGETS)

add_dependencies(${proName} ${ALL_LIB_TARGETS})

SET(LIB_INCLUDE_DIRECTORY 	
														${CMAKE_SOURCE_DIR}/source 
														${CMAKE_SOURCE_DIR}/3rdParty/Eigen3
)

SET(LIB_NAMES 	
	${libName}${DLL_DEBUG_POSTFIX}
	MyLib::HeaderOnly
)

#三方库
set(LIB_DIRECTORIES 
${CMAKE_SOURCE_DIR}/output/bin
)


target_link_directories(${proName} PRIVATE ${LIB_DIRECTORIES})	
target_include_directories(${proName} PRIVATE ${LIB_INCLUDE_DIRECTORY})
target_link_libraries(${proName} PRIVATE  ${LIB_NAMES})