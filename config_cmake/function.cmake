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

endif (MSVC)