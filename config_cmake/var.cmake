set(SOURCES_FILES)																								#头文件路径
set(HERDERS_FILES)																								#源文件路径
set(LIB_INCLUDE_DIRECTORY)																				#包含目录
set(LIB_NAMES)																										#lib名称
set(LIB_DIRECTORIES)																							#lib包含目录
set(ALL_SOURCES)																							#lib包含目录


if(CMAKE_BUILD_TYPE  MATCHES "Debug")
SET(DLL_DEBUG_POSTFIX)  
else() 
SET(DLL_DEBUG_POSTFIX)
endif()

#
list(APPEND LIB_NAMES MyLib::HeaderOnly MyLib::HDF5) 