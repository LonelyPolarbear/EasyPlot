#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "hdf5-static" for configuration "Debug"
set_property(TARGET hdf5-static APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(hdf5-static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libhdf5_D.lib"
  )

list(APPEND _cmake_import_check_targets hdf5-static )
list(APPEND _cmake_import_check_files_for_hdf5-static "${_IMPORT_PREFIX}/lib/libhdf5_D.lib" )

# Import target "hdf5-shared" for configuration "Debug"
set_property(TARGET hdf5-shared APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(hdf5-shared PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/hdf5_D.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/hdf5_D.dll"
  )

list(APPEND _cmake_import_check_targets hdf5-shared )
list(APPEND _cmake_import_check_files_for_hdf5-shared "${_IMPORT_PREFIX}/lib/hdf5_D.lib" "${_IMPORT_PREFIX}/bin/hdf5_D.dll" )

# Import target "mirror_server" for configuration "Debug"
set_property(TARGET mirror_server APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(mirror_server PROPERTIES
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/mirror_server.exe"
  )

list(APPEND _cmake_import_check_targets mirror_server )
list(APPEND _cmake_import_check_files_for_mirror_server "${_IMPORT_PREFIX}/bin/mirror_server.exe" )

# Import target "mirror_server_stop" for configuration "Debug"
set_property(TARGET mirror_server_stop APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(mirror_server_stop PROPERTIES
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/mirror_server_stop.exe"
  )

list(APPEND _cmake_import_check_targets mirror_server_stop )
list(APPEND _cmake_import_check_files_for_mirror_server_stop "${_IMPORT_PREFIX}/bin/mirror_server_stop.exe" )

# Import target "hdf5_hl-static" for configuration "Debug"
set_property(TARGET hdf5_hl-static APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(hdf5_hl-static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libhdf5_hl_D.lib"
  )

list(APPEND _cmake_import_check_targets hdf5_hl-static )
list(APPEND _cmake_import_check_files_for_hdf5_hl-static "${_IMPORT_PREFIX}/lib/libhdf5_hl_D.lib" )

# Import target "hdf5_hl-shared" for configuration "Debug"
set_property(TARGET hdf5_hl-shared APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(hdf5_hl-shared PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/hdf5_hl_D.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/hdf5_hl_D.dll"
  )

list(APPEND _cmake_import_check_targets hdf5_hl-shared )
list(APPEND _cmake_import_check_files_for_hdf5_hl-shared "${_IMPORT_PREFIX}/lib/hdf5_hl_D.lib" "${_IMPORT_PREFIX}/bin/hdf5_hl_D.dll" )

# Import target "hdf5_cpp-static" for configuration "Debug"
set_property(TARGET hdf5_cpp-static APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(hdf5_cpp-static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libhdf5_cpp_D.lib"
  )

list(APPEND _cmake_import_check_targets hdf5_cpp-static )
list(APPEND _cmake_import_check_files_for_hdf5_cpp-static "${_IMPORT_PREFIX}/lib/libhdf5_cpp_D.lib" )

# Import target "hdf5_cpp-shared" for configuration "Debug"
set_property(TARGET hdf5_cpp-shared APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(hdf5_cpp-shared PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/hdf5_cpp_D.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/hdf5_cpp_D.dll"
  )

list(APPEND _cmake_import_check_targets hdf5_cpp-shared )
list(APPEND _cmake_import_check_files_for_hdf5_cpp-shared "${_IMPORT_PREFIX}/lib/hdf5_cpp_D.lib" "${_IMPORT_PREFIX}/bin/hdf5_cpp_D.dll" )

# Import target "hdf5_hl_cpp-static" for configuration "Debug"
set_property(TARGET hdf5_hl_cpp-static APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(hdf5_hl_cpp-static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libhdf5_hl_cpp_D.lib"
  )

list(APPEND _cmake_import_check_targets hdf5_hl_cpp-static )
list(APPEND _cmake_import_check_files_for_hdf5_hl_cpp-static "${_IMPORT_PREFIX}/lib/libhdf5_hl_cpp_D.lib" )

# Import target "hdf5_hl_cpp-shared" for configuration "Debug"
set_property(TARGET hdf5_hl_cpp-shared APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(hdf5_hl_cpp-shared PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/hdf5_hl_cpp_D.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/hdf5_hl_cpp_D.dll"
  )

list(APPEND _cmake_import_check_targets hdf5_hl_cpp-shared )
list(APPEND _cmake_import_check_files_for_hdf5_hl_cpp-shared "${_IMPORT_PREFIX}/lib/hdf5_hl_cpp_D.lib" "${_IMPORT_PREFIX}/bin/hdf5_hl_cpp_D.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
