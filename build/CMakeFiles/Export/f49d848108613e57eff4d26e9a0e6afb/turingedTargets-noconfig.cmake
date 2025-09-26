#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "turinged::turinged" for configuration ""
set_property(TARGET turinged::turinged APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(turinged::turinged PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "CXX"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libturinged.a"
  )

list(APPEND _cmake_import_check_targets turinged::turinged )
list(APPEND _cmake_import_check_files_for_turinged::turinged "${_IMPORT_PREFIX}/lib/libturinged.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
