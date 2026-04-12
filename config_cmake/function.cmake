if (MSVC)
function(assign_source_group)
    foreach(_source IN ITEMS ${ARGN})	
		if(_source MATCHES "ui_.*" OR _source MATCHES "moc_.*" OR _source MATCHES "qrc_.*" OR _source MATCHES ".*cpp.rule")
			source_group("moc" FILES "${_source}")
		#ui文件
		elseif(_source MATCHES ".*.ui" OR _source MATCHES ".*.qrc")
			source_group("src\\moc" FILES "${_source}")
		else()
			if (IS_ABSOLUTE "${_source}")
				file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
			else()
				set(_source_rel "${_source}")
			endif()
			get_filename_component(_source_path "${_source_rel}" PATH)
			string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
			source_group("src\\${_source_path_msvc}" FILES "${_source}")
		endif()   
    endforeach()
endfunction(assign_source_group)

function(my_add_executable first_arg)
	#第一个参数不处理
    foreach(_source IN ITEMS ${ARGN})
        assign_source_group(${_source})
    endforeach()
    add_executable(${ARGV})
endfunction(my_add_executable)


function(group_source_files_by_directory)
    # 接收文件列表作为参数
    set(files ${ARGN})
    
    foreach(file ${files})
        # 获取文件的完整路径
        get_filename_component(file_path ${file} ABSOLUTE)
        
        # 获取文件相对于项目源目录的路径
        file(RELATIVE_PATH relative_path ${CMAKE_SOURCE_DIR} ${file_path})
        
        # 获取目录部分
        get_filename_component(dir_part ${relative_path} DIRECTORY)
        
        # 获取源文件所在相对目录（从当前目录开始）
        file(RELATIVE_PATH relative_to_current ${CMAKE_CURRENT_SOURCE_DIR} ${file})
        get_filename_component(dir_part_current ${relative_to_current} DIRECTORY)
        
        # 确定分组路径
        if(dir_part_current)
            # 构建分组路径（使用\\作为分隔符）
            string(REPLACE "/" "\\\\" group_path "${dir_part_current}")
            set(group_path "\\${group_path}")
        else()
            # 当前目录的文件
            set(group_path "\\")
        endif()
        
        # 设置source_group
        source_group("${group_path}" FILES ${file})
    endforeach()
endfunction()


# utils.cmake （或直接写在顶层 CMakeLists.txt 中）
function(generate_dllmain_if_needed TARGET_NAME)
    # 1. 获取目标类型 EXECUTABLE STATIC_LIBRARY SHARED_LIBRARY MODULE_LIBRARY INTERFACE_LIBRAR OBJECT_LIBRARY
    get_target_property(TARGET_TYPE ${TARGET_NAME} TYPE)

    # 2. 仅对共享库处理（且仅限 Windows）
    if(NOT WIN32)
        return()
    endif()

    if(NOT TARGET_TYPE STREQUAL "SHARED_LIBRARY")
        return()
    endif()

    # 获取目标源码目录
    #SOURCE_DIR 是 CMake 内置的目标属性（Target Property），表示：该目标被定义时所在的源码目录（即包含其 CMakeLists.txt 的目录）
    get_target_property(TARGET_SOURCE_DIR ${TARGET_NAME} SOURCE_DIR)

    set(DLLMAIN_PATH "${TARGET_SOURCE_DIR}/dllmain.cpp")

    # 检查 dllmain.cpp 是否已存在
    if(NOT EXISTS "${DLLMAIN_PATH}")
        message(STATUS "Generating dllmain.cpp for target: ${TARGET_NAME}")
        # === 设置你要替换的变量 ===
        set(DLL_NAME "${TARGET_NAME}")
    
        # 模板文件路径（假设放在 cmake/templates/ 下）
        set(TEMPLATE_FILE "${CMAKE_SOURCE_DIR}/config_cmake/templates/dllmain.cpp.in")
         configure_file(
            "${TEMPLATE_FILE}"
            "${DLLMAIN_PATH}"
            @ONLY)
        # 确保被编译
        target_sources(${TARGET_NAME} PRIVATE "${DLLMAIN_PATH}")
    endif()
endfunction()

#变量大小写转换
function(get_dll_api_name TARGET_NAME OUTPUT_VAR)
string(TOUPPER "${TARGET_NAME}" TARGET_UPPER)
set(${OUTPUT_VAR} "${TARGET_UPPER}_API" PARENT_SCOPE)
endfunction()

function(get_dll_api_macro TARGET_NAME OUTPUT_VAR)
string(TOUPPER "${TARGET_NAME}" TARGET_UPPER)
set(${OUTPUT_VAR} "${TARGET_UPPER}_DLL" PARENT_SCOPE)
endfunction()

function(generate_dllapi_if_needed TARGET_NAME)
    # 1. 获取目标类型 EXECUTABLE STATIC_LIBRARY SHARED_LIBRARY MODULE_LIBRARY INTERFACE_LIBRAR OBJECT_LIBRARY
    get_target_property(TARGET_TYPE ${TARGET_NAME} TYPE)

    # 2. 仅对共享库处理（且仅限 Windows）
    if(NOT WIN32)
        return()
    endif()

    if(NOT TARGET_TYPE STREQUAL "SHARED_LIBRARY")
        return()
    endif()

    # 获取目标源码目录
    #SOURCE_DIR 是 CMake 内置的目标属性（Target Property），表示：该目标被定义时所在的源码目录（即包含其 CMakeLists.txt 的目录）
    get_target_property(TARGET_SOURCE_DIR ${TARGET_NAME} SOURCE_DIR)

    set(DLLAPI_PATH "${TARGET_SOURCE_DIR}/${TARGET_NAME}Api.h")

    # 检查 dllmain.cpp 是否已存在
    if(EXISTS "${DLLAPI_PATH}")
        return()
    endif()

     message(STATUS "Generating dllApi.h for target: ${TARGET_NAME}")
     # === 设置你要替换的变量 ===
     #DLL_MACRO_NAME
     #DLL_API_NAME
     string(TOUPPER "${TARGET_NAME}" TARGET_UPPER)
     set(DLL_API_NAME "${TARGET_UPPER}_API")
     set(DLL_MACRO_NAME "${TARGET_UPPER}_DLL")
    
     # 模板文件路径（假设放在 cmake/templates/ 下）
     set(TEMPLATE_FILE "${CMAKE_SOURCE_DIR}/config_cmake/templates/dllapi.h.in")
      configure_file(
         "${TEMPLATE_FILE}"
         "${DLLAPI_PATH}"
         @ONLY)
      target_sources(${TARGET_NAME} PRIVATE "${DLLAPI_PATH}")
endfunction()

#变量大小写转换
function(get_dll_api_name TARGET_NAME OUTPUT_VAR)
string(TOUPPER "${TARGET_NAME}" TARGET_UPPER)
set(${OUTPUT_VAR} "${TARGET_UPPER}_API" PARENT_SCOPE)
endfunction()

function(get_dll_api_macro TARGET_NAME OUTPUT_VAR)
string(TOUPPER "${TARGET_NAME}" TARGET_UPPER)
set(${OUTPUT_VAR} "${TARGET_UPPER}_DLL" PARENT_SCOPE)
endfunction()

function(generate_xscan_if_needed TARGET_NAME)
    # 1. 获取目标类型 EXECUTABLE STATIC_LIBRARY SHARED_LIBRARY MODULE_LIBRARY INTERFACE_LIBRAR OBJECT_LIBRARY
    get_target_property(TARGET_TYPE ${TARGET_NAME} TYPE)


    # 获取目标源码目录
    #SOURCE_DIR 是 CMake 内置的目标属性（Target Property），表示：该目标被定义时所在的源码目录（即包含其 CMakeLists.txt 的目录）
    get_target_property(TARGET_SOURCE_DIR ${TARGET_NAME} SOURCE_DIR)

    set(XSCAN_PATH "${TARGET_SOURCE_DIR}/XScan.h")

    # 检查 dllmain.cpp 是否已存在
    if(EXISTS "${XSCAN_PATH}")
        return()
    endif()

     message(STATUS "Generating XScan.h for target: ${TARGET_NAME}")
     # === 设置你要替换的变量 ===
     #DLL_MACRO_NAME
     #DLL_API_NAME

     # 模板文件路径（假设放在 cmake/templates/ 下）
     set(TEMPLATE_FILE "${CMAKE_SOURCE_DIR}/config_cmake/templates/xscan.h.in")
      configure_file(
         "${TEMPLATE_FILE}"
         "${XSCAN_PATH}"
         @ONLY)
      target_sources(${TARGET_NAME} PRIVATE "${XSCAN_PATH}")
endfunction()

endif (MSVC)