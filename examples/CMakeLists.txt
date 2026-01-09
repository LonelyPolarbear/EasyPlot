
set(EXAMPLES_TOP_FOLDER "Examples")

#add_subdirectory(dataBase)
# 遍历目录添加所有示例项目
file(GLOB child_entries LIST_DIRECTORIES true RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)

foreach(entry ${child_entries})
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${entry})
        message(STATUS "Found directory: ${entry}")
        add_subdirectory(${entry})
    endif()
endforeach()


if(CMAKE_BUILD_TYPE  MATCHES "Debug")
SET(DLL_DEBUG_POSTFIX "_d")  
else() 
SET(DLL_DEBUG_POSTFIX)
endif()
# 获取所有库目标
get_property(ALL_LIB_TARGETS GLOBAL PROPERTY MY_PROJECT_LIB_TARGETS)
get_property(ALL_LIBS GLOBAL PROPERTY MY_PROJECT_LIBS)
get_property(ALL_EXAMPLE_TARGET GLOBAL PROPERTY MY_EXAMPLE_TARGETS)

#[[
set(MAPPED_LIBS "")
foreach(lib ${ALL_LIBS})
     list(APPEND MAPPED_LIBS ${lib}${DLL_DEBUG_POSTFIX})
endforeach()
]]

# 可选：创建一个聚合目标用于构建所有示例
if(MSVC OR CMAKE_GENERATOR MATCHES "Visual Studio")
   add_custom_target(BuildAllExamples)
   set_target_properties(BuildAllExamples PROPERTIES FOLDER "${EXAMPLES_TOP_FOLDER}")

message("ALL_LIBS: ${ALL_LIBS}")
message("ALL_LIB_TARGETS: ${ALL_LIB_TARGETS}")
message("ALL_EXAMPLE_TARGET: ${ALL_EXAMPLE_TARGET}")
add_dependencies(BuildAllExamples ${ALL_EXAMPLE_TARGET})
endif()