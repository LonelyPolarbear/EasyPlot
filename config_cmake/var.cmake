set(SOURCES_FILES)																								#ͷ�ļ�·��
set(HERDERS_FILES)																								#Դ�ļ�·��
set(LIB_INCLUDE_DIRECTORY)																				#����Ŀ¼
set(LIB_NAMES)																										#lib����
set(LIB_DIRECTORIES)																							#lib����Ŀ¼
set(ALL_SOURCES)																							#lib����Ŀ¼


if(CMAKE_BUILD_TYPE  MATCHES "Debug")
SET(DLL_DEBUG_POSTFIX)  
else() 
SET(DLL_DEBUG_POSTFIX)
endif()
#
list(APPEND LIB_NAMES MyLib::HeaderOnly MyLib::HDF5) 