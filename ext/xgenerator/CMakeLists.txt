cmake_minimum_required(VERSION 3.8)
if(POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
endif()
# -------------------
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # 生成编译命令数据库,需要在project之前设置,MSVC下不生效
project(xgenerator)		                                                        #放在根目录下可以理解为解决方案的名称



#--------------------------------------------------------------------------------------------------------------
# 以下是各个库的CMakeLists.txt文件，按照顺序依次添加
#--------------------------------------------------------------------------------------------------------------
add_subdirectory(src/xmetagenerator)
add_subdirectory(src/scanjson)