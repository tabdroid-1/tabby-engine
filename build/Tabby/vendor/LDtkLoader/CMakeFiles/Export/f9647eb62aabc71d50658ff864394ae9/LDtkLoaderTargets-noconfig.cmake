#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "LDtkLoader" for configuration ""
set_property(TARGET LDtkLoader APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(LDtkLoader PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "CXX"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libLDtkLoader.a"
  )

list(APPEND _cmake_import_check_targets LDtkLoader )
list(APPEND _cmake_import_check_files_for_LDtkLoader "${_IMPORT_PREFIX}/lib/libLDtkLoader.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
